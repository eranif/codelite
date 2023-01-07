#include "CaptionBar.h"
#include "allocator_mgr.h"
#include "designer_container_panel.h"
#include "wxcTreeView.h"
#include "wxguicraft_main_view.h"
#include <event_notifier.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>

DesignerContainerPanel::DesignerContainerPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL | wxBORDER_RAISED)
    , m_mainPanel(NULL)
    , m_height(-1)
    , m_width(-1)
    , m_topLevelType(ID_WXPANEL_TOPLEVEL)
    , m_caption(NULL)
{
    Bind(wxEVT_SIZE, &DesignerContainerPanel::OnSize, this);
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
}

DesignerContainerPanel::~DesignerContainerPanel() { Unbind(wxEVT_SIZE, &DesignerContainerPanel::OnSize, this); }

void DesignerContainerPanel::EnableCaption(const wxString& caption, const wxString& style, const wxBitmap& icon)
{
    m_caption = new CaptionBar(this, caption, style, icon);
    GetSizer()->Insert(0, m_caption, 0, wxEXPAND | wxALL, 2);
    m_height += m_caption->GetSize().GetHeight();
}

void DesignerContainerPanel::SetMenuBar(MenuBar* mb)
{
    mb->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(DesignerContainerPanel::OnRightDown), NULL, this);

    // Make sure that the caption is on top of the menu bar
    GetSizer()->Insert(m_caption ? 1 : 0, mb, 0, wxEXPAND);
    m_height += mb->GetSize().GetHeight();
}

void DesignerContainerPanel::SetStatusBar(wxStatusBar* mb)
{
    mb->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(DesignerContainerPanel::OnLeftDown), NULL, this);
    mb->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(DesignerContainerPanel::OnRightDown), NULL, this);
    GetSizer()->Add(mb, 0, wxEXPAND);
    m_height += mb->GetSize().GetHeight();
}

void DesignerContainerPanel::SetToolbar(ToolBar* tb)
{
    tb->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(DesignerContainerPanel::OnRightDown), NULL, this);
    GetSizer()->Add(tb, 0, wxEXPAND);
    m_height += tb->GetSize().GetHeight();
}

void DesignerContainerPanel::AddMainView(wxPanel* panel)
{
    m_width = panel->GetSize().x;
    GetSizer()->Add(panel, 1, wxEXPAND);
    m_mainPanel = panel;
    m_height += m_mainPanel->GetSize().GetHeight();

    DoConnectCharEvent(this);
}

void DesignerContainerPanel::CalcBestSize(int winType)
{
    wxcWidget* page = GUICraftMainPanel::m_MainPanel->GetActiveWizardPage();
    wxcWidget* topWin = GUICraftMainPanel::m_MainPanel->GetActiveTopLevelWin();

    if(winType != ID_WXWIZARD || !page) {

        if(topWin)
            SetSizeHints(topWin->GetSize());
        else
            SetSizeHints(m_width, m_height);

        GetSizer()->Fit(this);

    } else {

        wxSize sz = page->GetSize();
        SetSizeHints(sz);
        GetSizer()->Fit(this);
    }
}

void DesignerContainerPanel::OnLeftDown(wxMouseEvent& e) { wxUnusedVar(e); }

void DesignerContainerPanel::OnRightDown(wxMouseEvent& e)
{
    e.Skip();
    wxCommandEvent contextEvent(wxEVT_SHOW_CONTEXT_MENU);
    EventNotifier::Get()->AddPendingEvent(contextEvent);
}

void DesignerContainerPanel::DoConnectCharEvent(wxWindow* win)
{
    if(win) {
        m_windows.insert(win);

        wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
        while(pclNode) {
            wxWindow* pclChild = pclNode->GetData();
            this->DoConnectCharEvent(pclChild);
            pclNode = pclNode->GetNext();
        }
    }
}

void DesignerContainerPanel::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}
