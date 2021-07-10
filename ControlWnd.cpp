// ControlWnd.cpp -  A window for containing camera controls.

#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <libcamera/controls.h>
#include <libcamera/control_ids.h>
#include "ControlWnd.h"

BEGIN_EVENT_TABLE(ControlWnd, wxPanel)
   EVT_CHOICE(wxID_ANY, ControlWnd::OnChoice)
   EVT_SLIDER(wxID_ANY, ControlWnd::OnSlider)
   EVT_CHECKBOX(wxID_ANY, ControlWnd::OnCheckBox)
   EVT_SPINCTRL(wxID_ANY, ControlWnd::OnSpinCtrl)
END_EVENT_TABLE()

/*** ControlWnd *******************************************************

     Constructor.

     Inputs:

     parent   =  Parent window.

     Coding history:

     WJB 19/06/21    Converted from MovieCap

*/

ControlWnd::ControlWnd (wxWindow *parent)
    :  wxPanel (parent)
    {
    // printf ("ControlWnd constructor.\n");

    // Create controls.
    LoadCtl ();
    m_fgsz   =  new wxFlexGridSizer (2);
    m_fgsz->AddGrowableCol (1);
    std::vector<ImgCtl>::iterator   it;
    int iCtrl;
    for (iCtrl = 0, it = m_victl.begin (); it != m_victl.end (); ++iCtrl, ++it)
        {
        const char *psDesc   =  it->GetDesc ().c_str ();
        // printf ("Create image control \"%s\"\n", psDesc);
        wxString sDesc =  wxString (psDesc, wxConvUTF8);
        m_fgsz->Add (new wxStaticText (this, wxID_ANY, sDesc), 0, wxALIGN_CENTRE_VERTICAL);
        int   iType =  it->GetType ();
        int   iMin, iMax, iDef, iVal;
        it->GetData (iMin, iMax, iDef, iVal);
        if ( iType == 1 )
            {
            wxBoxSizer *phbsz1   =  new   wxBoxSizer (wxHORIZONTAL);
            m_fgsz->Add (phbsz1, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL);
            phbsz1->Add (new wxSlider (this, 3*iCtrl, iVal, iMin, iMax, wxDefaultPosition,
                    wxSize (100,-1)), 1, wxEXPAND | wxALIGN_CENTRE_VERTICAL);
            wxSpinCtrl *pspin =  new   wxSpinCtrl (this, 3*iCtrl + 1, wxT(""), wxDefaultPosition,
                wxSize (120, -1), wxSP_VERTICAL);
            pspin->SetRange (iMin, iMax);
            pspin->SetValue (iVal);
            phbsz1->Add (pspin, 0, wxALIGN_CENTRE_VERTICAL);
            }
        else if ( iType == 2 )
            {
            wxCheckBox *   pchk  =  new wxCheckBox (this, 3*iCtrl, wxT(""));
            pchk->SetValue (it->Enabled ());
            m_fgsz->Add (pchk, 0, wxEXPAND);
            }
        else if ( iType == 3 )
            {
            wxChoice *  pchc  =  new wxChoice (this, 3*iCtrl);
            for ( int iItem = iMin; iItem <= iMax; ++iItem )
                {
                const char *psMenu   =  it->GetMenuItem (iItem).c_str ();
                // printf ("Menu item %d = \"%s\"\n", iItem, psMenu);
                pchc->Append (wxString (psMenu, wxConvUTF8));
                }
            pchc->SetSelection (iVal - iMin);
            m_fgsz->Add (pchc, 0, wxEXPAND);
            }
        else if ( iType == 4 )
            {
            wxBoxSizer *phbsz1   =  new   wxBoxSizer (wxHORIZONTAL);
            m_fgsz->Add (phbsz1, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL);
            wxCheckBox *   pchk  =  new wxCheckBox (this, 3*iCtrl, wxT(""));
            pchk->SetValue (it->Enabled ());
            phbsz1->Add (pchk, 0, wxALIGN_CENTRE_VERTICAL);
            phbsz1->Add (new wxSlider (this, 3*iCtrl + 1, iVal, iMin, iMax, wxDefaultPosition,
                    wxSize (100,-1)), 1, wxALIGN_CENTRE_VERTICAL);
            wxSpinCtrl *pspin =  new   wxSpinCtrl (this, 3*iCtrl + 2, wxT(""), wxDefaultPosition,
                wxSize (120, -1), wxSP_VERTICAL);
            pspin->SetRange (iMin, iMax);
            pspin->SetValue (iVal);
            phbsz1->Add (pspin, 0, wxALIGN_CENTRE_VERTICAL);
            }
        else if ( iType == 5 )
            {
            wxSpinCtrl *pspin =  new   wxSpinCtrl (this, 3*iCtrl, wxT(""), wxDefaultPosition,
                wxSize (120, -1), wxSP_VERTICAL);
            pspin->SetRange (iMin, iMax);
            pspin->SetValue (iVal);
            m_fgsz->Add (pspin, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL);
            }
        else if ( iType == 6 )
            {
            wxBoxSizer *phbsz1   =  new   wxBoxSizer (wxHORIZONTAL);
            m_fgsz->Add (phbsz1, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL);
            wxCheckBox *   pchk  =  new wxCheckBox (this, 3*iCtrl, wxT(""));
            pchk->SetValue (it->Enabled ());
            phbsz1->Add (pchk, 0, wxALIGN_CENTRE_VERTICAL);
            wxSpinCtrl *pspin =  new   wxSpinCtrl (this, 3*iCtrl + 2, wxT(""), wxDefaultPosition,
                wxSize (120, -1), wxSP_VERTICAL);
            pspin->SetRange (iMin, iMax);
            pspin->SetValue (iVal);
            phbsz1->Add (pspin, 1, wxALIGN_CENTRE_VERTICAL);
            }
        }
    m_fgsz->Add (new wxStaticText (this, wxID_ANY, ""), 0, wxALIGN_CENTRE_VERTICAL);
    m_fgsz->Add (new wxButton (this, ID_SNAP, "Snap"), 0, wxALIGN_CENTRE_VERTICAL);

    SetSizer (m_fgsz);
    m_fgsz->SetSizeHints (this);
    }


