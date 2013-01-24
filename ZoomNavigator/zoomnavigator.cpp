//	Copyright: 2013 Brandon Captain
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/stc/stc.h>
#include "dockablepane.h"
#include "detachedpanesinfo.h"
#include "zoomnavigator.h"
#include "event_notifier.h"
#include "znSettingsDlg.h"
#include "zn_config_item.h"
#include <wx/msgdlg.h>
#include <wx/wupdlock.h>

static ZoomNavigator* thePlugin = NULL;
#define CHECK_CONDITION( cond ) if ( !cond ) return;

const char* ZOOM_PANE_TITLE = "Zoom Navigator";

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new ZoomNavigator(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Brandon Captain"));
    info.SetName(wxT("ZoomNavigator"));
    info.SetDescription(wxT("A dockable pane that shows a zoomed-out view of your code."));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

ZoomNavigator::ZoomNavigator(IManager *manager)
    : IPlugin(manager)
    , mgr( manager )
    , zoompane( NULL )
    , m_topWindow(NULL)
    , m_text( NULL )
    , m_editor( NULL )
    , m_markerFirstLine(wxNOT_FOUND)
    , m_markerLastLine(wxNOT_FOUND)
    , m_enabled(true)
    , m_lastLine(wxNOT_FOUND)
    , m_startupCompleted(false)
{
    m_config = new clConfig("zoom-navigator.conf");
    m_longName = wxT("Zoom Navigator");
    m_shortName = wxT("ZoomNavigator");
    m_topWindow = m_mgr->GetTheApp();
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(ZoomNavigator::OnEditorChanged),    NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_PAGE_CHANGED,      wxCommandEventHandler(ZoomNavigator::OnEditorChanged),    NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED,   wxCommandEventHandler(ZoomNavigator::OnAllEditorsClosing), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ZN_SETTINGS_UPDATED,   wxCommandEventHandler(ZoomNavigator::OnSettingsChanged),  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED,            wxCommandEventHandler(ZoomNavigator::OnFileSaved),        NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED,      wxCommandEventHandler(ZoomNavigator::OnWorkspaceClosed),  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE,             wxCommandEventHandler(ZoomNavigator::OnInitDone), NULL, this);
    m_topWindow->Connect(XRCID("zn_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ZoomNavigator::OnSettings), NULL, this);
    DoInitialize();
}

ZoomNavigator::~ZoomNavigator()
{
}

clToolBar *ZoomNavigator::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void ZoomNavigator::CreatePluginMenu(wxMenu *pluginsMenu)
{
    wxMenu *menu = new wxMenu();
    wxMenuItem *item(NULL);
    item = new wxMenuItem(menu, XRCID("zn_settings"), _("Settings"), _("Settings"), wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Zoom Navigator"), menu);
}

void ZoomNavigator::OnShowHideClick( wxCommandEvent& e )
{ }

void ZoomNavigator::DoInitialize()
{
    znConfigItem data;
    if ( m_config->ReadItem( &data ) ) {
        m_enabled = data.IsEnabled();
    }
    
    // create tab (possibly detached)
    Notebook *book = m_mgr->GetWorkspacePaneNotebook();
    if( IsZoomPaneDetached() ) {
        // Make the window child of the main panel (which is the grand parent of the notebook)
        DockablePane *cp = new DockablePane(book->GetParent()->GetParent(), book, ZOOM_PANE_TITLE, wxNullBitmap, wxSize(200, 200));
        zoompane = new wxPanel( cp );
        cp->SetChildNoReparent(zoompane);

    } else {
        zoompane = new wxPanel( book );
        book->AddPage( zoompane, ZOOM_PANE_TITLE, false);
    }

    m_text = new ZoomText( zoompane );
    m_text->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(ZoomNavigator::OnPreviewClicked), NULL, this);
    m_text->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(ZoomNavigator::OnPreviewClicked), NULL, this);
    m_text->SetCursor(wxCURSOR_POINT_LEFT);

    wxBoxSizer* bs = new wxBoxSizer( wxVERTICAL );
    bs->Add( m_text, 1, wxEXPAND, 0 );
    wxCheckBox* cbEnablePlugin = new wxCheckBox(zoompane, wxID_ANY, _("Enable plugin"));
    cbEnablePlugin->SetValue( data.IsEnabled() );
    bs->Add( cbEnablePlugin, 0, wxEXPAND);
    
    cbEnablePlugin->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ZoomNavigator::OnEnablePlugin), NULL, this);
    zoompane->SetSizer( bs );
}

