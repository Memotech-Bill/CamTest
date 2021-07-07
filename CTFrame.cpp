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

BEGIN_EVENT_TABLE(CTFrame, wxFrame)
EVT_BUTTON(ID_SNAP, CTFrame::OnSnap)
EVT_MENU(wxID_EXIT, CTFrame::OnExit)
END_EVENT_TABLE()

static CTFrame *pframe = NULL;

/*** CTFrame ***********************************************

     Constructor. Creates all the child windows.

     Coding history:

     WJB    14/ 6/21    First draft, based upon MovieCap code.

*/

CTFrame::CTFrame (void)
:  wxFrame (NULL, wxID_ANY, wxT("Camera Test Program"), wxDefaultPosition, wxDefaultSize)
    {
    // Initialise variables.
    pframe = this;

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

    Config ();
    }

/*** ~CTFrame ************************************************************

     Destructor. Cleans up data.

     Coding history:

     WJB    14/ 6/21    First draft, based upon MovieCap code.

*/

CTFrame::~CTFrame (void)
    {
    if ( m_bRunning )
        {
	printf ("Stop camera\n");
	if (m_camera->stop())
	    throw std::runtime_error("failed to stop camera");
        }
    m_bRunning = false;
    
    if (m_camera_acquired)
	{
	printf ("Release camera\n");
	m_camera->release();
	}
    m_camera_acquired = false;

    printf ("Reset camera\n");
    m_camera.reset();

    printf ("Reset camera manager\n");
    m_camera_manager.reset();
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
    Close ();
    }

/*** Config ************************************************************************************************

Configure camera.

*/

void CTFrame::Config (void)
    {

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
    scfg.pixelFormat = libcamera::formats::RGB888;
    scfg.size.width = 2592;
    scfg.size.height = 1944;
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
    m_ctrlwnd->ApplyControls (m_controls);
    printf ("Camera controls appled\n");
    if (m_camera->start(&m_controls))
	throw std::runtime_error("failed to start camera");
    printf ("Camera running\n");
    m_bRunning = true;
    }


/*** OnSnap ************************************************************************************************

Request an image, applying camera settings.

*/

void CTFrame::OnSnap (wxCommandEvent &e)
    {
    printf ("OnSnap\n");
    if ( ! m_free_req.empty () )
	{
	libcamera::Request *req = m_free_req.front ();
	m_free_req.pop ();
	m_ctrlwnd->ApplyControls (req->controls ());
	printf ("Controls appled\n");
	m_camera->queueRequest (req);
	printf ("Request queued\n");
	}
    }

/*** DoneSnap ***********************************************************************************************

Capture an image and display result.

*/

void CTFrame::DoneSnap (libcamera::Request *req)
    {
    printf ("DoneSnap\n");
    libcamera::Request::BufferMap::const_iterator bmapi = req->buffers().begin();
    const libcamera::StreamConfiguration &scfg = bmapi->first->configuration();
    const libcamera::FrameBuffer *frame = bmapi->second;
    const void * mem = m_mapped_buffers[frame][0];
    int len = frame->planes()[0].length;
    int iWth = scfg.size.width;
    int iHgt = scfg.size.height;
    int iStr = scfg.stride;
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
            for ( int j = 0; j < 3 * iWth; ++j )
                {
                int iVal = 0;
                for ( int ii = 0; ii < nScale; ++ii )
                    {
                    for ( int jj = 0; jj < nScale; ++jj )
                        {
                        iVal += pucImg[(nScale * i + ii) * iStr + nScale * j + jj];
                        }
                    }
                *pucPix = iVal / ( nScale * nScale );
                ++pucPix;
                }
            }
        pucImg = pucScl;
        }
    wxImage *pimg = new wxImage (iWth, iHgt, pucImg);
    m_pictwnd->SetImage (pimg);
    printf ("SetImage\nRequest metadata:\n");
    // const libcamera::ControlIdMap &idmap = m_camera->controls().idmap();
    const libcamera::ControlIdMap &idmap = libcamera::controls::controls;
    for (std::pair<const unsigned int, libcamera::ControlValue> cli : req->metadata())
	{
	// const std::string name = idmap.at(cli.first)->name();
	std::string name = "Unknown";
	libcamera::ControlIdMap::const_iterator it = idmap.find (cli.first);
	if ( it != idmap.end () ) name = it->second->name();
	std::string val = cli.second.toString();
	printf ("%s (%d) = %s\n", name.c_str (), cli.first, val.c_str ());
	}
    req->reuse (libcamera::Request::ReuseBuffers);
    m_free_req.push (req);
    }

/*** ShowGains **********************************************************************************************

 */

void CTFrame::ShowGains (int exp, float ag, float dg, float rg, float bg)
    {
    // printf ("Exp = %d us, AG = %5.3f, DG = %5.3f, RG = %5.3f, BG = %5.3f\n",
    //     exp, ag, dg, rg, bg);
    if ( pframe )
        {
        pframe->m_exp = exp;
        pframe->m_ag = ag;
        pframe->m_dg = dg;
        pframe->m_rg = rg;
        pframe->m_bg = bg;
        }
    }
