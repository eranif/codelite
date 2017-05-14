#ifndef __VIM_COMMANDS__
#define __VIM_COMMANDS__

#include <wx/chartype.h>
#include <wx/stc/stc.h>

enum class COMMAND_PART {
	REPEAT_NUM,
	FIRST_CMD,
	MOD_NUM, /*MOD == MODIFIER*/
	MOD_CMD,
	REPLACING	
};

enum class COMMAND_TYPE	{
	MOVE,
	REPLACE,
	INSERT,
	CHANGE_MODUS,
	CHANGE_INS	
};

enum class VIM_MODI {
	NORMAL_MODUS,
	INSERT_MODUS,
	VISUAL_MODUS,
	COMMAND_MODUS,
	SEARCH_MODUS,
	ISSUE_CMD,
	REPLACING_MODUS	
};

enum class COMMANDVI {
	NO_COMMAND,
	j, k, h, l,
	_0, _$, w, b, G,
	i, I, a, A, o, O,
	u,
	r,R,cw,C,cc, S,
	x, dw, dd, D
};


class VimCommand
{
 public:
	VimCommand();
	~VimCommand();

	bool OnNewKeyDown(wxChar ch);
	bool OnEscapeDown();
	void ResetCommand();
	int  getNumRepeat();
	int  getNumActions();
	
	VIM_MODI get_current_modus();
	bool Command_call( wxStyledTextCtrl *ctrl);
	bool is_cmd_complete();
	
 private:

	void normal_modus( wxChar ch );
	
	COMMANDVI    command_id;
	COMMAND_PART current_cmd_part;
	VIM_MODI     current_modus;
	int          mRepeat;
	wxChar       mBaseCmd;
	wxChar       mActionCmd;
	int          nActions;
	int          nCumulativeUndo;
};

#endif //__VIM_COMMANDS__