/*** ~ControlWnd ******************************************************

     Destructor.

     Coding history:

     WJB    19/ 6/21    Empty version.

*/

ControlWnd::~ControlWnd (void)
    {
    // printf ("ControlWnd destructor.\n");
    }

/*** LoadCtl *******************************************************

     Load all the controls for the image capture device.

     Coding history:

     WJB    19/ 6/21    Converted from MovieCap
*/

void ControlWnd::LoadCtl (void)
    {
    ImgCtl   ctl (0);

    m_victl.clear ();

    /* Brightness */
    ctl.m_iID      =  ctrlBright;       // Control ID.
    ctl.m_sName    =  "Brightness";     // Control name.
    ctl.m_iType    =  4;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  100;              // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  50;               // Default value.
    ctl.m_iValue   =  50;               // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

    /* Contrast */
    ctl.m_iID      =  ctrlCont;         // Control ID.
    ctl.m_sName    =  "Contrast";       // Control name.
    ctl.m_iType    =  4;                // Control type.
    ctl.m_iMin     =  -100;             // Minimum control value.
    ctl.m_iMax     =  100;              // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  0;                // Default value.
    ctl.m_iValue   =  0;                // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

    /* Saturation */
    ctl.m_iID      =  ctrlSat;          // Control ID.
    ctl.m_sName    =  "Saturation";     // Control name.
    ctl.m_iType    =  4;                // Control type.
    ctl.m_iMin     =  -100;             // Minimum control value.
    ctl.m_iMax     =  100;              // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  0;                // Default value.
    ctl.m_iValue   =  0;                // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

    /* Exposure Compensation */
    ctl.m_iID      =  ctrlExpComp;      // Control ID.
    ctl.m_sName    =  "Exposure Comp."; // Control name.
    ctl.m_iType    =  4;                // Control type.
    ctl.m_iMin     =  -10;              // Minimum control value.
    ctl.m_iMax     =  10;               // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  0;                // Default value.
    ctl.m_iValue   =  0;                // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

    /* White Balance */
    ctl.m_iID      =  ctrlWhiteBal;     // Control ID.
    ctl.m_sName    =  "White Balance";  // Control name.
    ctl.m_iType    =  3;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  8;                // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  1;                // Default value.
    ctl.m_iValue   =  1;                // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    ctl.m_vsMenu.push_back (std::string ("Off"));
    ctl.m_vsMenu.push_back (std::string ("Auto"));
    ctl.m_vsMenu.push_back (std::string ("Incandescent"));
    ctl.m_vsMenu.push_back (std::string ("Tungsten"));
    ctl.m_vsMenu.push_back (std::string ("Fluorescent"));
    ctl.m_vsMenu.push_back (std::string ("Indoor"));
    ctl.m_vsMenu.push_back (std::string ("Daylight"));
    ctl.m_vsMenu.push_back (std::string ("Cloudy"));
    ctl.m_vsMenu.push_back (std::string ("Custom"));
    m_victl.push_back (ctl);
    ctl.m_vsMenu.clear ();

    /* Exposure Mode */
    ctl.m_iID      =  ctrlExMode;       // Control ID.
    ctl.m_sName    =  "Exposure Mode";  // Control name.
    ctl.m_iType    =  3;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  4;                // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  1;                // Default value.
    ctl.m_iValue   =  1;                // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    ctl.m_vsMenu.push_back (std::string ("Off"));
    ctl.m_vsMenu.push_back (std::string ("Normal"));
    ctl.m_vsMenu.push_back (std::string ("Short"));
    ctl.m_vsMenu.push_back (std::string ("Long"));
    ctl.m_vsMenu.push_back (std::string ("Custom"));
    m_victl.push_back (ctl);
    ctl.m_vsMenu.clear ();

    /* Meter Mode */
    ctl.m_iID      =  ctrlMeterMode;    // Control ID.
    ctl.m_sName    =  "Meter Mode";     // Control name.
    ctl.m_iType    =  3;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  3;                // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  0;                // Default value.
    ctl.m_iValue   =  0;                // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    ctl.m_vsMenu.push_back (std::string ("Centre-weighted"));
    ctl.m_vsMenu.push_back (std::string ("Spot"));
    ctl.m_vsMenu.push_back (std::string ("Matrix"));
    ctl.m_vsMenu.push_back (std::string ("Custom"));
    m_victl.push_back (ctl);
    ctl.m_vsMenu.clear ();

    /* Exposure Time */
    ctl.m_iID      =  ctrlExp;          // Control ID.
    ctl.m_sName    =  "Exposure Time";  // Control name.
    ctl.m_iType    =  6;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  1000000;          // Maximum control value.
    ctl.m_iStep    =  1000;             // Control value step.
    ctl.m_iDefault =  10000;            // Default value.
    ctl.m_iValue   =  10000;            // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

    /* Analog Gain */
    ctl.m_iID      =  ctrlAlgGain;      // Control ID.
    ctl.m_sName    =  "Analog Gain";    // Control name.
    ctl.m_iType    =  4;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  400;              // Maximum control value.
    ctl.m_iStep    =  10;               // Control value step.
    ctl.m_iDefault =  100;              // Default value.
    ctl.m_iValue   =  100;              // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

#if HAVE_DIG_GAIN
    /* Digital Gain */
    ctl.m_iID      =  ctrlDigGain;      // Control ID.
    ctl.m_sName    =  "Digital Gain";   // Control name.
    ctl.m_iType    =  4;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  6400;             // Maximum control value.
    ctl.m_iStep    =  100;              // Control value step.
    ctl.m_iDefault =  100;              // Default value.
    ctl.m_iValue   =  100;              // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);
#endif

    /* Red Gain */
    ctl.m_iID      =  ctrlRedGain;      // Control ID.
    ctl.m_sName    =  "Red Gain";       // Control name.
    ctl.m_iType    =  4;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  800;              // Maximum control value.
    ctl.m_iStep    =  10;               // Control value step.
    ctl.m_iDefault =  100;              // Default value.
    ctl.m_iValue   =  100;              // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

    /* Blue Gain */
    ctl.m_iID      =  ctrlBlueGain;     // Control ID.
    ctl.m_sName    =  "Blue Gain";      // Control name.
    ctl.m_iType    =  4;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  800;              // Maximum control value.
    ctl.m_iStep    =  10;               // Control value step.
    ctl.m_iDefault =  100;              // Default value.
    ctl.m_iValue   =  100;              // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

    /* Dynamic Noise Reduction */
    ctl.m_iID      =  ctrlDenoise;      // Control ID.
    ctl.m_sName    =  "Denoise";        // Control name.
    ctl.m_iType    =  3;                // Control type.
    ctl.m_iMin     =  0;                // Minimum control value.
    ctl.m_iMax     =  3;                // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  0;                // Default value.
    ctl.m_iValue   =  0;                // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    ctl.m_vsMenu.push_back (std::string ("Off"));
    ctl.m_vsMenu.push_back (std::string ("Low"));
    ctl.m_vsMenu.push_back (std::string ("Medium"));
    ctl.m_vsMenu.push_back (std::string ("High"));
    m_victl.push_back (ctl);
    ctl.m_vsMenu.clear ();

    /* Image scale */
    ctl.m_iID      =  ctrlScale;        // Control ID.
    ctl.m_sName    =  "Image Scale";    // Control name.
    ctl.m_iType    =  1;                // Control type.
    ctl.m_iMin     =  1;                // Minimum control value.
    ctl.m_iMax     =  5;                // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  1;                // Default value.
    ctl.m_iValue   =  1;                // Current value.
    ctl.m_bEnable  =  true;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);

    /* Camera Run */
    ctl.m_iID      =  ctrlRun;          // Control ID.
    ctl.m_sName    =  "Camera Run";     // Control name.
    ctl.m_iType    =  2;                // Control type.
    ctl.m_iMin     =  1;                // Minimum control value.
    ctl.m_iMax     =  5;                // Maximum control value.
    ctl.m_iStep    =  1;                // Control value step.
    ctl.m_iDefault =  1;                // Default value.
    ctl.m_iValue   =  1;                // Current value.
    ctl.m_bEnable  =  false;
    ctl.m_bChanged =  false;
    m_victl.push_back (ctl);
    }

