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

/**
 * Default constructor
 */

VimManager::VimManager(IManager* manager, VimSettings& settings)
    : m_settings(settings)
    , mCurrCmd()
    , mLastCmd()
    , mTmpBuf()
{

    m_ctrl = NULL;
    m_editor = NULL;
    m_mgr = manager;
    mCaretInsStyle = 1;
    mCaretNormStyle = 2;

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &VimManager::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING, &VimManager::OnEditorClosing, this);
}

/**
 * Default distructor
 */

VimManager::~VimManager()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &VimManager::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CLOSING, &VimManager::OnEditorClosing, this);
}

/**
 * Method used to see the change of focus of the text editor:
 *
 */
void VimManager::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip(); // Always call Skip() so other plugins/core components will get this event
    if(!m_settings.IsEnabled()) return;
    DoBindCurrentEditor();
}

void VimManager::OnKeyDown(wxKeyEvent& event)
{

    wxChar ch = event.GetUnicodeKey();
    bool skip_event = true;

    if(m_ctrl == NULL || m_editor == NULL) {
        event.Skip();
        return;
    }

    VimCommand::eAction action = VimCommand::kNone;
    if(ch != WXK_NONE) {

        switch(ch) {
        case WXK_ESCAPE:
            if(mCurrCmd.get_current_modus() == VIM_MODI::INSERT_MODUS) {
                mTmpBuf = mCurrCmd.getTmpBuf();
            }
            skip_event = mCurrCmd.OnEscapeDown(m_ctrl);
            break;
        case WXK_RETURN: {

            skip_event = mCurrCmd.OnReturnDown(m_editor, m_mgr, action);
            break;
        }
        default:
            if(mCurrCmd.get_current_modus() == VIM_MODI::SEARCH_MODUS) {
                mCurrCmd.set_current_word(get_current_word());
                mCurrCmd.set_current_modus(VIM_MODI::NORMAL_MODUS);
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

    if(mCurrCmd.get_current_modus() == VIM_MODI::NORMAL_MODUS) {
        m_ctrl->SetCaretStyle(mCaretNormStyle);
        //::clGetManager()->GetStatusBar()->SetMessage("<N>");
        m_mgr->GetStatusBar()->SetMessage("NORMAL");
    } else if(mCurrCmd.get_current_modus() == VIM_MODI::COMMAND_MODUS) {
        m_mgr->GetStatusBar()->SetMessage(mCurrCmd.getTmpBuf());
    } else {
        m_ctrl->SetCaretStyle(mCaretInsStyle);
        //::clGetManager()->GetStatusBar()->SetMessage("<I>");
    }
}

void VimManager::OnCharEvt(wxKeyEvent& event)
{

    bool skip_event = true;
    int modifier_key = event.GetModifiers();
    wxChar ch = event.GetUnicodeKey();

    if(ch != WXK_NONE) {

        switch(ch) {
        case WXK_ESCAPE:
            skip_event = mCurrCmd.OnEscapeDown(m_ctrl);
            break;
        default:
            skip_event = mCurrCmd.OnNewKeyDown(ch, modifier_key);
            /*FIXME save here inser tmp buffer!*/
            break;
        }

    } else {
        skip_event = true;
    }

    if(mCurrCmd.is_cmd_complete()) {

        bool repeat_last = mCurrCmd.repeat_last_cmd();

        if(repeat_last)
            repeat_cmd();
        else
            Issue_cmd();

        if(mCurrCmd.get_current_modus() != VIM_MODI::REPLACING_MODUS) {
            if(repeat_last) {
                mCurrCmd.reset_repeat_last();
            } else if(mCurrCmd.save_current_cmd()) {
                mLastCmd = mCurrCmd;
            }
            mCurrCmd.ResetCommand();
        }
    }

    updateView();
    event.Skip(skip_event);
}

void VimManager::Issue_cmd()
{
    if(m_ctrl == NULL) return;

    mCurrCmd.issue_cmd(m_ctrl);
}

void VimManager::repeat_cmd()
{
    if(m_ctrl == NULL) return;

    mLastCmd.repeat_issue_cmd(m_ctrl, mTmpBuf);
}

void VimManager::CloseCurrentEditor()
{
    CHECK_PTR_RET(m_editor);

    // Fire a close event to the main frame to execute a default close tab operation
    wxCommandEvent eventClose(wxEVT_MENU, XRCID("close_file"));
    eventClose.SetEventObject(EventNotifier::Get()->TopFrame());
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(eventClose);

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

void VimManager::DoCleanup()
{
    if(m_ctrl) {
        m_ctrl->Unbind(wxEVT_CHAR, &VimManager::OnCharEvt, this);
        m_ctrl->Unbind(wxEVT_KEY_DOWN, &VimManager::OnKeyDown, this);
        m_ctrl->SetCaretStyle(mCaretInsStyle);
    }
    m_editor = NULL;
    m_ctrl = NULL;
    m_mgr->GetStatusBar()->SetMessage("");
}

void VimManager::SettingsUpdated()
{
    if(m_settings.IsEnabled()) {
        DoBindCurrentEditor();
    } else {
        DoCleanup();
    }
}

void VimManager::DoBindCurrentEditor()
{
    DoCleanup();
    m_editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(m_editor);

    m_ctrl = m_editor->GetCtrl();
    m_ctrl->Bind(wxEVT_CHAR, &VimManager::OnCharEvt, this);
    m_ctrl->Bind(wxEVT_KEY_DOWN, &VimManager::OnKeyDown, this);

    CallAfter(&VimManager::updateView);
}
