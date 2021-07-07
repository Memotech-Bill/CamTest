// CamTest.h  -  Camera Test Program.

#ifndef  H_CAMTEST
#define  H_CAMTEST

#include <wx/wxprec.h>
#include <wx/app.h>

#include "CTFrame.h"

class CamTest : public wxApp
   {
   public:
   CamTest () {}
   virtual bool OnInit ();
   CTFrame *GetFrame (void) { return m_frame; }

   private:
   CTFrame *   m_frame;
   };

DECLARE_APP(CamTest);

#endif
