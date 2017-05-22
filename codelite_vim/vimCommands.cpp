#include "vimCommands.h"

#include "macros.h"
#include "imanager.h"
#include "globals.h"
#include "codelite_events.h"
#include "clMainFrameHelper.h"
/*EXPERIMENTAL*/
#include "wx/uiaction.h"

VimCommand::VimCommand()
    : current_cmd_part(COMMAND_PART::REPEAT_NUM),
      current_modus(VIM_MODI::NORMAL_MODUS),
      command_id(COMMANDVI::NO_COMMAND),
      mRepeat(0),
      mBaseCmd('\0'),
      mActionCmd('\0'),
      nActions(0),
	  list_copied_str(),
      nCumulativeUndo(0),
      modifier_key(0),
      tmpBuf(),
      m_search_word(),
      new_line_cpy(false),
      repeat_cmd(false),
      save_cmd(true)
{}

VimCommand::~VimCommand()
{}

void VimCommand::set_current_word( wxString word)
{
    m_search_word = word;
}
/**
 *This function is used to update the command
 *@return true if the command can be issue
 */

bool VimCommand::OnNewKeyDown(wxChar ch, int modifier)
{
    
    bool skip_event = false;
    this->modifier_key = modifier;

    switch ( current_modus )
    {
    case VIM_MODI::INSERT_MODUS:
        insert_modus( ch );
        skip_event = true;
        break;
    case VIM_MODI::SEARCH_MODUS:
        //get_word_at_position( ctrl);
        current_modus = VIM_MODI::NORMAL_MODUS;
    case VIM_MODI::NORMAL_MODUS:
    case VIM_MODI::REPLACING_MODUS:
        normal_modus ( ch );
        skip_event = false;
        break;
    case VIM_MODI::COMMAND_MODUS:
        command_modus( ch );
        skip_event = false;
        break;
    }
    
    return skip_event;
}

/**
 * This function is used to deal with the Esc press event.
 * Mostly it terminates insert/replace mode and push back into
 * normal mode
 */
bool VimCommand::OnEscapeDown( wxStyledTextCtrl *ctrl )
{

    current_cmd_part = COMMAND_PART::REPEAT_NUM;
    current_modus = VIM_MODI::NORMAL_MODUS;
    tmpBuf.Clear();
}

/**
 * used to reset the command.
 */
void VimCommand::ResetCommand()
{
    current_cmd_part = COMMAND_PART::REPEAT_NUM;
    mRepeat          = 0;
    mBaseCmd         = '\0';
    mActionCmd       = '\0';
    nActions         = 0;
}

/**
 * get for the number of repetition a command has to be issued
 * Note: if the command is of <num>+'G' type, the repetition change
 * drasically the command, that's why there's a special behaviour.
 */
int VimCommand::getNumRepeat()
{
    if ( mBaseCmd == 'G' ||
         mBaseCmd == 'g'    )
        return 1;
    
    return ( ( mRepeat > 0 ) ? mRepeat : 1 ); 
}

/**
 *return the modus (insert/normal/serach/visual ...) we are actually working with.
 * Use mostly to comunicate with VimManager and differentiate the handliing of 
 * key event.
 */
VIM_MODI VimCommand::get_current_modus()
{
    return current_modus;
}

wxString VimCommand::getTmpBuf()
{
    return tmpBuf;
}

/**
 * Getter for the number of action for the modifier command.
 * NOTE:
 * most of the commands in vi have the following abstract structure:
 * <repetition of command > + < base command > + [ <extra> ].
 * Some commands have an extra argument made up of <number> + <command>, hier called
 * <nAction> + <mActionCmd>.
 * ~~~~~~~~~~~~~~~~
 * example:
 * ~~~~~~~~~~~~~~~~
 * command >> 5d3w 
 * means [5] times [d]elete [3] [w]ords
 */
int VimCommand::getNumActions()
{
    return nActions;
}

void VimCommand::insert_modus( wxChar ch)
{
    tmpBuf.Append( ch );
}
/**
 * This function is used to deal with the key event when we are in the 
 * normal modus.
 */