/*** ApplyControls *************************************************************************************

Apply control settings to camera

WJB 19/ 6/21    First draft

*/

void ControlWnd::ApplyControls (libcamera::ControlList &controls_)
    {
    controls_.clear ();
    
    /* Brightness */
    if ( m_victl[ctrlBright].m_bEnable )
	controls_.set(libcamera::controls::Brightness, m_victl[ctrlBright].m_iValue / 50.0 - 1.0);

    /* Contrast */
    if ( m_victl[ctrlCont].m_bEnable )
	controls_.set(libcamera::controls::Contrast, m_victl[ctrlCont].m_iValue / 100.0 + 1.0);

    /* Saturation */
    if ( m_victl[ctrlSat].m_bEnable )
	controls_.set(libcamera::controls::Saturation, m_victl[ctrlSat].m_iValue / 100.0 + 1.0);

    /* Exposure Compensation */
    if ( m_victl[ctrlExpComp].m_bEnable )
	controls_.set(libcamera::controls::ExposureValue, m_victl[ctrlExpComp].m_iValue / 4.0);

    /* White Balance */
    if ( m_victl[ctrlWhiteBal].m_bEnable )
	{
	controls_.set(libcamera::controls::AwbEnable, m_victl[ctrlWhiteBal].m_iValue > 0);
	controls_.set(libcamera::controls::AwbMode, m_victl[ctrlWhiteBal].m_iValue - 1);
	}

    /* Exposure Mode */
    if ( m_victl[ctrlExMode].m_bEnable )
	{
	controls_.set(libcamera::controls::AeEnable, m_victl[ctrlExMode].m_iValue > 0);
	controls_.set(libcamera::controls::AeExposureMode, m_victl[ctrlExMode].m_iValue - 1);
	}

    /* Meter Mode */
    if ( m_victl[ctrlMeterMode].m_bEnable )
	controls_.set(libcamera::controls::AeMeteringMode, m_victl[ctrlMeterMode].m_iValue);

    /* Exposure Time */
    if ( m_victl[ctrlExp].m_bEnable )
	controls_.set(libcamera::controls::ExposureTime, m_victl[ctrlExp].m_iValue);
    else controls_.set(libcamera::controls::ExposureTime, 0);

    /* Analog Gain */
    if ( m_victl[ctrlAlgGain].m_bEnable )
	controls_.set(libcamera::controls::AnalogueGain, m_victl[ctrlAlgGain].m_iValue / 100.0);

#if HAVE_DIG_GAIN
    /* Digital Gain */
    if ( m_victl[ctrlDigGain].m_bEnable )
	controls_.set(libcamera::controls::DigitalGain, m_victl[ctrlDigGain].m_iValue / 100.0);
#endif

    /* Red & Blue Gains */
    if ( ( m_victl[ctrlRedGain].m_bEnable ) && ( m_victl[ctrlBlueGain].m_bEnable ) )
	controls_.set(libcamera::controls::ColourGains,
	    {float(m_victl[ctrlRedGain].m_iValue / 100.0),
	     float(m_victl[ctrlBlueGain].m_iValue / 100.0)});
    // fval = picam->awb_gains_r; printf ("awb_gains_r = %10.3E\n", fval);

    /* Dynamic noise reduction */
    if ( m_victl[ctrlDenoise].m_bEnable )
	controls_.set(libcamera::controls::draft::NoiseReductionMode,
	    libcamera::controls::draft::NoiseReductionModeEnum(m_victl[ctrlDenoise].m_iValue));
    }

