// PictWnd.h  -  A window for displaying the video picture.

#ifndef  H_PICTWND
#define  H_PICTWND

// #include <wx/scrolwin.h> - Use the patched version to work around scrollbars bug
#include "scrolwin.h"
#include <wx/bitmap.h>
#include <wx/image.h>

class PictWnd : public wxScrolledWindow
   {
   public:
   PictWnd (wxWindow *parent);
   ~PictWnd ();
   void SetImage (wxImage *pimg);
   void OnSize (wxSizeEvent &e);
   virtual void OnDraw (wxDC &dc);

   private:
   wxImage *         m_image;
   wxBitmap *        m_bitmap;

    DECLARE_EVENT_TABLE()
   };

#endif
