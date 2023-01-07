#include "virtual_folder_picker_ctrl.h"
#include "VirtualDirectorySelectorDlg.h"
#include <event_notifier.h>
#include <workspace.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

const wxEventType wxEVT_VIRTUAL_FOLDER_PICKED = wxNewEventType();

VirtualFolderPickerCtrl::VirtualFolderPickerCtrl(wxWindow* parent, const wxString& path)
    : wxTextCtrl(parent, wxID_ANY, path, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_PROCESS_ENTER)
    , m_virtualFolder(path)
{
    this->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(VirtualFolderPickerCtrl::OnTextEnter), NULL, this);
    this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(VirtualFolderPickerCtrl::OnMouseLeft), NULL, this);
    SetEditable(true);
    wxTextCtrl::ChangeValue(path);
    SetEditable(false);
}

VirtualFolderPickerCtrl::~VirtualFolderPickerCtrl()
{
    this->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(VirtualFolderPickerCtrl::OnTextEnter), NULL, this);
    this->Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(VirtualFolderPickerCtrl::OnMouseLeft), NULL, this);
}

void VirtualFolderPickerCtrl::OnTextEnter(wxCommandEvent& e) { DoNotify(); }

void VirtualFolderPickerCtrl::OnMouseLeft(wxMouseEvent& event) { DoEdit(); }

void VirtualFolderPickerCtrl::DoEdit()
{
    wxString str = wxTextCtrl::GetValue();
    str.Trim().Trim(false);

    VirtualDirectorySelectorDlg selector(EventNotifier::Get()->TopFrame(), clCxxWorkspaceST::Get(), m_virtualFolder);
    if(selector.ShowModal() == wxID_OK) {
        m_virtualFolder = selector.GetVirtualDirectoryPath();
        SetEditable(true);
        wxTextCtrl::ChangeValue(m_virtualFolder);
        SetEditable(false);
        DoNotify();
    }
}

void VirtualFolderPickerCtrl::DoNotify()
{
    wxCommandEvent event(wxEVT_VIRTUAL_FOLDER_PICKED);
    event.SetString(GetValue());
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}
