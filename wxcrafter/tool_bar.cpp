#include "tool_bar.h"
#include "allocator_mgr.h"
#include <event_notifier.h>

extern const wxEventType wxEVT_PREVIEW_BAR_SELECTED;

ToolBar::ToolBar(wxWindow* parent)
    : wxPanel(parent)
    , m_wxtb(NULL)

{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
}

ToolBar::~ToolBar() {}

void ToolBar::AddToolbar(wxToolBar* tb)
{
    GetSizer()->Add(tb, 0, wxEXPAND);
    Connect(wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ToolBar::OnClick), NULL, this);
    m_wxtb = tb;
}

void ToolBar::OnClick(wxCommandEvent& e)
{
    e.Skip();
    if(m_wxtb) {
        wxToolBarToolBase* tool = m_wxtb->FindById(e.GetId());
        if(tool) {
            wxString label = tool->GetLabel();
            wxString s;
            s << m_wxtb->GetName() << wxT(":") << label;
            wxCommandEvent evt(wxEVT_PREVIEW_BAR_SELECTED);
            evt.SetString(s);
            evt.SetInt(ID_WXTOOLBAR);
            EventNotifier::Get()->AddPendingEvent(evt);
        }
    }
}
