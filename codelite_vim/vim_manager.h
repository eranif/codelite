#ifndef __VIM_MANAGER_H__
#define __VIM_MANAGER_H__

#include <wx/stc/stc.h>
#include "vimCommands.h"
#include "ieditor.h"
#include "event_notifier.h"
#include "macros.h"
#include "imanager.h"
#include "globals.h"
#include "codelite_events.h"
#include <wx/kbdstate.h>
#include "VimSettings.h"
/*Experimental*/
#include <list>

/**
 * @brief This Class is used to intercet the key event end rederect them
 * to perform vi-key-bindings.
 *
 *
 */

class VimManager : public wxEvtHandler
{

public:
    /*ctr-distr ... */
    VimManager(IManager* manager, VimSettings& settings);
    virtual ~VimManager();
    void SettingsUpdated();

protected:
    void OnEditorChanged(wxCommandEvent& event);
    void OnEditorClosing(wxCommandEvent& event);
    void OnWorkspaceClosing(wxCommandEvent& event);
    void OnAllEditorsClosing(wxCommandEvent& event);
    void OnCharEvt(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    wxString get_current_word();
    void DoCleanup(bool unbind = true);
    void DoBindCurrentEditor();

    // Internals
    void CloseCurrentEditor();
    void SaveCurrentEditor();

private:
    VimSettings& m_settings;
    IEditor* m_editor;
    wxStyledTextCtrl* m_ctrl;
    VimCommand m_currentCommand; /*!< command currenly */
    VimCommand m_lastCommand;    /*!< last command performed */
    wxString m_tmpBuf;           /*!< tmporary buffer (of inserted text) */
    IManager* m_mgr;
    int m_caretInsertStyle;
    int m_caretBlockStyle;

    void Issue_cmd();
    void updateView();
    void repeat_cmd();
};

#endif // __VIM_MANAGER_H__
