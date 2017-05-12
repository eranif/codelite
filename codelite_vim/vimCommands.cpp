#include "vimCommands.h"

VimCommand::VimCommand()
	: current_cmd_part(COMMAND_PART::REPEAT_NUM),
	  current_modus(VIM_MODI::NORMAL_MODUS),
	  mRepeat(0),
	  mBaseCmd('\0'),
	  mActionCmd('\0'),
	  nActions(0)
{}

VimCommand::~VimCommand()
{}

/**
 *This function is used to update the command
 *@return true if the command can be issue
 */

bool VimCommand::OnNewKeyDown(wxChar ch)
{

	bool skip_event = false;
	
	switch ( current_modus )
	{
	case VIM_MODI::INSERT_MODUS:
		skip_event = true;;
		break;
	case VIM_MODI::NORMAL_MODUS:
		normal_modus ( ch );
		skip_event = false;
		break;
	}

}

bool VimCommand::OnEscapeDown()
{
	current_cmd_part = COMMAND_PART::REPEAT_NUM;
	current_modus = VIM_MODI::NORMAL_MODUS;
}

void VimCommand::ResetCommand()
{
	current_cmd_part = COMMAND_PART::REPEAT_NUM;
	mRepeat          = 0;
	mBaseCmd         = '\0';
	mActionCmd       = '\0';
	nActions         = 0;
}

int VimCommand::getNumRepeat()
{
	if ( mBaseCmd == 'G' )
		return 1;
	
	return ( ( mRepeat > 0 ) ? mRepeat : 1 ); 
}

VIM_MODI VimCommand::get_current_modus()
{
	return current_modus;
}

int VimCommand::getNumActions()
{
	return nActions;
}

void VimCommand::normal_modus( wxChar ch )
{
	switch ( current_cmd_part ) {

	case COMMAND_PART::REPEAT_NUM:

		if (    ( ( ch <= '9' && ch >= '0' ) && mRepeat != 0 )
			 || ( ( ch <= '9' && ch >  '0' ) && mRepeat == 0 ) )
		{
			mRepeat += mRepeat*10 + (int)ch - 48;
		} else {
			mBaseCmd = ch;
		}
		break;

	case COMMAND_PART::MOD_NUM:
		
		if ( ch < '9' && ch > '0' ) {
			nActions += nActions*10 + (int)ch - 48;
		} else {
			mActionCmd = ch;
		}
		break;
	}
}


void VimCommand::Command_call( wxStyledTextCtrl *ctrl )
{

	switch ( command_id ) {
		/*======= MOVEMENT ===========*/	
	case COMMANDVI::j :
		ctrl->LineDown();
		break;
	case COMMANDVI::k :
		ctrl->LineUp();
		break;
	case COMMANDVI::h :
		ctrl->CharLeft();
		break;
	case COMMANDVI::l :
	case COMMANDVI::a :    /*add a char and insert*/
		ctrl->CharRight();
		break;
	case COMMANDVI::_0 :
		ctrl->Home();
		break;
	case COMMANDVI::_$  :
		ctrl->LineEnd();
			break;
	case COMMANDVI::w  :
		ctrl->WordRight();
			break;
	case COMMANDVI::b  :
		ctrl->WordLeft();
			break;
	case COMMANDVI::G  :                /*====== START G =======*/
		switch ( mRepeat ) {
		case 0:
			ctrl->DocumentEnd();
			break;
		case 1:
			ctrl->DocumentStart();
				break;
		default:
			ctrl->GotoLine( mRepeat );
			break;                      
		}
		break;                          /*~~~~~~~ END G ~~~~~~~~*/
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	}
	
}


bool VimCommand::is_cmd_complete()
{

	bool command_complete = false;
	
	switch ( mBaseCmd ) {
	case 'j':
		command_id = COMMANDVI::j;
		command_complete = true;
		break;
	case 'h':
		command_id = COMMANDVI::h;
		command_complete = true;
		break;
	case 'l':
		command_id = COMMANDVI::l;
		command_complete = true;
		break;
	case 'k':
		command_id = COMMANDVI::k;
		command_complete = true;
		break;

	case '$':
		command_id = COMMANDVI::_$;
		command_complete = true;
		break;
	case '0':
		command_id = COMMANDVI::_0;
		command_complete = true;
		break;
	case 'w':
		command_id = COMMANDVI::w;
		command_complete = true;
		break;
	case 'b':
		command_id = COMMANDVI::b;
		command_complete = true;
		break;
	case 'G':
		command_id = COMMANDVI::G;
		command_complete = true;
		break;
		
	case 'i':
		command_id = COMMANDVI::NO_COMMAND;
		current_modus = VIM_MODI::INSERT_MODUS;
		command_complete = true;
		break;

	case 'a':
		command_id = COMMANDVI::a;
		current_modus = VIM_MODI::INSERT_MODUS;
		command_complete = true;
	}

	// TODO: COPY FOR REPEAT .

	return command_complete;
	
}
