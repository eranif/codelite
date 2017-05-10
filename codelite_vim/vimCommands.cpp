#include "vimCommands.h"

VimCommand::VimCommand()
{}

VimCommand::~VimCommand()
{}

bool VimCommand::OnNewKeyDown(wxChar ch)
{
	
}

void VimCommand::ResetCommand()
{
	current_cmd_part = COMMAND_PART::REPEAT_NUM;
	mRepeat          = 1;
	mBaseCmd         = '\0';
	mActionCmd       = '\0';
	nActions         = 1;
}
