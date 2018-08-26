#include "vim_manager.h"
#include "clStatusBar.h"
#include <wx/xrc/xmlres.h>

/*EXPERIMENT*/
#include "search_thread.h"
#include <wx/tokenzr.h>
#include <wx/fontmap.h>
#include "event_notifier.h"
#include "dirpicker.h"
#include "macros.h"
#include "windowattrmanager.h"
#include <algorithm>
#include "clWorkspaceManager.h"

VimManager::VimManager(IManager* manager, VimSettings& settings)
    : m_settings(settings)
    , m_currentCommand(manager)
    , m_lastCommand(manager)
    , m_tmpBuf()
    , m_editorStates()
    , m_caretInsertStyle(1)
    , m_caretBlockStyle(2)
{

    m_ctrl = NULL;
    m_editor = NULL;
    m_mgr = manager;
    status_vim = NULL;
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &VimManager::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING, &VimManager::OnEditorClosing, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSING, &VimManager::OnWorkspaceClosing, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSING, &VimManager::OnAllEditorsClosing, this);
}

VimManager::~VimManager()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &VimManager::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CLOSING, &VimManager::OnEditorClosing, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSING, &VimManager::OnWorkspaceClosing, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSING, &VimManager::OnAllEditorsClosing, this);
}

/**
 * Method used to see the change of focus of the text editor:
 *
 */
void VimManager::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip();                      // Always call Skip() so other plugins/core components will get this event
    m_currentCommand.set_ctrl(m_ctrl); // Always keep the current editor. Even when disabled. Otherwise, when opening an
                                       // editor and *then* enabling the VIM plugin, it may lead to crashes
    if(!m_settings.IsEnabled()) return;
    IEditor* editor = reinterpret_cast<IEditor*>(event.GetClientData());
    SaveOldEditorState();

    DoBindEditor(editor);
}

void VimManager::OnKeyDown(wxKeyEvent& event)
{

    wxChar ch = event.GetUnicodeKey();
    bool skip_event = true;

    if(m_ctrl == NULL || m_editor == NULL || !m_settings.IsEnabled()) {
        event.Skip();
        return;
    }

    VimCommand::eAction action = VimCommand::kNone;
    if(ch != WXK_NONE) {

        switch(ch) {
        case WXK_BACK: {
            // Delete the last comand char, if not in command mode, return true (for skip event)
            skip_event = !(m_currentCommand.DeleteLastCommandChar());
            break;
        }
        case WXK_ESCAPE:
            if(m_currentCommand.get_current_modus() == VIM_MODI::INSERT_MODUS) {
                m_tmpBuf = m_currentCommand.getTmpBuf();
            } else if(m_currentCommand.get_current_modus() == VIM_MODI::VISUAL_MODUS) {
                long pos = m_ctrl->GetCurrentPos();
                m_ctrl->ClearSelections();
                m_ctrl->GotoPos(pos);
            }
            skip_event = m_currentCommand.OnEscapeDown();
            break;
        case WXK_RETURN: {

            skip_event = m_currentCommand.OnReturnDown(action);
            status_vim->Show(false);
            break;
        }
        default:
            if(m_currentCommand.get_current_modus() == VIM_MODI::SEARCH_CURR_MODUS) {
                m_currentCommand.set_current_word(get_current_word());
                m_currentCommand.set_current_modus(VIM_MODI::NORMAL_MODUS);
            }
            skip_event = true;
            break;
        }

    } else {
        skip_event = true;
    }

    updateView();
    event.Skip(skip_event);

    // Execute the action (this will done in the next event loop)
    switch(action) {
    case VimCommand::kClose:
        CallAfter(&VimManager::CloseCurrentEditor);
        break;
    case VimCommand::kSave:
        CallAfter(&VimManager::SaveCurrentEditor);
        break;
    case VimCommand::kSaveAndClose:
        CallAfter(&VimManager::SaveCurrentEditor);
        CallAfter(&VimManager::CloseCurrentEditor);
        break;
    default:
        break;
    }
}

wxString VimManager::get_current_word()
{
    long pos = m_ctrl->GetCurrentPos();
    long start = m_ctrl->WordStartPosition(pos, true);
    long end = m_ctrl->WordEndPosition(pos, true);
    wxString word = m_ctrl->GetTextRange(start, end);
    return word;
}

void VimManager::updateView()
{
    if(m_ctrl == NULL) return;

    updateCarret();

    if(m_currentCommand.getError() == MESSAGES_VIM::NO_ERROR_VIM_MSG) {
        updateMessageModus();
    } else {
        updateVimMessage();
    }
}

