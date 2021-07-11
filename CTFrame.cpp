// CTFrame.cpp -  The main window of the CamTest App.

#include <stdio.h>
#include <sys/mman.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <libcamera/control_ids.h>
#include <libcamera/stream.h>
#include <libcamera/formats.h>
#include <libcamera/framebuffer_allocator.h>
#include "CTFrame.h"
#include "ControlWnd.h"
#include "PictWnd.h"

wxDEFINE_EVENT(wxEVT_HAVE_IMG, wxCommandEvent);

#define EVT_HAVE_IMG(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_HAVE_IMG, id, wxID_ANY, \
        wxCommandEventHandler(fn), \
        (wxObject *) NULL \
    ),


BEGIN_EVENT_TABLE(CTFrame, wxFrame)
EVT_BUTTON(ID_SNAP, CTFrame::OnSnap)
EVT_MENU(wxID_EXIT, CTFrame::OnExit)
EVT_HAVE_IMG(wxID_ANY, CTFrame::OnHaveImg)
END_EVENT_TABLE()

// Used to return data from request completed callback to GUI thread

struct ImgInfo
    {
    ImgInfo (unsigned char *puc, int iWth, int iHgt)
	{
	m_puc = puc;
	m_iWth = iWth;
	m_iHgt = iHgt;
	}
    ~ImgInfo () {}
    unsigned char *m_puc;
    int m_iWth;
    int m_iHgt;
    int m_iExp;
    float m_a_gain;
    float m_d_gain;
    float m_r_gain;
    float m_b_gain;
    };

/*** CTFrame ***********************************************

     Constructor. Creates all the child windows.

     Coding history:

     WJB    14/ 6/21    First draft, based upon MovieCap code.

*/

CTFrame::CTFrame (void)
:  wxFrame (NULL, wxID_ANY, wxT("Camera Test Program"), wxDefaultPosition, wxDefaultSize)
    {
    // Create a menu.

    m_menuFile     =  new wxMenu ();
    m_menuFileExit =  m_menuFile->Append (wxID_EXIT, wxT("E&xit"), wxT("Terminate the program"));
    m_menubar      =  new wxMenuBar ();
    m_menubar->Append (m_menuFile, wxT("&File"));
    SetMenuBar (m_menubar);
    CreateStatusBar (1);

    // Add Windows

    m_sizer = new wxBoxSizer (wxHORIZONTAL);
    m_ctrlwnd = new ControlWnd (this);
    m_sizer->Add (m_ctrlwnd, 0);
    m_pictwnd = new PictWnd (this);
    m_sizer->Add (m_pictwnd, 1, wxEXPAND);
    SetSizer (m_sizer);

    // Start camera
    
    m_iWth = 2592;
    m_iHgt = 1944;
    m_frame = 0;
    CamStart ();
    }

/*** ~CTFrame ************************************************************

     Destructor. Cleans up data.

     Coding history:

     WJB    14/ 6/21    First draft, based upon MovieCap code.

*/

CTFrame::~CTFrame (void)
    {
    CamStop ();
    }

/*** OnExit **************************************************************

     Called by File/Exit menu selection. Closes the application.

     Inputs:

     e  =  Command event.

     Coding history:

     WJB    14/ 6/21    First draft, based upon MovieCap code.

*/

void CTFrame::OnExit (wxCommandEvent &e)
    {
    CamStop ();
    Close ();
    }

/*** CamStart ************************************************************************************************

Configure and start camera.

*/

