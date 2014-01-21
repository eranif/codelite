#include "theme_handler_helper.h"
#include "event_notifier.h"
#include "plugin.h"
#include "editor_config.h"
#include <wx/treectrl.h>
#include <wx/listbox.h>
#include <wx/dataview.h>
#include <wx/textctrl.h>
#include <wx/html/htmlwin.h>

class wxDataViewCtrl;
class wxTextCtrl;
class wxListBox;

ThemeHandlerHelper::ThemeHandlerHelper(wxWindow* win)
    : m_window(win)
{
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ThemeHandlerHelper::OnThemeChanged), NULL, this);
}

ThemeHandlerHelper::~ThemeHandlerHelper()
{
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ThemeHandlerHelper::OnThemeChanged), NULL, this);
}

void ThemeHandlerHelper::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    wxColour bgColour = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    wxColour fgColour = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    
    if ( !bgColour.IsOk() || !fgColour.IsOk() ) {
        return;
    }
    
    DoUpdateColours(m_window, bgColour, fgColour);
}

void ThemeHandlerHelper::DoUpdateColours(wxWindow* win, const wxColour& bg, const wxColour& fg)
{
    // wxTextCtrl needs some extra special handling
    if ( dynamic_cast<wxTextCtrl*>(win) ) {
        wxTextCtrl* txtCtrl = dynamic_cast<wxTextCtrl*>(win);
        wxTextAttr attr = txtCtrl->GetDefaultStyle();
        attr.SetBackgroundColour( bg );
        attr.SetTextColour( fg );
        txtCtrl->SetDefaultStyle( attr );
    }
    
    // Generic handling
    if ( dynamic_cast<wxTreeCtrl*>(win)     || 
         dynamic_cast<wxListBox*>(win)      || 
         dynamic_cast<wxDataViewCtrl*>(win) || 
         dynamic_cast<wxTextCtrl*>(win)     /*||
         dynamic_cast<wxHtmlWindow*>(win) */
        )
    {
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