bool ZoomNavigator::IsZoomPaneDetached()
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    wxArrayString detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(ZOOM_PANE_TITLE) != wxNOT_FOUND;
}

void ZoomNavigator::DoUpdate()
{
    // sanity tests
    CHECK_CONDITION( m_enabled );
    CHECK_CONDITION( !m_mgr->IsShutdownInProgress() );
    CHECK_CONDITION( m_editor );
    CHECK_CONDITION( (m_editor == m_mgr->GetActiveEditor()) );

    wxStyledTextCtrl* stc = m_editor->GetSTC();
    CHECK_CONDITION( stc );
    
    int first = stc->GetFirstVisibleLine();
    int last = stc->LinesOnScreen()+first;

    if ( m_markerFirstLine != first || m_markerLastLine != last ) {
        PatchUpHighlights( first, last );
        SetZoomTextScrollPosToMiddle( stc );
    }
}

void ZoomNavigator::SetEditorText( IEditor* editor )
{
    m_text->UpdateText( editor );
}

void ZoomNavigator::SetZoomTextScrollPosToMiddle( wxStyledTextCtrl* stc )
{
    int first;
    first = stc->GetFirstVisibleLine();

    // we want to make 'first' centered
    int numLinesOnScreen = m_text->LinesOnScreen();
    int linesAboveIt = numLinesOnScreen / 2;

    first = first - linesAboveIt;
    if ( first < 0 )
        first = 0;

    m_text->SetFirstVisibleLine( first );
    m_text->ClearSelections();
}

void ZoomNavigator::PatchUpHighlights( const int first, const int last )
{
    int x;
    m_text->MarkerDeleteAll(1);

    // set new highlights
    for ( x = first; x <= last; x++ ) {
        m_text->MarkerAdd( x, 1 );
    }
    m_markerFirstLine = first;
    m_markerLastLine  = last;
}

void ZoomNavigator::HookPopupMenu(wxMenu *menu, MenuType type)
{ }

void ZoomNavigator::UnHookPopupMenu(wxMenu *menu, MenuType type)
{ }

