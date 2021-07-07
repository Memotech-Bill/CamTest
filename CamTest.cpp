// CamTest.cpp   -  8mm Movie Capture program.

#include "CamTest.h"
#include "CTFrame.h"

IMPLEMENT_APP(CamTest)

/*** OnInit *************************************************************

Called on startup of the application. Initialise the main window.

Coding history:

     WJB    14/ 6/21    First draft, based upon MovieCap code.

*/

bool CamTest::OnInit (void)
   {
   m_frame  =  new CTFrame ();
   m_frame->Show ();

   return   true;
   }