void VimCommand::normal_modus( wxChar ch )
{

    const int shift_ashii_num = 48;
    
    switch ( current_cmd_part ) {

    case COMMAND_PART::REPEAT_NUM:

        if (    ( ( ch <= '9' && ch >= '0' ) && mRepeat != 0 )
             || ( ( ch <= '9' && ch >  '0' ) && mRepeat == 0 ) )
        {
            mRepeat = mRepeat*10 + (int)ch - shift_ashii_num;
        } else {
            mBaseCmd = ch;
            switch ( mBaseCmd ) {
            case 'R':
                current_modus = VIM_MODI::REPLACING_MODUS;
                current_cmd_part = COMMAND_PART::REPLACING;
                break;
            case ':':
                current_modus = VIM_MODI::COMMAND_MODUS;
                tmpBuf.Append( ch );
                break;
            default:
                current_cmd_part = COMMAND_PART::MOD_NUM;
                break;
            } 
        }
        break;

    case COMMAND_PART::MOD_NUM:
        
        if ( ch < '9' && ch > '0' && mBaseCmd != 'r' ) {
            nActions = nActions*10 + (int)ch - shift_ashii_num;
        } else {
            mActionCmd = ch;
        }
        break;

    case COMMAND_PART::REPLACING :
        mActionCmd = ch;
        break;
    }
}

bool VimCommand::OnReturnDown(IEditor **editor, IManager *manager)
{
    bool skip_event = true;
    //wxUIActionSimulator sim;

    if ( current_modus == VIM_MODI::COMMAND_MODUS )
    {
        if ( tmpBuf == _(":w") || tmpBuf == _(":write") ) {
            if( (*editor)->IsModified() && (*editor)->GetFileName().Exists()) {
                // Don't auto-save remote files marked with "SFTP"
                if(!(*editor)->GetClientData("sftp")) {
                    (*editor)->Save();
                }
            }

            skip_event = false;
            tmpBuf.Clear();
            ResetCommand();
            current_modus    = VIM_MODI::NORMAL_MODUS;
        }
        else if ( tmpBuf == _(":q") || tmpBuf == _(":quit") ) {
            manager->CloseEditor( *editor, true );
            skip_event = false;
            (*editor) = NULL;
            tmpBuf.Clear();
            ResetCommand();
            //current_modus    = VIM_MODI::NORMAL_MODUS;
            //sim.Char('W', wxMOD_CONTROL);
            //wxYield();
        } else if ( tmpBuf == _(":q!")  ) {
            manager->CloseEditor( *editor, true );
            skip_event = false;
            (*editor) = NULL;
            tmpBuf.Clear();
            ResetCommand();
        }else if ( tmpBuf == _(":wq") ) {
            (*editor)->Save();
            manager->CloseEditor( *editor, false );
            (*editor) = NULL;
            skip_event = false;
            tmpBuf.Clear();
            ResetCommand();
        }
    }

    return skip_event;
}

/**
 * function dealing with the command status (i.e. ": ... " )
 */
void VimCommand::command_modus( wxChar ch )
{
    tmpBuf.Append( ch );
}

/**
 * This function call on the controller of the actual editor the vim-command.
 * Here is the actual implementation of the binding.
 */
