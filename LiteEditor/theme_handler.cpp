#include "theme_handler.h"
#include "editor_config.h"
#include "frame.h"
#include "workspace_pane.h"
#include "event_notifier.h"
#include "drawingutils.h"
#include <wx/listbox.h>

const wxEventType wxEVT_CL_THEME_CHANGED = ::wxNewEventType();

ThemeHandler::ThemeHandler()
{
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ThemeHandler::OnEditorThemeChanged), NULL, this);
}

ThemeHandler::~ThemeHandler()
{
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ThemeHandler::OnEditorThemeChanged), NULL, this);
}

void ThemeHandler::OnEditorThemeChanged(wxCommandEvent& e)
{
    e.Skip();
#if defined(__WXMSW__) || defined(__WXGTK__)
    wxColour bgColour = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    wxColour fgColour = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    
    if ( !bgColour.IsOk() || !fgColour.IsOk() ) {
        return;
    }
    
    bool newColourIsDark = DrawingUtils::IsDark( bgColour );
    size_t pageCount = clMainFrame::Get()->GetWorkspacePane()->GetNotebook()->GetPageCount();
    for(size_t i=0; i<pageCount; ++i) {
        
        wxWindow * page = clMainFrame::Get()->GetWorkspacePane()->GetNotebook()->GetPage(i);
        if ( page ) {
            DoUpdateColours(page, bgColour, fgColour);
        }
    }
#endif
}

void ThemeHandler::DoUpdateColours(wxWindow* win, const wxColour& bg, const wxColour& fg)
{
    if ( dynamic_cast<wxTreeCtrl*>(win) || dynamic_cast<wxListBox*>(win) ) {
        win->SetBackgroundColour( bg );
        win->SetForegroundColour( fg );
    }
    
    wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    while(pclNode) {
        wxWindow* pclChild = pclNode->GetData();
        this->DoUpdateColours(pclChild, bg, fg);
        pclNode = pclNode->GetNext();
    }
}