/*** OnChoice *********************************************************

Update a camera control.

Inputs:

   e  =  Choice event.

Coding history:

   WJB   28/ 5/10 First draft.
   WJB    4/ 9/11 Revised for separate controls.

*/

void ControlWnd::OnChoice (wxCommandEvent &e)
   {
   int   iCtrl =  e.GetId () / 3;
   wxChoice *  pchc  =  (wxChoice *) e.GetEventObject ();
   ImgCtl * pictl =  &m_victl[iCtrl];
   int   iMin, iMax, iDefault, iValue;
   pictl->GetData (iMin, iMax, iDefault, iValue);
   iValue   =  pchc->GetSelection () + iMin;
   pictl->Set (iValue);
   }

/*** OnCheckBox *********************************************************

Update a camera control.

Inputs:

   e  =  Choice event.

Coding history:

   WJB    4/ 9/11 Revised for separate controls.

*/

void ControlWnd::OnCheckBox (wxCommandEvent &e)
   {
   int   iCtrl =  e.GetId () / 3;
   wxCheckBox *pchk  =  (wxCheckBox *) e.GetEventObject ();
   bool bEnable = pchk->GetValue ();
   ImgCtl * pictl =  &m_victl[iCtrl];
   pictl->Enable (bEnable);
   if ( pictl->GetType () == 4 )
       {
       wxSlider *  psld  =  (wxSlider *)   pchk->GetNextSibling ();
       wxSpinCtrl *pspin =  (wxSpinCtrl *) psld->GetNextSibling ();
       psld->Enable (bEnable);
       pspin->Enable (bEnable);
       }
   else if ( pictl->GetType () == 6 )
       {
       wxSpinCtrl *pspin =  (wxSpinCtrl *) pchk->GetNextSibling ();
       pspin->Enable (bEnable);
       }
   if ( iCtrl == ctrlRedGain )
       {
       m_victl[ctrlBlueGain].m_bEnable = bEnable;
       ((wxCheckBox *)FindWindow (3 * ctrlBlueGain))->SetValue (bEnable);
       FindWindow (3 * ctrlBlueGain + 1)->Enable (bEnable);
       FindWindow (3 * ctrlBlueGain + 2)->Enable (bEnable);
       }
   else if ( iCtrl == ctrlBlueGain )
       {
       m_victl[ctrlRedGain].m_bEnable = bEnable;
       ((wxCheckBox *)FindWindow (3 * ctrlRedGain))->SetValue (bEnable);
       FindWindow (3 * ctrlRedGain + 1)->Enable (bEnable);
       FindWindow (3 * ctrlRedGain + 2)->Enable (bEnable);
       }
   }

