#include "editorframe.h"
#include "cl_editor.h"
#include "plugin.h"
#include "event_notifier.h"
#include "quickfindbar.h"
#include "bookmark_manager.h"
#include <wx/xrc/xmlres.h>
#include "my_menu_bar.h"
#include "manager.h"

wxDEFINE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, clCommandEvent);

EditorFrame::EditorFrame(wxWindow* parent, LEditor* editor)
    : EditorFrameBase(parent)
    , m_editor(editor)
{
    m_editor->Reparent( m_mainPanel );
    m_mainPanel->GetSizer()->Add(m_editor, 1, wxEXPAND|wxALL, 2);
    
    // Load the menubar from XRC and set this frame's menubar to it.
    wxMenuBar *mb = wxXmlResource::Get()->LoadMenuBar(wxT("main_menu"));
    
    // Under wxGTK < 2.9.4 we need this wrapper class to avoid warnings on ubuntu when codelite exits
    m_myMenuBar = new MyMenuBar();
    m_myMenuBar->Set(mb);
    SetMenuBar(mb);
    
    // Set a find control for this editor
    m_findBar = new QuickFindBar(m_mainPanel);
    m_findBar->SetEditor( m_editor );
    m_mainPanel->GetSizer()->Add(m_findBar, 0, wxEXPAND|wxALL, 2);
    m_findBar->Hide();
    m_toolbar->SetDropdownMenu(XRCID("toggle_bookmark"), BookmarkManager::Get().CreateBookmarksSubmenu(NULL));
    
    m_mainPanel->Layout();
    SetTitle( m_editor->GetFileName().GetFullPath() );
    
    // Update the accelerator table for this frame
    ManagerST::Get()->UpdateMenuAccelerators(this);
}

EditorFrame::~EditorFrame()
{
    // this will make sure that the main menu bar's member m_widget is freed before the we enter wxMenuBar destructor
    // see this wxWidgets bug report for more details:
    //  http://trac.wxwidgets.org/ticket/14292
#if defined(__WXGTK__) && wxVERSION_NUMBER < 2904
    delete m_myMenuBar;
#endif

    clCommandEvent evntInternalClosed(wxEVT_DETACHED_EDITOR_CLOSED);
    evntInternalClosed.SetClientData( (IEditor*)m_editor);
    EventNotifier::Get()->ProcessEvent( evntInternalClosed );

    // Send the traditional plugin event notifying that this editor is about to be destroyed
    wxCommandEvent eventClose(wxEVT_EDITOR_CLOSING);
    eventClose.SetClientData( (IEditor*)m_editor);
    EventNotifier::Get()->ProcessEvent( eventClose );

    m_editor = NULL;
}

void EditorFrame::OnClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Destroy();
}

void EditorFrame::OnCloseUI(wxUpdateUIEvent& event)
{
    event.Enable(true);
}

void EditorFrame::OnFind(wxCommandEvent& event)
{
    if ( !m_findBar->IsShown() ) {
        m_findBar->Show();
        m_mainPanel->GetSizer()->Layout();
    }
}

void EditorFrame::OnFindUI(wxUpdateUIEvent& event)
{
    event.Enable(true);
}