void VimManager::updateMessageModus()
{
    switch(m_currentCommand.get_current_modus()) {
    case VIM_MODI::NORMAL_MODUS:
        m_mgr->GetStatusBar()->SetMessage("NORMAL");
        if(status_vim->IsShown()) status_vim->Show(false);
        break;
    case VIM_MODI::COMMAND_MODUS:
    case VIM_MODI::SEARCH_MODUS:
        // m_mgr->GetStatusBar()->SetMessage(m_currentCommand.getTmpBuf());
        m_tmpBuf = m_currentCommand.getTmpBuf();
        setUpVimBar();
        status_vim->SetStatusText(m_tmpBuf);
        if(!status_vim->IsShown()) status_vim->Show(true);
        break;
    case VIM_MODI::VISUAL_MODUS:
        m_mgr->GetStatusBar()->SetMessage("VISUAL");
        if(status_vim->IsShown()) status_vim->Show(false);
        break;
    case VIM_MODI::INSERT_MODUS:
        m_mgr->GetStatusBar()->SetMessage("INSERT");
        if(status_vim->IsShown()) status_vim->Show(false);
        break;
    default:
        m_mgr->GetStatusBar()->SetMessage("NORMAL");
        if(status_vim->IsShown()) status_vim->Show(false);
        break;
    }
}

void VimManager::updateVimMessage()
{
    switch(m_currentCommand.getError()) {
    case MESSAGES_VIM::UNBALNCED_PARENTESIS_VIM_MSG:
        m_mgr->GetStatusBar()->SetMessage(_("Unbalanced Parentesis"));
        break;
    case MESSAGES_VIM::SAVED_VIM_MSG:
        m_mgr->GetStatusBar()->SetMessage(_("Saving"));
        break;
    case MESSAGES_VIM::CLOSED_VIM_MSG:
        m_mgr->GetStatusBar()->SetMessage(_("Closing"));
        break;
    case MESSAGES_VIM::SAVE_AND_CLOSE_VIM_MSG:
        m_mgr->GetStatusBar()->SetMessage(_("Saving and Closing"));
        break;
    case MESSAGES_VIM::SEARCHING_WORD:
        m_mgr->GetStatusBar()->SetMessage("Searching: " + m_currentCommand.getSearchedWord());
        break;
    default:
        m_mgr->GetStatusBar()->SetMessage("Unknown Error");
        break;
    }
}

void VimManager::updateCarret()
{

    switch(m_currentCommand.get_current_modus()) {
    case VIM_MODI::NORMAL_MODUS:
        m_ctrl->SetCaretStyle(m_caretBlockStyle);
        break;
    case VIM_MODI::COMMAND_MODUS:
        m_ctrl->SetCaretStyle(m_caretBlockStyle);
        break;
    case VIM_MODI::VISUAL_MODUS:
        m_ctrl->SetCaretStyle(m_caretBlockStyle);
        break;
    case VIM_MODI::INSERT_MODUS:
        m_ctrl->SetCaretStyle(m_caretInsertStyle);
        break;
    default:
        m_ctrl->SetCaretStyle(m_caretBlockStyle);
        break;
    }
}

void VimManager::OnCharEvt(wxKeyEvent& event)
{
    if(!m_settings.IsEnabled()) {
        event.Skip();
        return;
    }

    bool skip_event = true;
    int modifier_key = event.GetModifiers();
    wxChar ch = event.GetUnicodeKey();

    if(ch != WXK_NONE) {

        switch(ch) {
        case WXK_ESCAPE:
            skip_event = m_currentCommand.OnEscapeDown();
            if(status_vim->IsShown()) status_vim->Show(false);
            break;
        default:
            skip_event = m_currentCommand.OnNewKeyDown(ch, modifier_key);
            /*FIXME save here inser tmp buffer!*/
            break;
        }

    } else {
        skip_event = true;
    }

    if(m_currentCommand.is_cmd_complete()) {

        bool repeat_last = m_currentCommand.repeat_last_cmd();

        if(repeat_last) {
            m_lastCommand.set_ctrl(m_ctrl);
            RepeatCommand();
        } else {
            IssueCommand();
        }

        if(m_currentCommand.get_current_modus() != VIM_MODI::REPLACING_MODUS) {
            if(repeat_last) {
                m_currentCommand.reset_repeat_last();
            } else if(m_currentCommand.save_current_cmd()) {
                m_lastCommand = m_currentCommand;
            }
            m_currentCommand.ResetCommand();
        }
    }

    updateView();
    event.Skip(skip_event);
}

void VimManager::IssueCommand()
{
    if(m_ctrl == NULL) return;
    m_currentCommand.set_ctrl(m_ctrl);
    m_currentCommand.IssueCommand();
}

void VimManager::RepeatCommand()
{
    if(m_ctrl == NULL) return;

    m_lastCommand.RepeatIssueCommand(m_tmpBuf);
}

