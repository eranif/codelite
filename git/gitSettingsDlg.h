//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitSettingsDlg__
#define __gitSettingsDlg__

#include <wx/wx.h>
#include <wx/clrpicker.h>
#include <wx/filepicker.h>

class GitSettingsDlg : public wxDialog
{
  wxColourPickerCtrl* m_colourTrackedFile;
  wxColourPickerCtrl* m_colourDiffFile;
  wxFilePickerCtrl* m_pathGIT;
  wxFilePickerCtrl* m_pathGITK;
  public:
    GitSettingsDlg(wxWindow* parent,const wxColour& tracked, const wxColour& diff,
                   const wxString& pathGIT, const wxString& pathGITK);
    const wxColour GetTrackedFileColour();
    const wxColour GetDiffFileColour();
    const wxString GetGITExecutablePath();
    const wxString GetGITKExecutablePath();
};

#endif //__gitSettingsDlg__