/*** OnSlider *********************************************************

Update a camera control.

Inputs:

   e  =  Choice event.

Coding history:

   WJB    4/ 9/11 Revised for separate controls.
   WJB   11/ 9/11 Display both slider and spin control.

*/

void ControlWnd::OnSlider (wxCommandEvent &e)
   {
   int   iCtrl =  e.GetId () / 3;
   wxSlider *  psld  =  (wxSlider *) e.GetEventObject ();
   wxSpinCtrl *pspin =  (wxSpinCtrl *) psld->GetNextSibling ();
   ImgCtl * pictl =  &m_victl[iCtrl];
   int   iVal  =  psld->GetValue ();
   // printf ("OnSlider: this = %p, m_grabimg = %p, iCtrl = %d, pictl = %p, iVal = %d\n",
   //    this, m_grabimg, iCtrl, pictl, iVal);
   pspin->SetValue (iVal);
   pictl->Set (iVal);
   }

/*** OnSpinCtrl *********************************************************

Update a camera control.

Inputs:

   e  =  Choice event.

Coding history:

   WJB   11/ 9/11 Display both slider and spin control.

*/

void ControlWnd::OnSpinCtrl (wxSpinEvent &e)
   {
   int   iCtrl =  e.GetId () / 3;
   wxSpinCtrl *pspin =  (wxSpinCtrl *) e.GetEventObject ();
   ImgCtl * pictl =  &m_victl[iCtrl];
   int   iVal  =  pspin->GetValue ();
   // printf ("OnSpinCtrl: this = %p, m_grabimg = %p, iCtrl = %d, pictl = %p, iVal = %d\n",
   //    this, m_grabimg, iCtrl, pictl, iVal);
   pictl->Set (iVal);
   if ( pictl->GetType () < 5 )
       {
       wxSlider *  psld  =  (wxSlider *) pspin->GetPrevSibling ();
       psld->SetValue (iVal);
       }
   }
