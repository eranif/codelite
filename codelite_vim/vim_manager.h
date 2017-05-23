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
    VimManager(IManager* manager);
    virtual ~VimManager();

protected:
    void OnEditorChanged(wxCommandEvent& event);
    void OnEditorClosing(wxCommandEvent& event);
    void OnCharEvt(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    wxString get_current_word();
    void DoCleanup();
    
    // Internals
    void CloseCurrentEditor();
    void SaveCurrentEditor();
    
private:
    IEditor* m_editor;
    wxStyledTextCtrl* m_ctrl;
    VimCommand mCurrCmd;          /*!< command currenly */
    VimCommand mLastCmd;          /*!< last command performed */
    wxString mTmpBuf;             /*!< tmporary buffer (of inserted text) */
    std::list<wxString> yank_buf; /*!< list of yanked buffers */
    IManager* m_mgr;
    int mCaretInsStyle;
    int mCaretNormStyle;

    void Issue_cmd();
    void updateView();
    void repeat_cmd();
};

#endif // __VIM_MANAGER_H__
