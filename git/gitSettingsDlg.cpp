#include "gitSettingsDlg.h"

#include "icons/icon_git.xpm"

GitSettingsDlg::GitSettingsDlg(wxWindow* parent, const wxColour& tracked,
                               const wxColour& diff, const wxString& pathGIT,
                               const wxString& pathGITK)
:wxDialog(parent, wxID_ANY, _("GIT plugin settings"),wxDefaultPosition, wxDefaultSize,
          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  SetIcon(wxICON(icon_git));
  
  wxStaticText* m_labelTrackedColour = new wxStaticText(this ,wxID_ANY,
                                                        _("Colour for tracked files"));
  wxStaticText* m_labelDiffColour = new wxStaticText(this ,wxID_ANY,
                                                        _("Colour for files with diffs"));
  wxStaticText* m_labelGITPath = new wxStaticText(this ,wxID_ANY,
                                                        _("Path to gitk executable"));
  wxStaticText* m_labelGITKPath = new wxStaticText(this ,wxID_ANY,
                                                        _("Path to gitk executable"));
  
  m_colourTrackedFile = new wxColourPickerCtrl(this, wxID_ANY, tracked);
  m_colourDiffFile = new wxColourPickerCtrl(this, wxID_ANY, diff);
  m_pathGIT = new wxFilePickerCtrl(this,wxID_ANY,pathGIT,
                                    wxFileSelectorPromptStr,
                                    wxFileSelectorDefaultWildcardStr,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
   m_pathGITK = new wxFilePickerCtrl(this,wxID_ANY,pathGITK,
                                    wxFileSelectorPromptStr,
                                    wxFileSelectorDefaultWildcardStr,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
  wxGridSizer* sizer = new wxGridSizer(0,2, 4);
  sizer->Add(m_labelTrackedColour,0,wxEXPAND,2);
  sizer->Add(m_colourTrackedFile,0,wxEXPAND,2);
  sizer->Add(m_labelDiffColour,0,wxEXPAND,2);
  sizer->Add(m_colourDiffFile,0,wxEXPAND,2);
  sizer->Add(m_labelGITPath,0,wxEXPAND,2);
  sizer->Add(m_pathGIT,0,wxEXPAND,2);
  sizer->Add(m_labelGITKPath,0,wxEXPAND,2);
  sizer->Add(m_pathGITK,0,wxEXPAND,2);
  sizer->AddSpacer(10);
  sizer->Add(CreateButtonSizer( wxOK|wxCANCEL),0,wxEXPAND,2);
  SetSizer(sizer);
}

const wxColour GitSettingsDlg::GetTrackedFileColour()
{
  return m_colourTrackedFile->GetColour();
}
const wxColour GitSettingsDlg::GetDiffFileColour()
{
  return m_colourDiffFile->GetColour();
}
const wxString GitSettingsDlg::GetGITExecutablePath()
{
  return m_pathGIT->GetPath();
}
const wxString GitSettingsDlg::GetGITKExecutablePath()
{
  return m_pathGITK->GetPath();
}

