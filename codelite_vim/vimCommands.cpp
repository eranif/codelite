#include "vimCommands.h"

#include "macros.h"
#include "imanager.h"
#include "globals.h"
#include "codelite_events.h"
#include "clMainFrameHelper.h"
/*EXPERIMENTAL*/
#include "wx/uiaction.h"

VimCommand::VimCommand()
    : m_currentCommandPart(COMMAND_PART::REPEAT_NUM)
    , m_currentModus(VIM_MODI::NORMAL_MODUS)
    , m_commandID(COMMANDVI::NO_COMMAND)
    , m_repeat(0)
    , m_baseCommand('\0')
    , m_actionCommand('\0')
    , m_actions(0)
    , m_listCopiedStr()
    , m_cumulativeUndo(0)
    , m_modifierKey(0)
    , m_tmpbuf()
    , m_searchWord()
    , m_newLineCopy(false)
    , m_repeatCommand(false)
    , m_saveCommand(true)
{
}

VimCommand::~VimCommand() {}

void VimCommand::set_current_word(wxString word) { m_searchWord = word; }
/**
 *This function is used to update the command
 *@return true if the command can be issue
 */

bool VimCommand::OnNewKeyDown(wxChar ch, int modifier)
{

    bool skip_event = false;
    this->m_modifierKey = modifier;

    switch(m_currentModus) {
    case VIM_MODI::INSERT_MODUS:
        insert_modus(ch);
        skip_event = true;
        break;
    case VIM_MODI::SEARCH_MODUS:
        // get_text_at_position( ctrl);
        m_currentModus = VIM_MODI::NORMAL_MODUS;
    case VIM_MODI::NORMAL_MODUS:
    case VIM_MODI::REPLACING_MODUS:
        normal_modus(ch);
        skip_event = false;
        break;
    case VIM_MODI::COMMAND_MODUS:
        command_modus(ch);
        skip_event = false;
        break;
    case VIM_MODI::VISUAL_MODUS:
       visual_modus( ch );
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
bool VimCommand::OnEscapeDown(wxStyledTextCtrl* ctrl)
{

    m_currentCommandPart = COMMAND_PART::REPEAT_NUM;
    m_currentModus = VIM_MODI::NORMAL_MODUS;
    m_tmpbuf.Clear();
}

/**
 * used to reset the command.
 */
void VimCommand::ResetCommand()
{
    m_currentCommandPart = COMMAND_PART::REPEAT_NUM;
    m_repeat = 0;
    m_baseCommand = '\0';
    m_actionCommand = '\0';
    m_actions = 0;
}

/**
 * get for the number of repetition a command has to be issued
 * Note: if the command is of <num>+'G' type, the repetition change
 * drasically the command, that's why there's a special behaviour.
 */
int VimCommand::getNumRepeat()
{
    if(m_baseCommand == 'G' || m_baseCommand == 'g') return 1;

    return ((m_repeat > 0) ? m_repeat : 1);
}

/**
 *return the modus (insert/normal/serach/visual ...) we are actually working with.
 * Use mostly to comunicate with VimManager and differentiate the handliing of
 * key event.
 */
VIM_MODI VimCommand::get_current_modus() { return m_currentModus; }

wxString VimCommand::getTmpBuf() { return m_tmpbuf; }

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
int VimCommand::getNumActions() { return m_actions; }

void VimCommand::insert_modus(wxChar ch) { m_tmpbuf.Append(ch); }
/**
 * This function is used to deal with the key event when we are in the
 * normal modus.
 */
void VimCommand::normal_modus(wxChar ch)
{

    const int shift_ashii_num = 48;

    switch(m_currentCommandPart) {

    case COMMAND_PART::REPEAT_NUM:

        if(((ch <= '9' && ch >= '0') && m_repeat != 0) || ((ch <= '9' && ch > '0') && m_repeat == 0)) {
            m_repeat = m_repeat * 10 + (int)ch - shift_ashii_num;
        } else {
            m_baseCommand = ch;
            switch(m_baseCommand) {
            case 'R':
                m_currentModus = VIM_MODI::REPLACING_MODUS;
                m_currentCommandPart = COMMAND_PART::REPLACING;
                break;
            case ':':
                m_currentModus = VIM_MODI::COMMAND_MODUS;
                m_tmpbuf.Append(ch);
                break;
            default:
                m_currentCommandPart = COMMAND_PART::MOD_NUM;
                break;
            }
        }
        break;

    case COMMAND_PART::MOD_NUM:

        if(ch < '9' && ch > '0' && m_baseCommand != 'r') {
            m_actions = m_actions * 10 + (int)ch - shift_ashii_num;
        } else {
            m_actionCommand = ch;
        }
        break;

    case COMMAND_PART::REPLACING:
        m_actionCommand = ch;
        break;
    }
}

void VimCommand::visual_modus(wxChar ch)
{

    const int shift_ashii_num = 48;

    switch(m_currentCommandPart) {

    case COMMAND_PART::REPEAT_NUM:

        if(((ch <= '9' && ch >= '0') && m_repeat != 0) || ((ch <= '9' && ch > '0') && m_repeat == 0)) {
            m_repeat = m_repeat * 10 + (int)ch - shift_ashii_num;
        } else {
            m_baseCommand = ch;
            switch(m_baseCommand) {
            case 'R':
                m_currentModus = VIM_MODI::REPLACING_MODUS;
                m_currentCommandPart = COMMAND_PART::REPLACING;
                break;
            case ':':
                m_currentModus = VIM_MODI::COMMAND_MODUS;
                m_tmpbuf.Append(ch);
                break;
            default:
                m_currentCommandPart = COMMAND_PART::MOD_NUM;
                break;
            }
        }
        break;

    case COMMAND_PART::MOD_NUM:

        if(ch < '9' && ch > '0' && m_baseCommand != 'r') {
            m_actions = m_actions * 10 + (int)ch - shift_ashii_num;
        } else {
            m_actionCommand = ch;
        }
        break;

    }
}


bool VimCommand::OnReturnDown(IEditor* editor, IManager* manager, VimCommand::eAction& action)
{
    bool skip_event = true;
    action = kNone;

    if(m_currentModus == VIM_MODI::COMMAND_MODUS) {
        if(m_tmpbuf == _(":w") || m_tmpbuf == _(":write")) {
            action = kSave;

            skip_event = false;
            m_tmpbuf.Clear();
            ResetCommand();
            m_currentModus = VIM_MODI::NORMAL_MODUS;
        } else if(m_tmpbuf == _(":q") || m_tmpbuf == _(":quit")) {
            action = kClose;
            skip_event = false;
            m_tmpbuf.Clear();
            ResetCommand();
        } else if(m_tmpbuf == _(":q!")) {
            action = kClose;
            skip_event = false;
            m_tmpbuf.Clear();
            ResetCommand();
        } else if(m_tmpbuf == _(":wq")) {
            action = kSaveAndClose;
            skip_event = false;
            m_tmpbuf.Clear();
            ResetCommand();
        }
    }

    return skip_event;
}

/**
 * function dealing with the command status (i.e. ": ... " )
 */
void VimCommand::command_modus(wxChar ch) { m_tmpbuf.Append(ch); }

/**
 * This function call on the controller of the actual editor the vim-command.
 * Here is the actual implementation of the binding.
 */
bool VimCommand::Command_call(wxStyledTextCtrl* ctrl)
{

    if ( m_currentModus == VIM_MODI::VISUAL_MODUS ) {
       return Command_call_visual_mode( ctrl );
    }
   
    wxUIActionSimulator sim;
    bool repeat_command = true;
    this->m_saveCommand = true;
    switch(m_commandID) {
    /*======= MOVEMENT ===========*/

    case COMMANDVI::j:
        ctrl->LineDown();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::k:
        ctrl->LineUp();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::h:
        ctrl->CharLeft();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::l:
        ctrl->CharRight();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::_0:
        ctrl->Home();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::_$:
        ctrl->LineEnd();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::w:
        ctrl->WordRight();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::b:
        ctrl->WordLeft();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::ctrl_D:
        ctrl->PageDown();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::ctrl_U:
        ctrl->PageUp();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::G: /*====== START G =======*/
        this->m_saveCommand = false;
        switch(m_repeat) {
        case 0:
            ctrl->DocumentEnd();
            break;
        case 1:
            ctrl->DocumentStart();
            break;
        default:
            ctrl->GotoLine(m_repeat - 1);
            break;
        }
        break;          /*~~~~~~~ END G ~~~~~~~~*/
    case COMMANDVI::gg: /*====== START G =======*/
        this->m_saveCommand = false;
        if(m_repeat == 0) {
            m_repeat = 1;
        }
        ctrl->GotoLine(m_repeat - 1);
        repeat_command = false;
        break;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*========= PUT IN INSERT MODE =============*/
    case COMMANDVI::i:
        this->m_tmpbuf.Clear();
        break;
    case COMMANDVI::I:
        this->m_tmpbuf.Clear();
        ctrl->Home();
        break;
    case COMMANDVI::a:
        this->m_tmpbuf.Clear();
        ctrl->CharRight();
        break;
    case COMMANDVI::A:
        this->m_tmpbuf.Clear();
        ctrl->LineEnd();
        break;
    case COMMANDVI::o:
        this->m_tmpbuf.Clear();
        ctrl->LineEnd();
        ctrl->NewLine();
        break;
    case COMMANDVI::O:
        this->m_tmpbuf.Clear();
        ctrl->LineUp();
        ctrl->LineEnd();
        ctrl->NewLine();
        break;

    /*=============== VISUAL MODE ===============*/
    case COMMANDVI::v:
        m_currentModus = VIM_MODI::VISUAL_MODUS;
        this->m_tmpbuf.Clear();
        break;
        
    case COMMANDVI::perc:
        /*FIXME*/
        // sim.Char(']', wxMOD_SHIFT);
        // wxYield();
        break;
    /*========= UNDO =============*/
    /* FIXME: the undo works strange with 'r' command*/
    case COMMANDVI::u:
        ctrl->Undo();
        break;

    /*======== REPLACE ===========*/
    case COMMANDVI::r:
        this->m_tmpbuf.Clear();
        ctrl->CharRight();
        ctrl->DeleteBackNotLine();
        ctrl->AddText(m_actionCommand);
        break;
    case COMMANDVI::R:
        this->m_tmpbuf.Clear();
        ctrl->CharRight();
        ctrl->DeleteBackNotLine();
        ctrl->AddText(m_actionCommand);
        repeat_command = false;
        break;
    case COMMANDVI::cw:
        this->m_tmpbuf.Clear();
        ctrl->DelWordRight();
        break;

    case COMMANDVI::S: {
        this->m_tmpbuf.Clear();
        int repeat_S = std::max(1, m_repeat);
        for(int i = 0; i < repeat_S; ++i) {
            ctrl->LineDelete();
        }
        ctrl->NewLine();
        ctrl->LineUp();
        break;
    }
    case COMMANDVI::C:
        ctrl->DelLineRight();
        if(m_repeat > 1) {
            ctrl->LineDown();
            for(int i = 0; i < m_repeat - 1; ++i) { // delete extra line if [num]C
                ctrl->LineDelete();
            }
            ctrl->LineUp();
            ctrl->LineEnd();
        }
        repeat_command = false;
        break;

    case COMMANDVI::cc: {
        int repeat_cc = std::max(1, m_repeat + m_actions);

        for(int i = 0; i < repeat_cc; ++i) {
            ctrl->LineDelete();
        }
        // ctrl->NewLine();
        repeat_command = false;
        break;
    }

    case COMMANDVI::x:
        this->m_tmpbuf.Clear();
        ctrl->CharRight();
        ctrl->DeleteBackNotLine();
        break;
    case COMMANDVI::dw:
        m_listCopiedStr.push_back(get_text_at_position(ctrl , VimCommand::eTypeTextSearch::kFromPosToEndWord) );
        if(is_space_following(ctrl)) m_listCopiedStr.push_back(' ');
        m_newLineCopy = false;
        ctrl->DelWordRight();
        break;
    break;
    case COMMANDVI::db:
        m_listCopiedStr.push_back(get_text_at_position(ctrl, VimCommand::eTypeTextSearch::kFromPosToBeginWord ));
        if(is_space_following(ctrl)) m_listCopiedStr.push_back(' ');
        m_newLineCopy = false;
        ctrl->DelWordLeft();
        break;
    break;
    case COMMANDVI::dd: {
        int linePos;
        this->m_listCopiedStr.push_back(ctrl->GetCurLine(&linePos));
        ctrl->LineDelete();
    } break;

    case COMMANDVI::D:
        this->m_listCopiedStr.push_back( get_text_at_position( ctrl, VimCommand::eTypeTextSearch::kFromPositionToEndLine) );
        ctrl->DelLineRight();
        break;
    /*=============== COPY ====================*/
    case COMMANDVI::yw:
        m_listCopiedStr.push_back(get_text_at_position(ctrl));
        if(is_space_following(ctrl)) m_listCopiedStr.push_back(' ');
        m_newLineCopy = false;
        break;
    case COMMANDVI::yy: {
        int linePos;
        this->m_listCopiedStr.push_back(ctrl->GetCurLine(&linePos));
    } break;

    case COMMANDVI::diesis: {
        m_searchWord = get_text_at_position(ctrl);
        // ctrl->SetCurrentPos( /*FIXME*/ );
        search_word(SEARCH_DIRECTION::BACKWARD, ctrl);
    } break;

    case COMMANDVI::slesh:
        sim.Char('F', wxMOD_CONTROL);
        m_currentModus = VIM_MODI::SEARCH_MODUS;
        wxYield();
        this->m_saveCommand = false;
        break;

    case COMMANDVI::N:
        search_word(SEARCH_DIRECTION::BACKWARD, ctrl);
        break;
        this->m_saveCommand = false;

    case COMMANDVI::n:
        search_word(SEARCH_DIRECTION::FORWARD, ctrl);
        this->m_saveCommand = false;
        break;

    /*=============================== PASTE =========================*/
    // FIXME: One line too many !!!
    case COMMANDVI::P:
        this->m_saveCommand = false;
        if(this->m_newLineCopy) {
            ctrl->LineEnd();
            ctrl->NewLine();
        } else {
            ctrl->CharLeft(); /*Is one char before andd the the same as p!*/
        }

        for(std::vector<wxString>::iterator yanked = this->m_listCopiedStr.begin();
            yanked != this->m_listCopiedStr.end();
            ++yanked) {
            ctrl->AddText(*yanked);
        }
        // FIXME: troppo contorto!
        if(this->m_newLineCopy) ctrl->LineDelete();
        break;
    case COMMANDVI::p:
        this->m_saveCommand = false;
        if(this->m_newLineCopy) {
            ctrl->LineUp();
            ctrl->LineEnd();
            ctrl->NewLine();
        }
        for(std::vector<wxString>::iterator yanked = this->m_listCopiedStr.begin();
            yanked != this->m_listCopiedStr.end();
            ++yanked) {
            ctrl->AddText(*yanked);
        }
        // FIXME: troppo contorto!
        if(this->m_newLineCopy) ctrl->LineDelete();
        break;
    case COMMANDVI::repeat:
        // repeat_cmd( ctrl );
        this->m_saveCommand = false;
        this->m_repeatCommand = true;
        break;

    {
    case COMMANDVI::J:
        int curr_pos = ctrl->GetCurrentPos();
        ctrl->LineDown();
        ctrl->Home();
        ctrl->DeleteBack();
        ctrl->SetCurrentPos( curr_pos );
        /*FIXME: Workaround to avoid selection between previous 
          end of line and current position*/
        ctrl->CharLeft();
        ctrl->CharRight();
        break;
    }

    }

    return repeat_command;
}

bool VimCommand::Command_call_visual_mode(wxStyledTextCtrl* ctrl)
{

    wxUIActionSimulator sim;
    bool repeat_command = true;
    this->m_saveCommand = true;
    switch(m_commandID) {
    /*======= MOVEMENT ===========*/

    case COMMANDVI::j:
        ctrl->LineDownExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::k:
        ctrl->LineUpExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::h:
        ctrl->CharLeftExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::l:
        ctrl->CharRightExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::_0:
        ctrl->HomeExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::_$:
        ctrl->LineEndExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::w:
        ctrl->WordRightExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::b:
        ctrl->WordLeftExtend();
        this->m_saveCommand = false;
        break;

    case COMMANDVI::G: /*====== START G =======*/
       {
          /*FIXME extend section*/
            // this->m_saveCommand = false;
        //     switch(m_repeat) {
        //     case 0:
        //         ctrl->DocumentEndExtend();
        //         break;
        //     case 1:
        //         ctrl->DocumentStartExtend();
        //         break;
        //     default:
        //        //ctrl->SetSelectionStart( ctrl->GetCurrentPos() );
        //         ctrl->GotoLine(m_repeat - 1);
        //         ctrl->SetSelectionEnd( ctrl->GetCurrentPos() );
        //         break;
        //     }
        }
        break;          /*~~~~~~~ END G ~~~~~~~~*/
    case COMMANDVI::gg: /*====== START G =======*/
        {
          /*// FIXME extend section*/
        //     this->m_saveCommand = false;
        //     if(m_repeat == 0) {
        //         m_repeat = 1;
        //     }
        //     //ctrl->SetSelectionStart( ctrl->GetCurrentPos() );
        //     ctrl->GotoLine(m_repeat - 1);
        //     ctrl->SetSelectionEnd( ctrl->GetCurrentPos() );
        //     repeat_command = false;
        }
        break;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        /*========== DELETE AND COPY =======================*/
        
    case COMMANDVI::d:
        this->m_listCopiedStr.push_back( ctrl->GetSelectedText() );
        ctrl->DeleteBack(); /*? better use Clear()*/
        this->m_saveCommand = false; /*FIXME: check what is vim-behaviour*/
        m_currentModus = VIM_MODI::NORMAL_MODUS;
        break;

    case COMMANDVI::y:
        this->m_listCopiedStr.push_back( ctrl->GetSelectedText() );
        this->m_saveCommand = false; /*FIXME: check what is vim-behaviour*/
        m_currentModus = VIM_MODI::NORMAL_MODUS;
        break;
    }

    return repeat_command;
}



wxString VimCommand::get_text_at_position(wxStyledTextCtrl* ctrl, VimCommand::eTypeTextSearch typeTextToSearch)
{

   long pos, start, end;
   
   pos = ctrl->GetCurrentPos();

   switch ( typeTextToSearch )
   {
   case kAllWord:
      start = ctrl->WordStartPosition(pos, true);
      end = ctrl->WordEndPosition(pos, true);
      break;
   case kFromPosToEndWord:
      start = pos;
      end = ctrl->WordEndPosition(pos, true);
      break;
   case kFromPosToBeginWord:
      end = pos;
      start = ctrl->WordStartPosition( pos, true );
      break;
   case kFromPositionToEndLine:
      start = pos;
      end = ctrl->GetLineEndPosition( ctrl->GetCurrentLine() );
      break;
   case kFromPositionToBeginLine:
      end = pos;
      start = ctrl->PositionFromLine( ctrl->GetCurrentLine() );
      break;
   }

   return ctrl->GetTextRange(start, end);

}

bool VimCommand::is_space_following(wxStyledTextCtrl* ctrl)
{
    long pos = ctrl->GetCurrentPos();
    long end = ctrl->WordEndPosition(pos, true);
    if(ctrl->GetCharAt(end) == ' ') return true;

    return false;
}

bool VimCommand::search_word(SEARCH_DIRECTION direction, wxStyledTextCtrl* ctrl)
{

    // /*flag 2: word separated, Big pr small!*/
    // /*flag 3: same as before*/
    // /*flag 1-0: from the find other*/
    long pos = ctrl->GetCurrentPos();
    bool found = false;
    int flag = 3;
    int pos_prev;
    if(direction == SEARCH_DIRECTION::BACKWARD) {
        pos_prev = ctrl->FindText(0, pos, m_searchWord, flag);
    } else {
        ctrl->CharRight();
        int pos_end_word = ctrl->WordEndPosition(pos, true);
        pos_prev = ctrl->FindText(pos_end_word + 1, ctrl->GetTextLength(), m_searchWord, flag);
        ctrl->SetCurrentPos(pos_end_word);
    }
    ctrl->SearchAnchor();
    if(pos_prev != wxNOT_FOUND) {
        int pos_word;

        if(direction == SEARCH_DIRECTION::BACKWARD) {
            pos_word = ctrl->SearchPrev(flag, m_searchWord);
            ctrl->GotoPos(pos_word);
        } else {
            pos_word = ctrl->SearchNext(flag, m_searchWord);
            /*FIXME error searching next: we get the current*/
            ctrl->GotoPos(pos_word + 1);
        }

        evidentiate_word(ctrl);
        found = true;

    } else {
        found = false;
    }
    return found;
}

void VimCommand::evidentiate_word(wxStyledTextCtrl* ctrl)
{
    long pos = ctrl->GetCurrentPos();
    long start = ctrl->WordStartPosition(pos, true);
    long end = ctrl->WordEndPosition(pos, true);
    ctrl->SetSelectionStart(start);
    ctrl->SetSelectionEnd(end);
}

/**
 * This function is used to check when a command is complete.
 * In this case it is assign a unque command_id, which will be used by the
 * function Command_call().
 */
bool VimCommand::is_cmd_complete()
{

    bool command_complete = false;

    switch(m_baseCommand) {

        /*========================== MOVEMENT =========================*/
        {
        case 'j':
            m_commandID = COMMANDVI::j;
            command_complete = true;
            break;
        case 'h':
            m_commandID = COMMANDVI::h;
            command_complete = true;
            break;
        case 'l':
            m_commandID = COMMANDVI::l;
            command_complete = true;
            break;
        case 'k':
            m_commandID = COMMANDVI::k;
            command_complete = true;
            break;

        case '$':
            m_commandID = COMMANDVI::_$;
            command_complete = true;
            break;
        case '0':
            m_commandID = COMMANDVI::_0;
            command_complete = true;
            break;
        case 'w':
            m_commandID = COMMANDVI::w;
            command_complete = true;
            break;
        case 'b':
            m_commandID = COMMANDVI::b;
            command_complete = true;
            break;
        case 'G':
            m_commandID = COMMANDVI::G;
            command_complete = true;
            break;
        case 'g':
            if(this->m_actionCommand == 'g') {
                m_commandID = COMMANDVI::gg;
                command_complete = true;
            }
            break;
        }
        /*===================== CANGE INTO INSER ====================*/
        {
        case 'i':
            m_commandID = COMMANDVI::i;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;
        case 'I':
            m_commandID = COMMANDVI::I;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;

        case 'a':
            m_commandID = COMMANDVI::a;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;
        case 'A':
            m_commandID = COMMANDVI::A;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;

        case 'o':
            m_commandID = COMMANDVI::o;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;
        case 'O':
            m_commandID = COMMANDVI::O;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;
        }
        /*================== CHANGE INTO VISUAL MODE ================*/
    case 'v':
        m_commandID = COMMANDVI::v;
        //m_currentModus = VIM_MODI::VISUAL_MODUS;
        command_complete = true;
        m_repeat = 1;
        break;
    case '%':
        m_commandID = COMMANDVI::perc;
        command_complete = true;
        m_repeat = 1;
        break;
    /*===================== UNDO ====================*/
    case 'u':
        m_commandID = COMMANDVI::u;
        command_complete = true;
        break;

    /*==================== REPLACE =================*/
    case 'r':
        if(m_actionCommand == '\0') {
            command_complete = false;
        } else {
            command_complete = true;
            m_commandID = COMMANDVI::r;
        }
        break;

    case 'R':
        if(m_actionCommand == '\0') {
            command_complete = false;
        } else {
            command_complete = true;
            m_commandID = COMMANDVI::R;
        }
        break;

    /*===================== DELETE TEXT ===============*/

    case 'c': /*~~~~~~ c[...] ~~~~~~~*/
        switch(m_actionCommand) {
        case '\0':
            command_complete = false;
            break;
        case 'w':
            command_complete = true;
            m_commandID = COMMANDVI::cw;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case 'c':
            command_complete = true;
            m_commandID = COMMANDVI::cc;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        }
        break;
    case 'S':
        command_complete = true;
        m_commandID = COMMANDVI::S;
        m_currentModus = VIM_MODI::INSERT_MODUS;
        break;

    case 'C':
        command_complete = true;
        m_commandID = COMMANDVI::C;
        m_currentModus = VIM_MODI::INSERT_MODUS;
        break;
    case 'x':
        command_complete = true;
        m_commandID = COMMANDVI::x;
        break;
    case 'd': /*~~~~~~~ d[...] ~~~~~~~~*/
        switch(m_actionCommand) {
        case '\0':
            if ( m_currentModus == VIM_MODI::VISUAL_MODUS ) {
                command_complete = true;
                m_commandID = COMMANDVI::d;
                this->m_listCopiedStr.clear();
            } else {
                command_complete = false;
            }
            break;
        case 'w':
            command_complete = true;
            m_commandID = COMMANDVI::dw;
            this->m_listCopiedStr.clear();
            break;
        case 'd':
            command_complete = true;
            m_commandID = COMMANDVI::dd;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            break;
        case 'b':
            command_complete = true;
            m_commandID = COMMANDVI::db;
            this->m_listCopiedStr.clear();
            break;
        }
        break;
    case 'D':
        /*FIXME: the event ctrl+D event does not reach the editor*/
        if(this->m_modifierKey == wxMOD_CONTROL) {
            command_complete = true;
            m_commandID = COMMANDVI::ctrl_D;
            m_modifierKey = 0;
        } else {
            command_complete = true;
            this->m_listCopiedStr.clear();
            m_commandID = COMMANDVI::D;
        }
        break;
    case 'U':
        /*FIXME: the event ctrl+U event does not reach the editor*/
        if(this->m_modifierKey == wxMOD_CONTROL) {
            command_complete = true;
            m_commandID = COMMANDVI::ctrl_U;
            m_modifierKey = 0;
        }
        break;
    /*========================== COPY ==================================*/
    /*FIXME To be complete */
    case 'y':
        switch(m_actionCommand) {
        case '\0':
            if ( m_currentModus == VIM_MODI::VISUAL_MODUS ) {
                command_complete = true;
                m_commandID = COMMANDVI::y;
                this->m_listCopiedStr.clear();
            } else {
                command_complete = false;
            }
            break;
        case 'w':
            command_complete = true;
            m_commandID = COMMANDVI::yw;
            this->m_listCopiedStr.clear();
            break;
        case 'y':
            command_complete = true;
            m_commandID = COMMANDVI::yy;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            break;
        }
        break;
    /*================== SEARCH ====================*/
    case '#':
        command_complete = true;
        m_commandID = COMMANDVI::diesis;
        m_repeat = 1;
        break;

    case 'N':
        command_complete = true;
        m_commandID = COMMANDVI::N;
        m_repeat = 1;
        break;

    case 'n':
        command_complete = true;
        m_commandID = COMMANDVI::n;
        m_repeat = 1;
        break;

    /*We just open quick find*/
    case '/':
        command_complete = true;
        m_commandID = COMMANDVI::slesh;
        m_repeat = 1;
        break;

    /*================ Repeat , i.e '.' ============*/
    case '.':
        command_complete = true;
        this->m_repeatCommand = true;
        m_commandID = COMMANDVI::repeat;
        break;

    /*=============== Paste ======================*/
    case 'p':
        command_complete = true;
        m_commandID = COMMANDVI::p;
        this->m_repeat = 1;
        break;
    case 'P':
        command_complete = true;
        m_commandID = COMMANDVI::P;
        this->m_repeat = 1;
        break;
    case 'J':
        command_complete = true;
        m_commandID = COMMANDVI::J;
        break;
    }

    return command_complete;
}

void VimCommand::set_current_modus(VIM_MODI modus) { m_currentModus = modus; }

void VimCommand::append_command(wxChar ch) { m_tmpbuf.Append(ch); }

bool VimCommand::repeat_last_cmd() { return m_repeatCommand; }

void VimCommand::reset_repeat_last() { m_repeatCommand = false; }

bool VimCommand::save_current_cmd() { return m_saveCommand; }

void VimCommand::issue_cmd(wxStyledTextCtrl* ctrl)
{
    if(ctrl == NULL) return;

    for(int i = 0; i < this->getNumRepeat(); ++i) {
        if(!this->Command_call(ctrl)) return; /*If the num repeat is internally implemented do not repeat!*/
    }
}

void VimCommand::repeat_issue_cmd(wxStyledTextCtrl* ctrl, wxString buf)
{
    if(ctrl == NULL) return;

    for(int i = 0; i < this->getNumRepeat(); ++i) {
        if(!this->Command_call(ctrl)) return;
    }

    if(m_currentModus == VIM_MODI::INSERT_MODUS) {
        /*FIXME*/
        ctrl->AddText(buf);
    }
}

void VimCommand::set_ctrl(wxStyledTextCtrl* ctrl) {}

bool VimCommand::DeleteLastCommandChar()
{
    if(m_currentModus == VIM_MODI::COMMAND_MODUS) {
        m_tmpbuf.RemoveLast();
        return true;
    }
    return false;
}
