#ifndef __VIM_MANAGER_H__
#define __VIM_MANAGER_H__

#include "cl_editor.h"
#include "codelite_events.h"

#include <vector>


class VimManager : public wxEvtHandler
{
 public:
	VimManager();
	~VimManager();

 protected:
	void OnEditorChanged(wxCommandEvent &event);
	void OnEditorModified(wxKeyEvent &event);
	void OnKeyUp(wxKeyEvent &event);

	bool insertMode(int keycode);
	bool modaleMode(int keycode);

	bool changeIntoInsMode(int keycode);
	bool arrowMove(int keycode);

	bool isShiftPressed( int keycode );
	
 private:
	IEditor* mEditor;
	wxStyledTextCtrl* ctrl;

	bool shift_pressed;
	
	bool mGiveCommandBar;
	bool mInsertMode;
	bool mChangeCommand;
	
};

#endif // __VIM_MANAGER_H__

