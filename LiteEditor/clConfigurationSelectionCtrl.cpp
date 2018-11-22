#include "bitmap_loader.h"
#include "clConfigurationSelectionCtrl.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "wxStringHash.h"
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/menu.h>
#include <wx/renderer.h>

#ifdef __WXMSW__
#define MARGIN_SPACER 5
#elif defined(__WXGTK3__)
#define MARGIN_SPACER 5
#else
#define MARGIN_SPACER 3
#endif

clConfigurationSelectionCtrl::clConfigurationSelectionCtrl(wxWindow* parent, wxWindowID winid, const wxPoint& pos,
                                                           const wxSize& size, long style)
    : wxPanel(parent, winid, pos, size, style)
    , m_state(eButtonState::kNormal)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    MSWSetNativeTheme(this);
    Bind(wxEVT_PAINT, &clConfigurationSelectionCtrl::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clConfigurationSelectionCtrl::OnEraseBG, this);
    Bind(wxEVT_LEFT_DOWN, &clConfigurationSelectionCtrl::OnLeftDown, this);
    Bind(wxEVT_SIZE, [&](wxSizeEvent& event) {
        event.Skip();
        Refresh();
    });
    SetSizeHints(DrawingUtils::GetBestSize("ABCDEFGHIJKLp", MARGIN_SPACER, MARGIN_SPACER));
}

clConfigurationSelectionCtrl::~clConfigurationSelectionCtrl()
{
    Unbind(wxEVT_PAINT, &clConfigurationSelectionCtrl::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clConfigurationSelectionCtrl::OnEraseBG, this);
    Unbind(wxEVT_LEFT_DOWN, &clConfigurationSelectionCtrl::OnLeftDown, this);
}

void clConfigurationSelectionCtrl::OnPaint(wxPaintEvent& e)
{
    wxAutoBufferedPaintDC bdc(this);
    PrepareDC(bdc);
    wxGCDC gcdc(bdc);

    wxRect rect = GetClientRect();
    gcdc.SetPen(DrawingUtils::GetPanelBgColour());
    gcdc.SetBrush(DrawingUtils::GetPanelBgColour());
    gcdc.DrawRectangle(rect);
    
#ifdef __WXGTK__
    rect.Deflate(1);
#endif

    // Build the text to draw
    wxString label;
    label << m_activeProject << " :: " << m_activeConfiguration;
    DrawingUtils::DrawNativeChoice(this, gcdc, rect, label);
}

void clConfigurationSelectionCtrl::OnEraseBG(wxEraseEvent& e) {}

void clConfigurationSelectionCtrl::Update(const wxArrayString& projects, const wxArrayString& configurations)
{
    m_projects = projects;
    m_configurations = configurations;
}

void clConfigurationSelectionCtrl::Clear() {}

void clConfigurationSelectionCtrl::OnLeftDown(wxMouseEvent& e)
{
    wxUnusedVar(e);
    wxPoint pt = GetClientRect().GetBottomLeft();
#ifdef __WXOSX__
    pt.y += 5;
#endif
    wxMenu menu;
    std::unordered_map<int, wxString> M;
    for(size_t i = 0; i < m_configurations.GetCount(); ++i) {
        wxMenuItem* item = menu.Append(wxID_ANY, m_configurations.Item(i), "", wxITEM_CHECK);
        if(m_configurations.Item(i) == m_activeConfiguration) { item->Check(); }
        M[item->GetId()] = m_configurations.Item(i);
    }
    menu.AppendSeparator();
    wxMenuItem* openConfig = new wxMenuItem(NULL, wxID_ANY, OPEN_CONFIG_MGR_STR, "", wxITEM_CHECK);
    // openConfig->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("cog"));
    openConfig = menu.Append(openConfig);
    M[openConfig->GetId()] = OPEN_CONFIG_MGR_STR;

    int selection = GetPopupMenuSelectionFromUser(menu, pt);
    if(selection == wxID_NONE) { return; }

    wxString newConfig = M[selection];
    clCommandEvent changeEvent(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED);
    changeEvent.SetString(newConfig);
    EventNotifier::Get()->AddPendingEvent(changeEvent);

    if(newConfig != OPEN_CONFIG_MGR_STR) { m_activeConfiguration = newConfig; }
    m_state = eButtonState::kNormal;
    Refresh();
}

void clConfigurationSelectionCtrl::SetActiveConfiguration(const wxString& activeConfiguration)
{
    this->m_activeConfiguration = activeConfiguration;
    Refresh();
}

void clConfigurationSelectionCtrl::SetActiveProject(const wxString& activeProject)
{
    this->m_activeProject = activeProject;
    Refresh();
}
