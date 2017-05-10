#ifndef __VIM_COMMANDS__
#define __VIM_COMMANDS__

#include <wx/chartype.h>

enum class COMMAND_PART {
	REPEAT_NUM,
	FIRST_CMD,
	MOD_NUM,
	MOD_CMD
};

enum class COMMAND_TYPE	{
	MOVE,
	REPLACE,
	INSERT,
	CHANGE_MODUS
};

class VimCommand
{
 public:
	VimCommand();
	~VimCommand();

	bool OnNewKeyDown(wxChar ch);
	void ResetCommand();
	
 private:

	COMMAND_PART current_cmd_part;
	
	int mRepeat;
	wxChar mBaseCmd;
	wxChar mActionCmd;
	int nActions;
};

#endif //__VIM_COMMANDS__
