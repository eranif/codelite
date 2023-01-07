#include "file_picker_ctrl.h"
#include <wx/filedlg.h>
#include <wx/filename.h>

const wxEventType wxEVT_FILE_PICKED = wxNewEventType();

FilePickerCtrl::FilePickerCtrl(wxWindow* parent, const wxString& projectPath, const wxString& value)
    : wxTextCtrl(parent, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_PROCESS_ENTER)
    , m_bmpPath(value)
    , m_projectPath(projectPath)
{
    this->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(FilePickerCtrl::OnTextEnter), NULL, this);
    this->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(FilePickerCtrl::OnMouseLeft), NULL, this);
    wxTextCtrl::ChangeValue(value);
}

FilePickerCtrl::~FilePickerCtrl()
{
    this->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(FilePickerCtrl::OnTextEnter), NULL, this);
    this->Disconnect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(FilePickerCtrl::OnMouseLeft), NULL, this);
}

void FilePickerCtrl::OnTextEnter(wxCommandEvent& e) { DoNotify(); }

void FilePickerCtrl::OnMouseLeft(wxMouseEvent& event) { DoEdit(); }

void FilePickerCtrl::DoEdit()
{
    wxString str = wxTextCtrl::GetValue();
    str.Trim().Trim(false);

    wxFileName fn;
    if(m_bmpPath.IsEmpty()) {
        fn = wxFileName(m_projectPath, wxT(""));

    } else {
        fn = wxFileName(m_bmpPath);
        fn.MakeAbsolute(m_projectPath);
    }

    wxString wildcard = wxT("PNG files (*.png)|*.png|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif|All files (*)|*");
    wxString newPath = wxFileSelector(_("Select bitmap"), wxT(""), wxT(""), wxEmptyString, wildcard, wxFD_OPEN);
    if(newPath.IsEmpty() == false) {
        wxFileName newFilePath(newPath);
        if(m_projectPath.IsEmpty() == false) { newFilePath.MakeRelativeTo(m_projectPath); }
        m_bmpPath = newFilePath.GetFullPath(wxPATH_UNIX);
        wxTextCtrl::ChangeValue(m_bmpPath);
        DoNotify();
    }
}

void FilePickerCtrl::DoNotify()
{
    wxCommandEvent event(wxEVT_FILE_PICKED);
    event.SetString(GetValue());
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}