void CTFrame::CamStart (void)
    {
    printf ("CamStart\n");

    // Get camera
    
    m_camera_manager = std::make_unique<libcamera::CameraManager>();
    int ret = m_camera_manager->start();
    if (ret)
      throw std::runtime_error("camera manager failed to start, code " + std::to_string(-ret));
    printf ("Started camera manager\n");

    if (m_camera_manager->cameras().size() == 0)
      throw std::runtime_error("no cameras available");

    std::string const &cam_id = m_camera_manager->cameras()[0]->id();
    m_camera = m_camera_manager->get(cam_id);
    if (!m_camera)
      throw std::runtime_error("failed to find camera " + cam_id);

    if (m_camera->acquire())
      throw std::runtime_error("failed to acquire camera " + cam_id);
    m_camera_acquired = true;
    printf ("Camera acquired\n");

    // Configure
    
    libcamera::StreamRoles stream_roles = { libcamera::StreamRole::StillCapture };
    m_configuration = m_camera->generateConfiguration(stream_roles);
    if (!m_configuration)
	throw std::runtime_error("failed to generate still capture configuration");
    libcamera::StreamConfiguration &scfg = (*m_configuration)[0];
    scfg.pixelFormat = libcamera::formats::BGR888;
    scfg.size.width = m_iWth;
    scfg.size.height = m_iHgt;
    m_configuration->transform = libcamera::Transform::Identity;
    libcamera::CameraConfiguration::Status validation = m_configuration->validate();
    if (validation == libcamera::CameraConfiguration::Invalid)
	throw std::runtime_error("failed to valid stream configurations");
    if (m_camera->configure(m_configuration.get()) < 0)
	throw std::runtime_error("failed to configure streams");
    printf ("Camera stream configured\n");

    m_allocator = new libcamera::FrameBufferAllocator(m_camera);
    libcamera::Stream *stream = scfg.stream();
    if (m_allocator->allocate(stream) < 0)
	throw std::runtime_error("failed to allocate capture buffers");

    for (const std::unique_ptr<libcamera::FrameBuffer> &buffer : m_allocator->buffers(stream))
	{
	printf ("FrameBuffer contains %d planes\n", buffer->planes().size());
	for (unsigned i = 0; i < buffer->planes().size(); i++)
	    {
	    const libcamera::FrameBuffer::Plane &plane = buffer->planes()[i];
	    void *memory = mmap(NULL, plane.length, PROT_READ, MAP_SHARED, plane.fd.fd(), 0);
	    printf ("Memory for plane mapped at %p\n", memory);
	    m_mapped_buffers[buffer.get()].push_back(memory);
	    }
	m_frame_buffers[stream].push(buffer.get());
	std::unique_ptr<libcamera::Request> request = m_camera->createRequest();
	if (!request)
	    throw std::runtime_error("failed to make request");
	if (request->addBuffer(stream, buffer.get()) < 0)
	    throw std::runtime_error("failed to add buffer to request");
	m_requests.push_back(std::move(request));
	m_free_req.push (m_requests.back ().get());
	}
    m_camera->requestCompleted.connect(this, &CTFrame::DoneSnap);
    libcamera::ControlList controls;
    m_ctrlwnd->ApplyControls (controls);
    printf ("Camera controls appled\n");
    if (m_camera->start(&controls))
	throw std::runtime_error("failed to start camera");
    printf ("Camera running\n");
    m_bRunning = true;
    }

/*** CamStop ***********************************************************************************************

Stop camera.

*/

void CTFrame::CamStop (void)
    {
    printf ("CamStop\n");
    if ( m_bRunning )
        {
	printf ("Stop camera\n");
	if (m_camera->stop())
	    printf("Failed to stop camera\n");
        }
    m_bRunning = false;

    if ( m_camera )
	{
	printf ("Disconnect completion callback\n");
	m_camera->requestCompleted.disconnect(this, &CTFrame::DoneSnap);
	}

    printf ("Clear requests\n");
    m_requests.clear ();

    for (auto &iter : m_mapped_buffers)
	{
	assert(iter.first->planes().size() == iter.second.size());
	for (unsigned i = 0; i < iter.first->planes().size(); i++)
	    {
	    printf ("unmap %p, length = %d\n", iter.second[i], iter.first->planes()[i].length);
	    munmap(iter.second[i], iter.first->planes()[i].length);
	    }
	}
    m_mapped_buffers.clear();
    m_frame_buffers.clear();

    if ( m_allocator )
	{
	printf ("Delete FrameBufferAllocator\n");
	delete m_allocator;
	m_allocator = nullptr;
	}

    if ( m_configuration )
	{
	printf ("Release camera configuration\n");
	m_configuration.reset();
	}
    
    if (m_camera_acquired)
	{
	printf ("Release camera\n");
	m_camera->release();
	}
    m_camera_acquired = false;
    
    if ( m_camera )
	{
	printf ("Reset camera\n");
	m_camera.reset();
	}

    if ( m_camera_manager )
	{
	printf ("Reset camera manager\n");
	m_camera_manager.reset();
	}
    printf ("Camera stopped\n");
    }

/*** CamReq ************************************************************************************************

Request a frame

*/

void CTFrame::CamReq (void)
    {
    printf ("CamReq\n");
    if ( ! m_free_req.empty () )
	{
	libcamera::Request *req = m_free_req.front ();
	m_free_req.pop ();
	m_ctrlwnd->ApplyControls (req->controls ());
	printf ("Controls appled\n");
	int iSta = m_camera->queueRequest (req);
	if ( iSta == 0 )
	    {
	    printf ("Request queued\n");
	    }
	else
	    {
	    printf ("Error %d queueing request\n", iSta);
	    GetStatusBar()->SetStatusText(wxString::Format ("Error %d queueing request\n", iSta));
	    }
	}
    else
	{
	printf ("No free request buffers\n");
	GetStatusBar()->SetStatusText("No free request buffers");
	}
    }

/*** OnSnap ************************************************************************************************

Request an image, applying camera settings.

*/

void CTFrame::OnSnap (wxCommandEvent &e)
    {
    printf ("OnSnap\n");
    CamReq ();
    GetStatusBar()->SetStatusText("Requested new Image");
    }

/*** DoneSnap ***********************************************************************************************

Request completed callback. Not a GUI context so need to queue updates.

*/