bool VimCommand::Command_call( wxStyledTextCtrl *ctrl )
{
    wxUIActionSimulator sim;
    bool repeat_command = true;
    this->save_cmd = true; 
    switch ( command_id ) {
        /*======= MOVEMENT ===========*/    
    
    case COMMANDVI::j :
        ctrl->LineDown();
        this->save_cmd = false;
        break;
    case COMMANDVI::k :
        ctrl->LineUp();
        this->save_cmd = false;
        break;
    case COMMANDVI::h :
        ctrl->CharLeft();
        this->save_cmd = false;
        break;
    case COMMANDVI::l :
        ctrl->CharRight();
        this->save_cmd = false;
        break;
    case COMMANDVI::_0 :
        ctrl->Home();
        this->save_cmd = false;
        break;
    case COMMANDVI::_$  :
        ctrl->LineEnd();
        this->save_cmd = false;
            break;
    case COMMANDVI::w  :
        ctrl->WordRight();
        this->save_cmd = false;
            break;
    case COMMANDVI::b  :
        ctrl->WordLeft();
        this->save_cmd = false;
            break;
	case COMMANDVI::ctrl_D :
		ctrl->PageDown();
        this->save_cmd = false;
		break;
	case COMMANDVI::ctrl_U :
		ctrl->PageUp();
        this->save_cmd = false;
		break;
    case COMMANDVI::G  :                /*====== START G =======*/
        this->save_cmd = false;
        switch ( mRepeat ) {
        case 0:
            ctrl->DocumentEnd();
            break;
        case 1:
            ctrl->DocumentStart();
                break;
        default:
            ctrl->GotoLine( mRepeat -1 );
            break;                      
        }
        break;                          /*~~~~~~~ END G ~~~~~~~~*/
    case COMMANDVI::gg  :                /*====== START G =======*/
        this->save_cmd = false;
        if ( mRepeat == 0) {
            mRepeat = 1;
        }
        ctrl->GotoLine( mRepeat -1 );
        repeat_command = false;
        break;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        /*========= PUT IN INSERT MODE =============*/
    case COMMANDVI::i :
        this->tmpBuf.Clear();
        break;
    case COMMANDVI::I :
        this->tmpBuf.Clear();
        ctrl->Home();
        break;
    case COMMANDVI::a :
        this->tmpBuf.Clear();
        ctrl->CharRight();
        break;
    case COMMANDVI::A :
        this->tmpBuf.Clear();
        ctrl->LineEnd();
            break;
    case COMMANDVI::o :
        this->tmpBuf.Clear();
        ctrl->LineEnd();
        ctrl->NewLine();
            break;
    case COMMANDVI::O :
        this->tmpBuf.Clear();
        ctrl->LineUp();
        ctrl->LineEnd();
        ctrl->NewLine();
        break;
	case COMMANDVI::perc:
		/*FIXME*/
		//sim.Char(']', wxMOD_SHIFT);
        //wxYield();
		break;
        /*========= UNDO =============*/
        /* FIXME: the undo works strange with 'r' command*/
    case COMMANDVI::u :
        ctrl->Undo();
        break;
        
        /*======== REPLACE ===========*/
    case COMMANDVI::r :
        this->tmpBuf.Clear();
        ctrl->CharRight();
        ctrl->DeleteBackNotLine();
        ctrl->AddText(mActionCmd);
        break;
    case COMMANDVI::R :
        this->tmpBuf.Clear();
        ctrl->CharRight();
        ctrl->DeleteBackNotLine();
        ctrl->AddText(mActionCmd);
        repeat_command = false;
        break;
    case COMMANDVI::cw :
        this->tmpBuf.Clear();
        ctrl->DelWordRight();
        break;

    case COMMANDVI::S :
    {
        this->tmpBuf.Clear();
        int repeat_S = std::max( 1, mRepeat );
        for ( int i = 0; i < repeat_S ; ++i ) {
            ctrl->LineDelete();
        }
        ctrl->NewLine();
        ctrl->LineUp();
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
        this->tmpBuf.Clear();
        ctrl->CharRight();
        ctrl->DeleteBackNotLine();
        break;
    case COMMANDVI::dw :
        list_copied_str.push_back( get_word_at_position( ctrl ) ); 
        if ( is_space_following( ctrl ) ) 
            list_copied_str.push_back( ' ' ); 
        new_line_cpy = false;
        ctrl->DelWordRight();
        break;
    case COMMANDVI::dd :
        {
            int linePos;
            this->list_copied_str.push_back( ctrl->GetCurLine( &linePos ) );
            ctrl->LineDelete();
        }
        break;
        
    case COMMANDVI::D:
        ctrl->DelLineRight();
        break;
    /*=============== COPY ====================*/
    case COMMANDVI::yw :
        list_copied_str.push_back( get_word_at_position( ctrl ) ); 
        if ( is_space_following( ctrl ) ) 
            list_copied_str.push_back( ' ' ); 
        new_line_cpy = false;
        break;
    case COMMANDVI::yy :
        {
            int linePos;
            this->list_copied_str.push_back( ctrl->GetCurLine( &linePos ) );
        }
        break;
    
    case COMMANDVI::diesis:
    {
        m_search_word = get_word_at_position( ctrl );
            //ctrl->SetCurrentPos( /*FIXME*/ );
        search_word( SEARCH_DIRECTION::BACKWARD, ctrl );
    }
        break;

    case COMMANDVI::slesh :
        sim.Char('F', wxMOD_CONTROL);
        current_modus = VIM_MODI::SEARCH_MODUS;
        wxYield();
        this->save_cmd = false;
        break;

    case COMMANDVI::N :
        search_word( SEARCH_DIRECTION::BACKWARD, ctrl );
        break;
        this->save_cmd = false;

    case COMMANDVI::n :
        search_word( SEARCH_DIRECTION::FORWARD, ctrl );
        this->save_cmd = false;
        break;

    /*=============================== PASTE =========================*/
    // FIXME: One line too many !!!
    case COMMANDVI::P :
        this->save_cmd = false;
        if ( this->new_line_cpy ){
            ctrl->LineEnd();
            ctrl->NewLine();
        } else {
            ctrl->CharLeft();    /*Is one char before andd the the same as p!*/
        }

        for ( std::vector<wxString>::iterator 
              yanked = this->list_copied_str.begin();
              yanked != this->list_copied_str.end();
              ++yanked ) 
            {
                ctrl->AddText( *yanked );
            }
            // FIXME: troppo contorto!
            if ( this->new_line_cpy )
                ctrl->LineDelete();
        break;
    case COMMANDVI::p :
        this->save_cmd = false;
        if ( this->new_line_cpy ) {
            ctrl->LineUp();
            ctrl->LineEnd();
            ctrl->NewLine();
        }
        for ( std::vector<wxString>::iterator 
              yanked = this->list_copied_str.begin();
              yanked != this->list_copied_str.end();
              ++yanked ) 
            {
                ctrl->AddText( *yanked );
            }
            // FIXME: troppo contorto!
            if ( this->new_line_cpy )
                ctrl->LineDelete();
        break;
    case COMMANDVI::repeat :
        //repeat_cmd( ctrl );
        this->save_cmd = false;
        this->repeat_cmd = true;
        break;
    }
    
    return repeat_command;
    
}

wxString VimCommand::get_word_at_position(wxStyledTextCtrl *ctrl)
{
    long pos = ctrl->GetCurrentPos();
    long start = ctrl->WordStartPosition(pos, true);
    long end = ctrl->WordEndPosition(pos, true);
    wxString word = ctrl->GetTextRange(start, end);
    return word;
}


bool VimCommand::is_space_following( wxStyledTextCtrl *ctrl)
{
    long pos = ctrl->GetCurrentPos();
    long end = ctrl->WordEndPosition( pos, true );
    if ( ctrl->GetCharAt( end ) == ' ' )
        return true;

    return false;
}

bool VimCommand::search_word( SEARCH_DIRECTION direction, wxStyledTextCtrl *ctrl )
{
    
    // /*flag 2: word separated, Big pr small!*/
    // /*flag 3: same as before*/
    // /*flag 1-0: from the find other*/
    long pos = ctrl->GetCurrentPos();
    bool found = false;
    int flag = 3;
    int pos_prev;
    if ( direction == SEARCH_DIRECTION::BACKWARD ) {
        pos_prev = ctrl->FindText( 0, pos, m_search_word, flag);
    } else {
		ctrl->CharRight();
		int pos_end_word = ctrl->WordEndPosition(pos, true);
        pos_prev = ctrl->FindText( pos_end_word + 1,
                                   ctrl->GetTextLength(), m_search_word, flag);
		ctrl->SetCurrentPos( pos_end_word );
    }
    ctrl->SearchAnchor();
    if ( pos_prev != wxNOT_FOUND ) {
        int pos_word;

        if ( direction == SEARCH_DIRECTION::BACKWARD ) {
            pos_word = ctrl->SearchPrev( flag, m_search_word );
			ctrl->GotoPos( pos_word );
        } else {
            pos_word = ctrl->SearchNext( flag, m_search_word );
            /*FIXME error searching next: we get the current*/
			ctrl->GotoPos( pos_word + 1 );
        }

		evidentiate_word( ctrl );
        found = true;
		
     } else {
        found = false;
     }
    return found;
}

void VimCommand::evidentiate_word( wxStyledTextCtrl* ctrl )
{
	long pos = ctrl->GetCurrentPos();
    long start = ctrl->WordStartPosition(pos, true);
    long end = ctrl->WordEndPosition(pos, true);
	ctrl->SetSelectionStart( start );
	ctrl->SetSelectionEnd( end );
}

/**
 * This function is used to check when a command is complete.
 * In this case it is assign a unque command_id, which will be used by the 
 * function Command_call().
 */
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
    case 'g':
        if ( this->mActionCmd == 'g') {
            command_id = COMMANDVI::gg;
            command_complete = true;
        }
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
	case '%':
		command_id = COMMANDVI::perc;
		command_complete = true;
		mRepeat = 1;
		break;
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

    /*===================== DELETE TEXT ===============*/

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
        break;
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
        command_complete = true;
        command_id = COMMANDVI::x;
        break;
    case 'd':                               /*~~~~~~~ d[...] ~~~~~~~~*/
        switch ( mActionCmd ) {
        case '\0' :
            command_complete = false;
            break;
        case 'w' :
            command_complete  = true;
            command_id = COMMANDVI::dw;
            this->list_copied_str.clear();
            break;
        case 'd':
            command_complete = true;
            command_id = COMMANDVI::dd;
            this->list_copied_str.clear();
            this->new_line_cpy = true;
            break;
        }
        break;
    case 'D':
        /*FIXME: the event ctrl+D event does not reach the editor*/
        if ( this->modifier_key == wxMOD_CONTROL ) {
            command_complete = true;
            command_id = COMMANDVI::ctrl_D;
            modifier_key = 0;
        } else {
            command_complete = true;
            command_id = COMMANDVI::D;
        }
        break;
    case 'U':
        /*FIXME: the event ctrl+U event does not reach the editor*/
        if ( this->modifier_key == wxMOD_CONTROL ) {
            command_complete = true;
            command_id = COMMANDVI::ctrl_U;
            modifier_key = 0;
        } 
        break;
    /*========================== COPY ==================================*/
    /*FIXME To be complete */
    case 'y':                            
        switch ( mActionCmd ) {
        case '\0' :
            command_complete = false;
            break;
        case 'w' :
            command_complete  = true;
            command_id = COMMANDVI::yw;
            this->list_copied_str.clear();
            break;
        case 'y':
            command_complete = true;
            command_id = COMMANDVI::yy;
            this->list_copied_str.clear();
            this->new_line_cpy = true;
            break;
        }
        break;
         /*================== SEARCH ====================*/
    case '#':
        command_complete = true;
        command_id = COMMANDVI::diesis;
        mRepeat = 1;
        break;

    case 'N':
        command_complete = true;
        command_id = COMMANDVI::N;
        mRepeat = 1;
        break;

    case 'n':
        command_complete = true;
        command_id = COMMANDVI::n;
        mRepeat = 1;
        break;

        /*We just open quick find*/
    case '/':
        command_complete = true;
        command_id = COMMANDVI::slesh;
        mRepeat = 1;
        break;
        
	/*================ Repeat , i.e '.' ============*/
	case '.':
		command_complete = true;
        this->repeat_cmd = true;
		command_id = COMMANDVI::repeat;
		break;

    /*=============== Paste ======================*/
    case 'p':
        command_complete = true;
        command_id = COMMANDVI::p;
        this->mRepeat = 1;
        break;
    case 'P':
        command_complete = true;
        command_id = COMMANDVI::P;
        this->mRepeat = 1;
        break;

    }

    return command_complete;
    
}

void VimCommand::set_current_modus( VIM_MODI modus) {
    current_modus = modus;
} 

void VimCommand::append_command( wxChar ch )
{
	tmpBuf.Append( ch );
}

bool VimCommand::repeat_last_cmd()
{
    return repeat_cmd;
}

void VimCommand::reset_repeat_last()
{
    repeat_cmd = false;
}

bool VimCommand::save_current_cmd()
{
    return save_cmd;
}


void VimCommand::issue_cmd( wxStyledTextCtrl *ctrl)
{
    if ( ctrl == NULL )
        return;
      
    for ( int i = 0; i < this->getNumRepeat(); ++i){
        if ( !this->Command_call( ctrl ) )
            return; /*If the num repeat is internally implemented do not repeat!*/
    }
}


void VimCommand::repeat_issue_cmd( wxStyledTextCtrl *ctrl, wxString buf)
{
    if ( ctrl == NULL )
        return;
    
    for ( int i = 0; i < this->getNumRepeat(); ++i){
        if ( !this->Command_call( ctrl ) )
            return;
    }

    if ( current_modus == VIM_MODI::INSERT_MODUS) {
        /*FIXME*/
        ctrl->AddText( buf );
    }

}

