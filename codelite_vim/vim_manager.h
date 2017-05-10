#ifndef __VIM_MANAGER_H__
#define __VIM_MANAGER_H__

#include <wx/stc/stc.h>
#include "vimCommands.h"
#include "ieditor.h"
#include "event_notifier.h"
#include "macros.h"
#include "ieditor.h"
#include "imanager.h"
#include "globals.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include <wx/kbdstate.h>

enum class VIM_MODI {
	NORMAL_MODUS,
	INSERT_MODUS,
	VISUAL_MODUS,
	COMMAND_MODUS,
	SEARCH_MODUS
};


/**
 * @brief This Class is used to intercet the key event end rederect them 
 * to perform vi-key-bindings.
 *
 * 
 */

class VimManager
{

 public:
	/*ctr-distr ... */
	VimManager();
	~VimManager();

 protected:
	void OnEditorChanged(wxCommandEvent &event);
	void OnCharEvt(wxKeyEvent &event);
 private:

	// IEditor* mEditor;
	wxStyledTextCtrl* mCtrl;
	VIM_MODI current_modus;
	VimCommand mCurrCmd; /*!< command currenly */
	VimCommand mLastCmd; /*!< last command performed */
	wxString mTmpBuf;    /*!< tmporary buffer (of inserted text) */
	
};

#endif // __VIM_MANAGER_H__
