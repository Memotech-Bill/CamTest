/*** ImgCtl.cpp -  A class representing a v4l2 image control */

#include "ImgCtl.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>

/*** ImgCtl ******************************************************************

     Control class constructor.

     Inputs:

     iDev  =  Device handle.

     Coding history:

     WJB   30/ 7/10 First draft.

*/

ImgCtl::ImgCtl (int iDev)
    {
    m_iDev      =  iDev;
    m_bChanged  =  false;
    }

/*** ~ImgCtl *****************************************************************

     Destructor.

     Coding history:

     WJB   30/ 7/10 First draft.

*/

ImgCtl::~ImgCtl (void)
    {
    }

/*** Set **********************************************************

     Sets the value of a control.

     Inputs:

     iValue   =  New value for control.

     Coding history:

     WJB   31/ 7/10 First draft.
     WJB   29/ 1/11 Version for dual threaded code. Simply remembers
     the required value, to be later set by a call to
     DoChange () from the video thread.

*/

bool ImgCtl::Set (int iValue)
    {
    if ( iValue != m_iValue )
        {
        m_iValue =  iValue;
        if ( m_iValue < m_iMin ) m_iValue =  m_iMin;
        else if ( m_iValue > m_iMax ) m_iValue = m_iMax;
        // printf ("Set %s (%p) = %d\n", m_sName.c_str (), &m_iValue, m_iValue);
        m_bChanged  =  true;
        }
    else
        {
        m_bChanged = false;
        }
    return m_bChanged;
    }

/*** GetData ***********************************************************

     Get control data.

     Outputs:

     iMin     =  Minimum value.
     iMax     =  Maximum value.
     iDefault =  Default value.
     iValue   =  Current value.

     Coding history:

     WJB   14/ 8/10 First draft.

*/

void ImgCtl::GetData (int &iMin, int &iMax, int &iDefault, int &iValue) const
    {
    iMin     =  m_iMin;
    iMax     =  m_iMax;
    iDefault =  m_iDefault;
    iValue   =  m_iValue;
    }