void CTFrame::DoneSnap (libcamera::Request *req)
    {
    // Get callback data
    
    printf ("DoneSnap\n");
    libcamera::Request::BufferMap::const_iterator bmapi = req->buffers().begin();
    if ( bmapi == req->buffers().end () ) return;
    const libcamera::StreamConfiguration &scfg = bmapi->first->configuration();
    const libcamera::FrameBuffer *frame = bmapi->second;
    if ( frame == nullptr ) return;
    const void * mem = m_mapped_buffers[frame][0];
    if ( mem == nullptr ) return;
    int len = frame->planes()[0].length;
    int iWth = scfg.size.width;
    int iHgt = scfg.size.height;
    int iStr = scfg.stride;

    // Extract image (scale if requested).
    
    printf ("wth = %d, hgt = %d, stride = %d, mem = %p\n", iWth, iHgt, iStr, mem);
    unsigned char *pucImg;;
    int nScale = m_ctrlwnd->ImgScale ();
    if ( nScale == 1 )
	{
	pucImg = (unsigned char *) malloc (len);
	memcpy (pucImg, mem, len);
	}
    else
        {
	pucImg = (unsigned char *) mem;
        iWth /= nScale;
        iHgt /= nScale;
        unsigned char *pucScl = (unsigned char *) malloc (3 * iWth * iHgt);
        unsigned char *pucPix = pucScl;
        for ( int i = 0; i < iHgt; ++i )
            {
            for ( int j = 0; j < iWth; ++j )
                {
		for ( int k = 0; k < 3; ++k )
		    {
		    int iVal = 0;
		    for ( int ii = 0; ii < nScale; ++ii )
			{
			for ( int jj = 0; jj < nScale; ++jj )
			    {
			    iVal += pucImg[(nScale * i + ii) * iStr + 3 * (nScale * j + jj) + k];
			    }
			}
		    *pucPix = iVal / ( nScale * nScale );
		    ++pucPix;
		    }
                }
            }
        pucImg = pucScl;
        }
    printf ("Queueing HaveImage event\n");
    wxCommandEvent *e = new wxCommandEvent(wxEVT_HAVE_IMG);
    ImgInfo *pii = new ImgInfo(pucImg, iWth, iHgt);
    pii->m_iExp = req->metadata().get(libcamera::controls::ExposureTime);
    pii->m_a_gain = req->metadata().get(libcamera::controls::AnalogueGain);
    pii->m_d_gain = req->metadata().get(libcamera::controls::DigitalGain);
    libcamera::Span<const float> gains = req->metadata().get(libcamera::controls::ColourGains);
    if ( gains.size() == 2 )
	{
	pii->m_r_gain = gains[0];
	pii->m_b_gain = gains[1];
	}
    else
	{
	pii->m_r_gain = -1.0;
	pii->m_b_gain = -1.0;
	}
    e->SetClientData (pii);
    wxQueueEvent (this, e);

    // Log all image details

    ++m_frame;
    // const libcamera::ControlIdMap &idmap = m_camera->controls().idmap();
    const libcamera::ControlIdMap &idmap = libcamera::controls::controls;
    printf ("Frame %d Request controls:\n", m_frame);
    for (std::pair<const unsigned int, libcamera::ControlValue> cli : req->controls())
	{
	// const std::string name = idmap.at(cli.first)->name();
	std::string name = "Unknown";
	libcamera::ControlIdMap::const_iterator it = idmap.find (cli.first);
	if ( it != idmap.end () ) name = it->second->name();
	std::string val = cli.second.toString();
	printf ("   Req %s (%d) = %s\n", name.c_str (), cli.first, val.c_str ());
	}
    printf ("Frame %d Request metadata:\n", m_frame);
    for (std::pair<const unsigned int, libcamera::ControlValue> cli : req->metadata())
	{
	// const std::string name = idmap.at(cli.first)->name();
	std::string name = "Unknown";
	libcamera::ControlIdMap::const_iterator it = idmap.find (cli.first);
	if ( it != idmap.end () ) name = it->second->name();
	std::string val = cli.second.toString();
	printf ("   Have %s (%d) = %s\n", name.c_str (), cli.first, val.c_str ());
	}
    req->reuse (libcamera::Request::ReuseBuffers);
    m_free_req.push (req);
    }

/*** OnHaveImage ********************************************************************************************

Update display with new image

*/

void CTFrame::OnHaveImg (wxCommandEvent &e)
    {
    printf ("OnHaveImage\n");
    ImgInfo *pii = (ImgInfo *) e.GetClientData ();
    wxImage *pimg = new wxImage (pii->m_iWth, pii->m_iHgt, pii->m_puc);
    m_pictwnd->SetImage (pimg);
    GetStatusBar()->SetStatusText(wxString::Format("Frame %d Exp %d AG %4.2f DG %4.2f RG %4.2f BG %4.2f",
	    m_frame, pii->m_iExp, pii->m_a_gain, pii->m_d_gain, pii->m_r_gain, pii->m_b_gain));
    delete pii;
    if ( m_bRunning && m_ctrlwnd->RunCamera () ) CamReq ();
    }
