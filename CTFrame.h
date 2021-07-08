// CTFrame.h  -  The main window of the CamTest App.

#ifndef  H_MCFRAME
#define  H_MCFRAME

#include <memory>
#include <map>
#include <vector>
#include <queue>
#include <wx/wxprec.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <libcamera/camera_manager.h>
#include <libcamera/camera.h>

class ControlWnd;
class PictWnd;

class CTFrame : public wxFrame
    {
public:
    CTFrame ();
    ~CTFrame ();
    void OnExit (wxCommandEvent &e);
    void CamStart (void);
    void CamStop (void);
    void OnSnap (wxCommandEvent &e);
    void DoneSnap (libcamera::Request *req);
    void OnHaveImg (wxCommandEvent &e);

private:
    wxMenuBar *     m_menubar;
    wxMenu *        m_menuFile;
    wxMenuItem *    m_menuFileExit;
    wxBoxSizer *    m_sizer;
    ControlWnd *    m_ctrlwnd;
    PictWnd *       m_pictwnd;
    bool            m_bRunning;
    int             m_iWth;
    int             m_iHgt;
    std::unique_ptr<libcamera::CameraManager> m_camera_manager;
    std::shared_ptr<libcamera::Camera> m_camera;
    bool m_camera_acquired;
    std::unique_ptr<libcamera::CameraConfiguration> m_configuration;
    libcamera::FrameBufferAllocator *m_allocator = nullptr;
    std::map<const libcamera::FrameBuffer *, std::vector<void *>> m_mapped_buffers;
    std::map<libcamera::Stream *, std::queue<libcamera::FrameBuffer *>> m_frame_buffers;
    std::vector<std::unique_ptr<libcamera::Request>> m_requests;
    std::queue<libcamera::Request *> m_free_req;
    libcamera::ControlList m_controls;

    DECLARE_EVENT_TABLE()
    };

#endif
