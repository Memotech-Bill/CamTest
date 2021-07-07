// PictWnd.cpp  -  A window for displaying the video picture.

#include <stdio.h>
#include <wx/dc.h>
#include "PictWnd.h"

BEGIN_EVENT_TABLE(PictWnd, wxPanel)
EVT_SIZE(PictWnd::OnSize)
END_EVENT_TABLE()

/*** PictWnd ***************************************************

     Constructor.

     Inputs:

     parent   =  Parent window.

     Coding history:

     WJB   20/ 5/10 First draft.
     WJB   26/ 3/11 PictWnd now takes ownership of the image.

*/

PictWnd::PictWnd (wxWindow *parent)
    :  wxScrolledWindow (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxHSCROLL | wxVSCROLL | wxALWAYS_SHOW_SB)
    {
    // printf ("PictWnd constructor.\n");
    m_image     =  NULL;
    m_bitmap    =  NULL;
    }

/*** ~PictWnd **************************************************

     Destructor. Free resources.

     Coding history:

     WJB   25/ 5/10 First draft.
     WJB   26/ 3/11 PictWnd now takes ownership of the image.
     WJB   17/ 7/11 Close log files.

*/

PictWnd::~PictWnd (void)
    {
    // printf ("PictWnd destructor.\n");
    if ( m_image != NULL )  delete   m_image;
    if ( m_bitmap != NULL ) delete   m_bitmap;
    }

/*** SetImage **************************************************

     Sets an image to be displayed in the window.

     Inputs:

     img   =  Image to display.

     Coding history:

     WJB   24/ 5/10 First draft.
     WJB   26/ 3/11 PictWnd now takes ownership of the image.

*/

void PictWnd::SetImage (wxImage *pimg)
    {
    // Copy image.
    if ( m_image != NULL )  delete   m_image;
    m_image  =  pimg;
    if ( m_bitmap != NULL ) delete   m_bitmap;
    m_bitmap = new wxBitmap (*pimg);
    SetVirtualSize (m_bitmap->GetWidth (), m_bitmap->GetHeight ());
    // printf ("Loaded bitmap: %d x %d\n", m_bitmap->GetWidth (), m_bitmap->GetHeight ());

    // Redraw screen.
    Refresh (false);
    // printf ("Called Refresh.\n");
    }

/*** OnDraw *****************************************************

     Display the current image (if any) in the window.

     Inputs:

     dc =  Device context to draw to.

     Coding history:

     WJB   24/ 5/10 First draft.

*/

void PictWnd::OnDraw (wxDC &dc)
    {
    // printf ("Entered OnDraw.\n");
    if ( m_bitmap != NULL )
        {
        // printf ("Draw bitmap.\n");
        dc.DrawBitmap (*m_bitmap, 0, 0, false);
        }
    }

/*** OnSize *************************************************************************************

 */

void PictWnd::OnSize (wxSizeEvent &e)
    {
    wxSize sz = e.GetSize ();
    // printf ("PictWnd size: %d x %d\n", sz.GetWidth (), sz.GetHeight ());
    
    int   iWidth, iHeight;
    GetClientSize (&iWidth, &iHeight);
    iWidth   /= 10;
    iHeight  /= 10;
    SetScrollRate (iWidth, iHeight);
    }