void ZoomNavigator::UnPlug()
{
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(ZoomNavigator::OnEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_PAGE_CHANGED,      wxCommandEventHandler(ZoomNavigator::OnEditorChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSED,   wxCommandEventHandler(ZoomNavigator::OnAllEditorsClosing), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ZN_SETTINGS_UPDATED,   wxCommandEventHandler(ZoomNavigator::OnSettingsChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED,            wxCommandEventHandler(ZoomNavigator::OnFileSaved), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED,      wxCommandEventHandler(ZoomNavigator::OnWorkspaceClosed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE,             wxCommandEventHandler(ZoomNavigator::OnInitDone), NULL, this);
    
    m_topWindow->Disconnect(XRCID("zn_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ZoomNavigator::OnSettings), NULL, this);
    
    // Remove the tab if it's actually docked in the workspace pane
    size_t index(Notebook::npos);
    index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(zoompane);
    if (index != Notebook::npos) {
        m_mgr->GetWorkspacePaneNotebook()->RemovePage(index, false);
    }
    zoompane->Destroy();
}

void ZoomNavigator::OnTimer(wxTimerEvent& e)
{
    DoUpdate();
}

void ZoomNavigator::OnEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    CHECK_CONDITION(m_startupCompleted);
    
    DoCleanup();
    
    if ( m_enabled ) {
        DoSetEditor( reinterpret_cast<IEditor*>( e.GetClientData() ) );
        if ( m_editor ) {
            m_text->UpdateLexer( m_editor->GetFileName().GetFullName() );
        }
        SetEditorText( m_editor );

    } else {
        DoSetEditor(NULL);
        SetEditorText( NULL );
    }
    DoUpdate();
}

void ZoomNavigator::OnEditorClosing(wxCommandEvent& e)
{
    e.Skip();
    CHECK_CONDITION(m_startupCompleted);
    if ( e.GetClientData() == m_editor && m_editor ) {
        DoCleanup();
    }
}

void ZoomNavigator::OnAllEditorsClosing(wxCommandEvent& e)
{
    e.Skip();
    CHECK_CONDITION(m_startupCompleted);
    DoCleanup();
}

void ZoomNavigator::OnPreviewClicked(wxMouseEvent& e)
{
    // user clicked on the preview
    CHECK_CONDITION(m_startupCompleted);
    CHECK_CONDITION(m_editor);
    CHECK_CONDITION(m_enabled);
    
    // the first line is taken from the preview
    int pos = m_text->PositionFromPoint(e.GetPosition());
    if ( pos == wxSTC_INVALID_POSITION ) {
        return;
    }
    int first = m_text->LineFromPosition( pos );
    int nLinesOnScreen = m_editor->GetSTC()->LinesOnScreen();
    first -= (nLinesOnScreen/2);
    if ( first < 0 ) first = 0;
    
    // however, the last line is set according to the actual editor
    int last  = nLinesOnScreen + first;

    PatchUpHighlights( first, last );
    m_editor->GetSTC()->SetFirstVisibleLine( first );
    m_editor->SetCaretAt( m_editor->PosFromLine( first + (nLinesOnScreen / 2) ) ) ;

    // reset the from/last members to avoid unwanted movements in the 'OnTimer' function
    m_markerFirstLine = m_editor->GetSTC()->GetFirstVisibleLine();
    m_markerLastLine  = m_markerFirstLine + m_editor->GetSTC()->LinesOnScreen();
}

void ZoomNavigator::DoCleanup()
{
    SetEditorText(NULL);
    DoSetEditor(NULL);
    m_markerFirstLine = wxNOT_FOUND;
    m_markerLastLine  = wxNOT_FOUND;
    m_text->UpdateLexer("");
}

void ZoomNavigator::OnSettings(wxCommandEvent& e)
{
    znSettingsDlg dlg(wxTheApp->GetTopWindow());
    dlg.ShowModal();
}

void ZoomNavigator::OnSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    m_config->Reload();
    znConfigItem data;
    if ( m_config->ReadItem( &data ) ) {
        m_enabled = data.IsEnabled();
        
        if ( !m_enabled ) {
            // Clear selection
            m_text->UpdateText(NULL);
            
        } else {
            DoSetEditor(m_mgr->GetActiveEditor());
            if ( m_editor ) {
                m_text->UpdateLexer(m_editor->GetFileName().GetFullName());
            }
            m_text->UpdateText(m_editor);
            m_markerFirstLine = wxNOT_FOUND;
            m_markerLastLine  = wxNOT_FOUND;
            
        }
    }
}

void ZoomNavigator::OnFileSaved(wxCommandEvent& e)
{
    e.Skip();
}

void ZoomNavigator::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    DoCleanup();
}

void ZoomNavigator::OnEditorScrolled(wxStyledTextEvent& e)
{
    e.Skip();
    CHECK_CONDITION( m_enabled );
    
    wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(e.GetEventObject());
    if ( ctrl && m_editor ) {
        int curline = ctrl->GetFirstVisibleLine();
        if ( m_lastLine != curline ) {
            DoUpdate();
        }
    }
}

void ZoomNavigator::DoSetEditor(IEditor* editor)
{
    if ( m_editor && editor != m_editor) {
        // changing editor
        m_editor->GetSTC()->Disconnect(wxEVT_STC_PAINTED, wxStyledTextEventHandler(ZoomNavigator::OnEditorScrolled), NULL, this);
    }

    m_editor = editor;
    m_lastLine = wxNOT_FOUND;
    if ( m_editor ) {
        m_lastLine = m_editor->GetSTC()->GetFirstVisibleLine();
        m_editor->GetSTC()->Connect(wxEVT_STC_PAINTED, wxStyledTextEventHandler(ZoomNavigator::OnEditorScrolled), NULL, this);
    }
}

void ZoomNavigator::OnEnablePlugin(wxCommandEvent& e)
{
    znConfigItem data;
    m_config->Reload();
    m_config->ReadItem( &data );
    data.SetEnabled( e.IsChecked() );
    m_config->WriteItem( &data );
    
    // Notify about the settings changed
    wxCommandEvent evt(wxEVT_ZN_SETTINGS_UPDATED);
    EventNotifier::Get()->AddPendingEvent( evt );
}

void ZoomNavigator::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    m_startupCompleted = true;
}
