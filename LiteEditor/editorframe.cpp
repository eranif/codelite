#include "editorframe.h"
#include "cl_editor.h"
#include "plugin.h"
#include "event_notifier.h"

wxDEFINE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, clCommandEvent);

EditorFrame::EditorFrame(wxWindow* parent, LEditor* editor)
    : EditorFrameBase(parent)
    , m_editor(editor)
{
    m_editor->Reparent( m_mainPanel );
    m_mainPanel->GetSizer()->Add(m_editor, 1, wxEXPAND|wxALL, 2);
    m_mainPanel->Layout();
    SetTitle( m_editor->GetFileName().GetFullPath() );
}

EditorFrame::~EditorFrame()
{
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

void EditorFrame::OnReload(wxCommandEvent& event)
{
    m_editor->ReloadFile();
}

void EditorFrame::OnReloadUI(wxUpdateUIEvent& event)
{
    event.Enable(true);
}

void EditorFrame::OnSave(wxCommandEvent& event)
{
    m_editor->SaveFile();
}

void EditorFrame::OnSaveUI(wxUpdateUIEvent& event)
{
    event.Enable(m_editor->GetModify());
}
