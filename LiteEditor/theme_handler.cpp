#include "theme_handler.h"
#include "editor_config.h"
#include "frame.h"
#include "workspace_pane.h"
#include "event_notifier.h"
#include "drawingutils.h"
#include <wx/listbox.h>
#include "wxcl_log_text_ctrl.h"

#define CHECK_POINTER(p) if ( !p ) return;

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
    wxColour bgColour = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    wxColour fgColour = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    
    if ( !bgColour.IsOk() || !fgColour.IsOk() ) {
        return;
    }
    
    size_t pageCount = clMainFrame::Get()->GetWorkspacePane()->GetNotebook()->GetPageCount();
    for(size_t i=0; i<pageCount; ++i) {
        
        wxWindow * page = clMainFrame::Get()->GetWorkspacePane()->GetNotebook()->GetPage(i);
        if ( page ) {
            DoUpdateColours(page, bgColour, fgColour);
        }
    }
    
    pageCount = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPageCount();
    for(size_t i=0; i<pageCount; ++i) {
        
        wxWindow * page = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(i);
        if ( page ) {
            DoUpdateColours(page, bgColour, fgColour);
        }
    }
    
    wxclTextCtrl *log = dynamic_cast<wxclTextCtrl*>( wxLog::GetActiveTarget() );
    if ( log ) {
        log->Reset();
    }
}

void ThemeHandler::DoUpdateColours(wxWindow* win, const wxColour& bg, const wxColour& fg)
{
    if ( dynamic_cast<wxTreeCtrl*>(win) || dynamic_cast<wxListBox*>(win) || dynamic_cast<wxDataViewCtrl*>(win) || dynamic_cast<wxTextCtrl*>(win)) {
        win->SetBackgroundColour( bg );
        win->SetForegroundColour( fg );
        win->Refresh();
    }

    wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    while(pclNode) {
        wxWindow* pclChild = pclNode->GetData();
        this->DoUpdateColours(pclChild, bg, fg);
        pclNode = pclNode->GetNext();
    }
}

void ThemeHandler::DoUpdateSTCBgColour(wxStyledTextCtrl* stc)
{
    CHECK_POINTER(stc);
    for (int i=0; i<=wxSTC_STYLE_DEFAULT; ++i) {
        stc->StyleSetBackground(i, DrawingUtils::GetOutputPaneBgColour());
    }
    stc->Refresh();
}
