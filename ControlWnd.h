// ControlWnd.h  -  A window for containing camera controls.

#ifndef  H_CONTROLWND
#define  H_CONTROLWND

#define ID_SNAP     101

#include <string>
#include <vector>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <libcamera/controls.h>
#include "ImgCtl.h"

class ControlWnd : public wxPanel
    {
public:
    ControlWnd (wxWindow *parent);
    virtual ~ControlWnd ();
    void LoadCtl (void);
    bool SetControl (enum CtrlID iCtrl, int iSet);
    // void UpdateControls (PiCam *picam);
    void ApplyControls (libcamera::ControlList &controls_);
    int ImgScale (void);
    void OnChoice (wxCommandEvent &e);
    void OnCheckBox (wxCommandEvent &e);
    void OnSlider (wxCommandEvent &e);
    void OnSpinCtrl (wxSpinEvent &e);

private:
    wxFlexGridSizer * m_fgsz;
    std::vector<ImgCtl>     m_victl;       // Image controls.

    DECLARE_EVENT_TABLE()
    };

#endif
