#include "multi_string_ctrl.h"
#include "enter_strings_dlg.h"
#include "wxgui_helpers.h"

const wxEventType wxEVT_MUTLI_STRING_UPDATE = wxNewEventType();

MultiStringCtrl::MultiStringCtrl(wxWindow* parent, const wxString& value, const wxString& delim, const wxString& msg)
    : wxTextCtrl(parent, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_PROCESS_ENTER)
    , m_delim(delim)
    , m_msg(msg)
{
    this->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(MultiStringCtrl::OnTextEnter), NULL, this);
    this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MultiStringCtrl::OnMouseLeft), NULL, this);
    wxTextCtrl::ChangeValue(value);
}

MultiStringCtrl::~MultiStringCtrl()
{
    this->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(MultiStringCtrl::OnTextEnter), NULL, this);
    this->Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MultiStringCtrl::OnMouseLeft), NULL, this);
}

void MultiStringCtrl::OnTextEnter(wxCommandEvent& e) { DoNotify(); }

void MultiStringCtrl::OnMouseLeft(wxMouseEvent& event) { DoEdit(); }

void MultiStringCtrl::DoEdit()
{
    wxString str = wxTextCtrl::GetValue();
    str.Trim().Trim(false);

    wxArrayString arr = wxCrafter::SplitByString(str, m_delim, true);
    str.Clear();

    for(size_t i = 0; i < arr.GetCount(); i++) {
        str << arr.Item(i) << wxT("\n");
    }

    if(str.IsEmpty() == false) { str.RemoveLast(); }

    EnterStringsDlg dlg(this, str);
    dlg.SetMessage(m_msg);
    if(dlg.ShowModal() == wxID_OK) {
        wxString value = dlg.GetValue();

        wxArrayString strings = wxCrafter::Split(value, wxT("\n\r"), wxTOKEN_RET_EMPTY_ALL);
        value = wxCrafter::Join(strings, m_delim);
        wxTextCtrl::ChangeValue(value); // We call 'SetValue' since want an event to fire

        DoNotify();
    }
}
void MultiStringCtrl::DoNotify()
{
    wxCommandEvent event(wxEVT_MUTLI_STRING_UPDATE);
    event.SetString(GetValue());
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}
