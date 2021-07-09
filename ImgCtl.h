/* ImgCtl.h  -  A class representing a v4l2 image control */

#ifndef H_IMGCTL
#define H_IMGCTL

#define HAVE_DIG_GAIN  0

#include <vector>
#include <string>

enum CtrlID
    {
    ctrlBright,
    ctrlCont,
    ctrlSat,
    ctrlExpComp,
    ctrlWhiteBal,
    ctrlExMode,
    ctrlMeterMode,
    ctrlExp,
    ctrlAlgGain,
#if HAVE_DIG_GAIN
    ctrlDigGain,
#endif
    ctrlRedGain,
    ctrlBlueGain,
    ctrlDenoise,
    ctrlScale,
    ctrlRun
    };

class ImgCtl
   {
   public:
   ImgCtl (int iDev);
   ~ImgCtl ();
   bool Set (int iValue);
   std::string GetDesc (void) const { return m_sName; }
   int GetType (void) const { return m_iType; }
   std::string GetMenuItem (int iItem) const { return m_vsMenu[iItem]; }
   void GetData (int &iMin, int &iMax, int &iDefault, int &iValue) const;
   int GetMin (void) const { return m_iMin; }
   int GetMax (void) const { return m_iMax; }
   int GetDefault (void) const { return m_iDefault; }
   int GetValue (void) const { return m_iValue; }
   bool Enabled (void) const { return m_bEnable; }
   void Enable (bool bEnable) { m_bEnable = bEnable; }

   private:
   bool GetCtrlMenu (void);

   friend class ControlWnd;
   private:
   int                        m_iDev;        // Video device handle.
   enum CtrlID                m_iID;         // Control ID.
   std::string                m_sName;       // Control name.
   int                        m_iType;       // Control type.
   int                        m_iMin;        // Minimum control value.
   int                        m_iMax;        // Maximum control value.
   int                        m_iStep;       // Control value step.
   int                        m_iDefault;    // Default value.
   int                        m_iValue;      // Current value.
   bool                       m_bEnable;     // Enable control
   std::vector<std::string>   m_vsMenu;      // Menu entries.
   bool                       m_bChanged;
   };

#endif
