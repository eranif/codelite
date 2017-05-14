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
		skip_event = true;
		break;
	case VIM_MODI::NORMAL_MODUS:
	case VIM_MODI::REPLACING_MODUS:
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

	const int shift_ashii_num = 48;
	
	switch ( current_cmd_part ) {

	case COMMAND_PART::REPEAT_NUM:

		if (    ( ( ch <= '9' && ch >= '0' ) && mRepeat != 0 )
			 || ( ( ch <= '9' && ch >  '0' ) && mRepeat == 0 ) )
		{
			mRepeat += mRepeat*10 + (int)ch - shift_ashii_num;
		} else {
			mBaseCmd = ch;
			if ( mBaseCmd == 'R' ) {
				current_modus = VIM_MODI::REPLACING_MODUS;
				current_cmd_part = COMMAND_PART::REPLACING;
			} else {
				current_cmd_part = COMMAND_PART::MOD_NUM;
			}
		}
		break;

	case COMMAND_PART::MOD_NUM:
		
		if ( ch < '9' && ch > '0' && mBaseCmd != 'r' ) {
			nActions += nActions*10 + (int)ch - shift_ashii_num;
		} else {
			mActionCmd = ch;
		}
		break;

	case COMMAND_PART::REPLACING :
		mActionCmd = ch;
		break;
	}
}


bool VimCommand::Command_call( wxStyledTextCtrl *ctrl )
{

	bool repeat_command = true;
	
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

		/*========= PUT IN INSERT MODE =============*/
	case COMMANDVI::i :
			break;
	case COMMANDVI::I :
			ctrl->Home();
			break;
	case COMMANDVI::a :
			ctrl->CharRight();
			break;
	case COMMANDVI::A :
		ctrl->LineEnd();
			break;
	case COMMANDVI::o :
		ctrl->LineEnd();
		ctrl->NewLine();
			break;
	case COMMANDVI::O :
		ctrl->LineUp();
		ctrl->LineEnd();
		ctrl->NewLine();
		break;

		/*========= UNDO =============*/
		/* FIXME: the undo works strange with 'r' command*/
	case COMMANDVI::u :
		ctrl->Undo();
		break;
		
		/*======== REPLACE ===========*/
	case COMMANDVI::r :
		ctrl->CharRight();
		ctrl->DeleteBackNotLine();
		ctrl->AddText(mActionCmd);
		break;
	case COMMANDVI::R :
		ctrl->CharRight();
		ctrl->DeleteBackNotLine();
		ctrl->AddText(mActionCmd);
		repeat_command = false;
		break;
	case COMMANDVI::cw :
		ctrl->DelWordRight();
		break;

	case COMMANDVI::S :
		{
			int repeat_S = std::max( 1, mRepeat );
			for ( int i = 0; i < repeat_S ; ++i ) {
				ctrl->LineDelete();
			}
			break;
		}
	case COMMANDVI::C :
		ctrl->DelLineRight();
		if ( mRepeat > 1 ) {
			ctrl->LineDown();
			for ( int i = 0; i < mRepeat - 1; ++i ) { // delete extra line if [num]C
				ctrl->LineDelete();
			}
			ctrl->LineUp();
			ctrl->LineEnd();
		}
		repeat_command = false;
		break;
		
	case COMMANDVI::cc :
	{
		int repeat_cc = std::max( 1, mRepeat + nActions );
		
		for ( int i = 0; i < repeat_cc; ++i ) {
			ctrl->LineDelete();
		}
		//ctrl->NewLine();
		repeat_command = false;
		break;
	}
	
	case COMMANDVI::x :
		ctrl->CharRight();
		ctrl->DeleteBackNotLine();
		break;
	case COMMANDVI::dw :
		ctrl->DelWordRight();
		break;
	case COMMANDVI::dd :
		ctrl->LineDelete();
		break;
		
	case COMMANDVI::D:
		ctrl->DelLineRight();
		break;

	}
	
	return repeat_command;
	
}


bool VimCommand::is_cmd_complete()
{

	bool command_complete = false;
	
	switch ( mBaseCmd ) {

		/*========================== MOVEMENT =========================*/
		{
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
		}
		/*===================== CANGE INTO INSER ====================*/
		{
	case 'i':
		command_id = COMMANDVI::i;
		current_modus = VIM_MODI::INSERT_MODUS;
		command_complete = true;
		mRepeat = 1;
		break;
	case 'I':
		command_id = COMMANDVI::I;
		current_modus = VIM_MODI::INSERT_MODUS;
		command_complete = true;
		mRepeat = 1;
		break;

	case 'a':
		command_id = COMMANDVI::a;
		current_modus = VIM_MODI::INSERT_MODUS;
		command_complete = true;
		mRepeat = 1;
		break;
	case 'A':
		command_id = COMMANDVI::A;
		current_modus = VIM_MODI::INSERT_MODUS;
		command_complete = true;
		mRepeat = 1;
		break;

	case 'o':
		command_id = COMMANDVI::o;
		current_modus = VIM_MODI::INSERT_MODUS;
		command_complete = true;
		mRepeat = 1;
		break;
	case 'O':
		command_id = COMMANDVI::O;
		current_modus = VIM_MODI::INSERT_MODUS;
		command_complete = true;
		mRepeat = 1;
		break;
		}
		/*===================== UNDO ====================*/
	case 'u':
		command_id = COMMANDVI::u;
		command_complete = true;
		break;

		/*==================== REPLACE =================*/
	case 'r' :
		if ( mActionCmd == '\0' ) {
			command_complete = false;
		} else {
			command_complete = true;
			command_id = COMMANDVI::r;
		}
		break;

	case 'R':
		if ( mActionCmd == '\0' ) {
			command_complete = false;
		} else {
			command_complete = true;
			command_id = COMMANDVI::R;
		}
		break;

	case 'c':                                /*~~~~~~ c[...] ~~~~~~~*/
		switch ( mActionCmd ) {
		case '\0' :
			command_complete = false;
			break;
		case 'w' :
			command_complete  = true;
			command_id = COMMANDVI::cw;
			current_modus = VIM_MODI::INSERT_MODUS;
			break;
		case 'c':
			command_complete = true;
			command_id = COMMANDVI::cc;
			current_modus = VIM_MODI::INSERT_MODUS;
			break;
		}
	case 'S':
		command_complete = true;
		command_id = COMMANDVI::S;
		current_modus = VIM_MODI::INSERT_MODUS;
		break;
		
	case 'C':
		command_complete = true;
		command_id = COMMANDVI::C;
		current_modus = VIM_MODI::INSERT_MODUS;
		break;
	case 'x':
		break;
	case 'd':                               /*~~~~~~~ d[...] ~~~~~~~~*/
		switch ( mActionCmd ) {
		case '\0' :
			command_complete = false;
			break;
		case 'w' :
			command_complete  = true;
			command_id = COMMANDVI::dw;
			break;
		case 'd':
			command_complete = true;
			command_id = COMMANDVI::dd;
			break;
		}
		break;
	case 'D':
		command_complete = true;
		command_id = COMMANDVI::D;
		break;
	}

	/*===================== DELETE TEXT ===============*/

	// TODO: COPY FOR REPEAT .

	return command_complete;
	
}