void VimManager::CloseCurrentEditor()
{
    CHECK_PTR_RET(m_editor);

    // Fire a close event to the main frame to execute a default close tab operation
    wxCommandEvent eventClose(wxEVT_MENU, XRCID("close_file"));
    eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(eventClose);
    DeleteClosedEditorState();
    DoCleanup();
}

void VimManager::SaveCurrentEditor()
{
    CHECK_PTR_RET(m_editor);

    // Save the editor
    m_editor->Save();
}

void VimManager::OnEditorClosing(wxCommandEvent& event)
{
    event.Skip();
    DoCleanup();
}

void VimManager::DoCleanup(bool unbind)
{
    if(m_ctrl && unbind) {
        m_ctrl->Unbind(wxEVT_CHAR, &VimManager::OnCharEvt, this);
        m_ctrl->Unbind(wxEVT_KEY_DOWN, &VimManager::OnKeyDown, this);
        m_ctrl->SetCaretStyle(m_caretInsertStyle);
    }

    m_editor = NULL;
    m_ctrl = NULL;
    status_vim = NULL;
    // m_mgr->GetStatusBar()->SetMessage("");
}

void VimManager::SettingsUpdated()
{
    if(m_settings.IsEnabled()) {
        DoBindEditor(m_mgr->GetActiveEditor());
    } else {
        DoCleanup();
    }
}

void VimManager::DoBindEditor(IEditor* editor)
{
    DoCleanup();

    m_editor = editor;
    CHECK_PTR_RET(m_editor);

    UpdateOldEditorState();

    m_ctrl = m_editor->GetCtrl();
    m_ctrl->Bind(wxEVT_CHAR, &VimManager::OnCharEvt, this);
    m_ctrl->Bind(wxEVT_KEY_DOWN, &VimManager::OnKeyDown, this);

    /*baby-steps*/
    setUpVimBar();

    // CallAfter(&VimManager::updateView);
    updateView();
}

void VimManager::setUpVimBar()
{

    if(status_vim != NULL) delete status_vim;

    // wxWindow* parent = m_ctrl->GetParent();
    wxWindow* parent = (wxWindow*)m_ctrl;
    status_vim = new wxStatusBar(parent, 1);
    // status_vim->Show( true );
    status_vim->SetFieldsCount(1);

    setUpVimBarPos();
}

void VimManager::setUpVimBarPos()
{
    int hight;
    int width;
    wxWindow* parent = (wxWindow*)m_ctrl;
    parent->GetSize(&width, &hight);
    // status_vim->Show( true );
    status_vim->SetSize(wxDefaultCoord, wxDefaultCoord, width, wxDefaultCoord);
    // status_vim->SetSize(0, 0, width, wxDefaultCoord);
}

void VimManager::OnWorkspaceClosing(wxCommandEvent& event)
{
    event.Skip();
    DeleteAllEditorState();
    DoCleanup(false);
}

void VimManager::OnAllEditorsClosing(wxCommandEvent& event)
{
    event.Skip();
    DeleteAllEditorState();
    DoCleanup(false);
}

void VimManager::UpdateOldEditorState()
{

    wxString fullpath_name = m_editor->GetFileName().GetFullPath();

    for(auto status_editor = m_editorStates.begin(); status_editor != m_editorStates.end(); ++status_editor) {

        if((*status_editor)->isCurrentEditor(fullpath_name)) {
            (*status_editor)->setSavedStatus(m_currentCommand);
            return;
        }
    }

    // if one arrived here, it is a new editor
    m_editorStates.push_back(new VimBaseCommand(fullpath_name));
}

void VimManager::SaveOldEditorState()
{
    if(!m_editor) return;

    wxString fullpath_name = m_editor->GetFileName().GetFullPath();
    for(auto status_editor = m_editorStates.begin(); status_editor != m_editorStates.end(); ++status_editor) {
        if((*status_editor)->isCurrentEditor(fullpath_name)) {
            (*status_editor)->saveCurrentStatus(m_currentCommand);
            return;
        }
    }

    // if one arrived here, it is a new editor
    m_editorStates.push_back(new VimBaseCommand(fullpath_name));
}

void VimManager::DeleteClosedEditorState()
{
    if(!m_editor) return;

    wxString fullpath_name = m_editor->GetFileName().GetFullPath();
    for(auto status_editor = m_editorStates.begin(); status_editor != m_editorStates.end(); ++status_editor) {
        if((*status_editor)->isCurrentEditor(fullpath_name)) {
            m_editorStates.erase(status_editor);
            return;
        }
    }
}

void VimManager::DeleteAllEditorState() { m_editorStates.clear(); }
