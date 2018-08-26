#include "vimCommands.h"

#include "macros.h"
#include "imanager.h"
#include "globals.h"
#include "codelite_events.h"
#include "clMainFrameHelper.h"
/*EXPERIMENTAL*/
#include "wx/uiaction.h"

VimCommand::VimCommand(IManager* m_mgr)
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
    , m_message_ID(MESSAGES_VIM::NO_ERROR_VIM_MSG)
{
    m_ctrl = NULL; /*FIXME: check it*/
    this->m_mgr = m_mgr;
}

VimCommand::~VimCommand() {}

MESSAGES_VIM VimCommand::getError() { return m_message_ID; }

void VimCommand::set_current_word(wxString word) { m_searchWord = word; }
/**
 *This function is used to update the command
 *@return true if the command can be issue
 */

bool VimCommand::OnNewKeyDown(wxChar ch, int modifier)
{
    m_message_ID = MESSAGES_VIM::NO_ERROR_VIM_MSG;
    bool skip_event = false;
    this->m_modifierKey = modifier;

    switch(m_currentModus) {
    case VIM_MODI::INSERT_MODUS:
        insert_modus(ch);
        skip_event = true;
        break;

    // get_text_at_position( ctrl);
    // m_currentModus = VIM_MODI::NORMAL_MODUS;
    case VIM_MODI::NORMAL_MODUS:
    case VIM_MODI::REPLACING_MODUS:
        normal_modus(ch);
        skip_event = false;
        break;
    case VIM_MODI::SEARCH_MODUS:
    case VIM_MODI::COMMAND_MODUS:
        command_modus(ch);
        skip_event = false;
        break;
    case VIM_MODI::VISUAL_MODUS:
        visual_modus(ch);
        skip_event = false;
        break;
    default:
        break;
    }

    return skip_event;
}

/**
 * This function is used to deal with the Esc press event.
 * Mostly it terminates insert/replace mode and push back into
 * normal mode
 */
bool VimCommand::OnEscapeDown()
{

    m_currentCommandPart = COMMAND_PART::REPEAT_NUM;
    m_currentModus = VIM_MODI::NORMAL_MODUS;
    m_tmpbuf.Clear();
    return true;
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
    // m_message_ID = MESSAGES_VIM::NO_ERROR;
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

void VimCommand::completing_command(wxChar ch)
{

    const int shift_ashii_num = '0';

    switch(m_currentCommandPart) {

    case COMMAND_PART::REPEAT_NUM:

        if(((ch <= '9' && ch >= '0') && m_repeat != 0) || ((ch <= '9' && ch > '0') && m_repeat == 0)) {
            int tmp = (int)ch;
            m_repeat = m_repeat * 10 + tmp - shift_ashii_num;
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
            case '/':
            case '?':
                m_currentModus = VIM_MODI::SEARCH_MODUS;
                m_tmpbuf.Append(ch);
                break;
            default:
                m_currentCommandPart = COMMAND_PART::MOD_NUM;
                break;
            }
        }
        break;

    case COMMAND_PART::MOD_NUM:

        if(ch < '9' && ch > '0' && m_baseCommand != 'r' && m_baseCommand != 'f' && m_baseCommand != 'F' &&
           m_baseCommand != 't' && m_baseCommand != 'T') {
            m_actions = m_actions * 10 + (int)ch - shift_ashii_num;
        } else {
            m_actionCommand = ch;
        }
        break;
    default:
        break;
    }
}

/**
 * This function is used to deal with the key event when we are in the
 * normal modus.
 */
void VimCommand::normal_modus(wxChar ch)
{

    completing_command(ch);

    if(m_currentCommandPart == COMMAND_PART::REPLACING) {
        m_actionCommand = ch;
    }
}

void VimCommand::visual_modus(wxChar ch) { completing_command(ch); }

bool VimCommand::OnReturnDown(VimCommand::eAction& action)
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
            m_message_ID = MESSAGES_VIM::SAVED_VIM_MSG;
        } else if(m_tmpbuf == _(":q") || m_tmpbuf == _(":quit")) {
            action = kClose;
            skip_event = false;
            m_tmpbuf.Clear();
            ResetCommand();
            m_currentModus = VIM_MODI::NORMAL_MODUS;
            m_message_ID = MESSAGES_VIM::CLOSED_VIM_MSG;
        } else if(m_tmpbuf == _(":q!")) {
            action = kClose;
            skip_event = false;
            m_tmpbuf.Clear();
            ResetCommand();
            m_currentModus = VIM_MODI::NORMAL_MODUS;
            m_message_ID = MESSAGES_VIM::CLOSED_VIM_MSG;
        } else if(m_tmpbuf == _(":wq")) {
            action = kSaveAndClose;
            skip_event = false;
            m_tmpbuf.Clear();
            ResetCommand();
            m_currentModus = VIM_MODI::NORMAL_MODUS;
            m_message_ID = MESSAGES_VIM::SAVE_AND_CLOSE_VIM_MSG;
        } else if(m_tmpbuf[0].GetValue() == ':') {
            skip_event = false;
            parse_cmd_string();
            m_tmpbuf.Clear();
            m_currentModus = VIM_MODI::NORMAL_MODUS;
            ResetCommand();
        }
    } else if(m_currentModus == VIM_MODI::SEARCH_MODUS) {
        skip_event = false;
        parse_cmd_string();
        m_tmpbuf.Clear();
        ResetCommand();
        m_currentModus = VIM_MODI::NORMAL_MODUS;
    } else if (m_currentModus == VIM_MODI::NORMAL_MODUS){
        m_ctrl->LineDown();
        skip_event = false;
    }
    return skip_event;
}

wxString VimCommand::getSearchedWord() {
    return m_searchWord;
}

void VimCommand::parse_cmd_string()
{
    bool all_file = false;
    bool search_forward = false;
    bool search_backward = false;
    bool replace = false;
    size_t len_buf = m_tmpbuf.Length();

    wxString word_to_replace;
    m_searchWord.Clear();
    for(size_t i = 0; i < len_buf; ++i) {
        switch(m_tmpbuf[i].GetValue()) {
        case '%':
            if(i + 1 < len_buf && m_tmpbuf[i + 1].GetValue() == 's') {
                all_file = true;
            }
            break;
        case '/':
            replace = search_forward;
            search_forward = true;
            break;
        case '?':
            replace = search_backward;
            search_backward = true;
            break;
        default:
            if(search_forward || search_backward)
                m_searchWord.Append(m_tmpbuf[i]);
            else if(replace)
                word_to_replace.Append(m_tmpbuf[i]);
            break;
        }
    }

    if(search_forward && !replace) {
        m_message_ID = MESSAGES_VIM::SEARCHING_WORD;
        long pos = -1L;
        if(all_file) pos = 0L;
        search_word(SEARCH_DIRECTION::FORWARD, pos);
        //m_mgr->FindAndSelect(m_searchWord, m_searchWord, pos);
    } else if(search_backward && !replace) {
        m_message_ID = MESSAGES_VIM::SEARCHING_WORD;
        long pos = -1L;
        if(all_file) pos = 0L;
        search_word(SEARCH_DIRECTION::BACKWARD, pos);
        //m_mgr->FindAndSelect(m_searchWord, m_searchWord, pos)
    }
}

/**
 * function dealing with the command status (i.e. ": ... " )
 */
void VimCommand::command_modus(wxChar ch) { m_tmpbuf.Append(ch); }

/**
 * This function call on the controller of the actual editor the vim-command.
 * Here is the actual implementation of the binding.
 */
bool VimCommand::Command_call()
{

    if(m_currentModus == VIM_MODI::VISUAL_MODUS) {
        return Command_call_visual_mode();
    }

    wxUIActionSimulator sim;
    bool repeat_command = true;
    this->m_saveCommand = true;
    switch(m_commandID) {
    /*======= MOVEMENT ===========*/

    case COMMANDVI::j:
        m_ctrl->LineDown();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::k:
        m_ctrl->LineUp();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::h:
        m_ctrl->CharLeft();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::l:
        m_ctrl->CharRight();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::_0:
        m_ctrl->Home();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::_$:
        m_ctrl->LineEnd();
        m_ctrl->CharLeft();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::w:
        m_ctrl->WordRight();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::W: {
        bool single_word = is_space_following();
        m_ctrl->WordRight();
        if(!single_word) {
            bool next_is_space = is_space_following();
            while(!next_is_space) {
                m_ctrl->WordRight();
                next_is_space = is_space_following();
            }
            m_ctrl->WordRight();
        }
        /*FIME - is a sequence of white space rightly jumped?*/
        this->m_saveCommand = false;
        break;
    }
    case COMMANDVI::b:
        m_ctrl->WordLeft();
        this->m_saveCommand = false;
        break;
    /*FIXME*/
    case COMMANDVI::B: {
        m_ctrl->WordLeft();
        bool prev_is_space = is_space_preceding(false, true);
        while(!prev_is_space) {
            m_ctrl->WordLeft();
            prev_is_space = is_space_preceding(false, true);
        }
        this->m_saveCommand = false;
        break;
    }
    case COMMANDVI::e: {
        long pos = m_ctrl->GetCurrentPos();
        long end = m_ctrl->WordEndPosition(pos, false);
        if(pos >= end - 1) {
            m_ctrl->WordRight();
            long i = 1;
            pos = m_ctrl->GetCurrentPos();
            end = m_ctrl->WordEndPosition(pos, false);
            while(m_ctrl->GetCharAt(end + i) == ' ') {
                i++;
                end = m_ctrl->WordEndPosition(pos + i, false);
            }
        }
        m_ctrl->GotoPos(end - 1);
        this->m_saveCommand = false;
        break;
    }
    case COMMANDVI::E: {
        bool single_word = is_space_following();
        m_ctrl->WordRight();
        if(!single_word) {
            bool next_is_space = is_space_following();
            while(!next_is_space) {
                m_ctrl->WordRight();
                next_is_space = is_space_following();
            }
            // m_ctrl->WordRight();
        }
        /*FIME - is a sequence of white space rightly jumped?*/
        this->m_saveCommand = false;

        long pos = m_ctrl->GetCurrentPos();
        long end = m_ctrl->WordEndPosition(pos, false);
        if(pos == end - 1) {
            m_ctrl->WordRight();
            long i = 1;
            pos = m_ctrl->GetCurrentPos();
            end = m_ctrl->WordEndPosition(pos, false);
            while(m_ctrl->GetCharAt(end + i) == ' ') {
                i++;
                end = m_ctrl->WordEndPosition(pos + i, false);
            }
        }
        m_ctrl->GotoPos(end - 1);

        break;
    }
    case COMMANDVI::F: {
        long i = 1;
        long pos = m_ctrl->GetCurrentPos();
        bool eoline = false;
        char cur_char;
        while((cur_char = m_ctrl->GetCharAt(pos - i)) != m_actionCommand) {
            if(cur_char == '\n') {
                eoline = true;
                break;
            }
            ++i;
        }
        if(eoline != true) m_ctrl->GotoPos(pos - i);
    } break;
    case COMMANDVI::f: {
        long i = 1;
        long pos = m_ctrl->GetCurrentPos();
        bool eoline = false;
        char cur_char;
        while((cur_char = m_ctrl->GetCharAt(pos + i)) != m_actionCommand) {
            if(cur_char == '\n') {
                eoline = true;
                break;
            }
            ++i;
        }
        if(eoline != true) m_ctrl->GotoPos(pos + i);
    } break;
    case COMMANDVI::T: {
        long i = 1;
        long pos = m_ctrl->GetCurrentPos();
        bool eoline = false;
        char cur_char;
        while((cur_char = m_ctrl->GetCharAt(pos - i)) != m_actionCommand) {
            if(cur_char == '\n') {
                eoline = true;
                break;
            }
            ++i;
        }
        if(eoline != true) m_ctrl->GotoPos(pos - i + 1);
    } break;
    case COMMANDVI::t: {
        long i = 1;
        long pos = m_ctrl->GetCurrentPos();
        bool eoline = false;
        char cur_char;
        while((cur_char = m_ctrl->GetCharAt(pos + i)) != m_actionCommand) {
            if(cur_char == '\n') {
                eoline = true;
                break;
            }
            ++i;
        }
        if(eoline != true) m_ctrl->GotoPos(pos + i - 1);
    } break;
    case COMMANDVI::ctrl_D:
        m_ctrl->PageDown();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::ctrl_U:
        m_ctrl->PageUp();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::G: /*====== START G =======*/
        this->m_saveCommand = false;
        switch(m_repeat) {
        case 0:
            m_ctrl->DocumentEnd();
            break;
        case 1:
            m_ctrl->DocumentStart();
            break;
        default:
            m_ctrl->GotoLine(m_repeat - 1);
            break;
        }
        break;          /*~~~~~~~ END G ~~~~~~~~*/
    case COMMANDVI::gg: /*====== START G =======*/
        this->m_saveCommand = false;
        if(m_repeat == 0) {
            m_repeat = 1;
        }
        m_ctrl->GotoLine(m_repeat - 1);
        repeat_command = false;
        break;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*========= PUT IN INSERT MODE =============*/
    case COMMANDVI::i:
        this->m_tmpbuf.Clear();
        break;
    case COMMANDVI::I:
        this->m_tmpbuf.Clear();
        m_ctrl->Home();
        break;
    case COMMANDVI::a:
        this->m_tmpbuf.Clear();
        m_ctrl->CharRight();
        break;
    case COMMANDVI::A:
        this->m_tmpbuf.Clear();
        m_ctrl->LineEnd();
        break;
    case COMMANDVI::o:
        this->m_tmpbuf.Clear();
        m_ctrl->LineEnd();
        m_ctrl->NewLine();
        break;
    case COMMANDVI::O:
        this->m_tmpbuf.Clear();
        m_ctrl->LineUp();
        m_ctrl->LineEnd();
        m_ctrl->NewLine();
        break;

    /*=============== VISUAL MODE ===============*/
    case COMMANDVI::v:
        m_currentModus = VIM_MODI::VISUAL_MODUS;
        this->m_tmpbuf.Clear();
        break;

    case COMMANDVI::perc: {
        long pos_matching = goToMatchingParentesis(m_ctrl->GetCurrentPos());
        if(pos_matching != -1)
            m_ctrl->GotoPos(pos_matching);
        else
            m_message_ID = MESSAGES_VIM::UNBALNCED_PARENTESIS_VIM_MSG;

        // FIXME:
        // sim.Char(']', wxMOD_SHIFT);
        // wxYield();
    } break;
    /*========= UNDO =============*/
    /* FIXME: the undo works strange with 'r' command*/
    case COMMANDVI::u:
        m_ctrl->Undo();
        break;

    /*======== REPLACE ===========*/
    case COMMANDVI::r:
        this->m_tmpbuf.Clear();
        m_ctrl->CharRight();
        m_ctrl->DeleteBackNotLine();
        m_ctrl->AddText(m_actionCommand);
        break;
    case COMMANDVI::R:
        this->m_tmpbuf.Clear();
        m_ctrl->CharRight();
        m_ctrl->DeleteBackNotLine();
        m_ctrl->AddText(m_actionCommand);
        repeat_command = false;
        break;
    case COMMANDVI::cw:
        this->m_tmpbuf.Clear();
        for( int i = 0; i < m_actions; ++i )
            m_ctrl->DelWordRight();
        break;
    case COMMANDVI::cb:
        this->m_tmpbuf.Clear();
        for( int i = 0; i < m_actions; ++i )
            m_ctrl->DelWordLeft();
        break;
    case COMMANDVI::ce:
        this->m_tmpbuf.Clear();
        for( int i = 0; i < m_actions; ++i )
            m_ctrl->DelWordRightEnd();
        break;

    case COMMANDVI::S: {
        this->m_tmpbuf.Clear();
        int repeat_S = std::max(1, m_repeat);
        for(int i = 0; i < repeat_S; ++i) {
            m_ctrl->LineDelete();
        }
        m_ctrl->NewLine();
        m_ctrl->LineUp();
        break;
    }
    case COMMANDVI::C:
        m_ctrl->DelLineRight();
        if(m_repeat > 1) {
            m_ctrl->LineDown();
            for(int i = 0; i < m_repeat - 1; ++i) { // delete extra line if [num]C
                m_ctrl->LineDelete();
            }
            m_ctrl->LineUp();
            m_ctrl->LineEnd();
        }
        repeat_command = false;
        break;

    case COMMANDVI::cc: {
        int repeat_cc = std::max(1, m_repeat) * std::max(1, m_actions);

        for(int i = 0; i < repeat_cc; ++i) {
            m_ctrl->LineDelete();
        }
        // m_ctrl->NewLine();
        repeat_command = false;
    } break;

    case COMMANDVI::x:
        this->m_tmpbuf.Clear();
        m_ctrl->CharRight();
        m_ctrl->DeleteBackNotLine();
        break;
    case COMMANDVI::X:
        this->m_tmpbuf.Clear();
        m_ctrl->CharRight();
        m_ctrl->CharRight();
        m_ctrl->DeleteBackNotLine();
        break;

    case COMMANDVI::dw: {
        int repeat_dw = std::max(1, m_actions);
        for(int i = 0; i < repeat_dw; ++i) {
            m_listCopiedStr.push_back(get_text_at_position(kFromPosToEndWord));
            if(is_space_following()) m_listCopiedStr.push_back(add_following_spaces());
            m_newLineCopy = false;
            m_ctrl->DelWordRight();
        }
    } break;
    case COMMANDVI::db: {
        int pos_init_db = m_ctrl->GetCurrentPos();
        int repeat_db = std::max(1, m_repeat) * std::max(1, m_actions);
        for(int i = 0; i < repeat_db; ++i) {
            m_ctrl->WordLeft();
        }
        int pos_end_db = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_db, pos_end_db));
        m_ctrl->SetCurrentPos(pos_init_db);
        for(int i = 0; i < repeat_db; ++i) {
            m_ctrl->DelWordLeft();
        }
        repeat_command = false;
        m_newLineCopy = false;
    } break;
    case COMMANDVI::de: {
        int repeat_de = std::max(1, m_actions);
        for(int i = 0; i < repeat_de - 1; ++i) {
            m_listCopiedStr.push_back(get_text_at_position(kFromPosToEndWord));
            if(is_space_following()) m_listCopiedStr.push_back(add_following_spaces());
            m_newLineCopy = false;
            m_ctrl->DelWordRight();
        } // last only the end of the word!
        m_listCopiedStr.push_back(get_text_at_position(kFromPosToEndWord));
        m_newLineCopy = false;
        m_ctrl->DelWordRightEnd();
    } break;
    case COMMANDVI::dd: {
        int repeat_dd = std::max(1, m_actions);
        int linePos;
        for(int i = 0; i < repeat_dd; ++i) {
            this->m_listCopiedStr.push_back(m_ctrl->GetCurLine(&linePos));
            m_ctrl->LineDelete();
        }
    } break;

    case COMMANDVI::D:
        this->m_listCopiedStr.push_back(get_text_at_position(kFromPositionToEndLine));
        m_ctrl->DelLineRight();
        break;
    /*=============== COPY ====================*/
    case COMMANDVI::yw: {
        int pos_init_yw = m_ctrl->GetCurrentPos();
        int repeat_yw = std::max(1, m_repeat) * std::max(1, m_actions);
        for(int i = 0; i < repeat_yw; ++i) {
            m_ctrl->WordRight();
        }
        int pos_end_yw = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_yw, pos_end_yw));
        repeat_command = false;
        m_ctrl->SetCurrentPos(pos_init_yw);
        m_ctrl->CharLeft();
        m_ctrl->CharRight();
        m_newLineCopy = false;
    } break;
    case COMMANDVI::yb: {
        int pos_init_yb = m_ctrl->GetCurrentPos();
        int repeat_yb = std::max(1, m_repeat) * std::max(1, m_actions);
        for(int i = 0; i < repeat_yb; ++i) {
            m_ctrl->WordLeft();
        }
        int pos_end_yb = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_yb, pos_end_yb));
        repeat_command = false;
        m_ctrl->SetCurrentPos(pos_init_yb);
        m_ctrl->CharLeft();
        m_ctrl->CharRight();
        m_newLineCopy = false;
    } break;
    case COMMANDVI::ye: {
        int pos_init_ye = m_ctrl->GetCurrentPos();
        int repeat_ye = std::max(1, m_repeat) * std::max(1, m_actions);
        for(int i = 0; i < repeat_ye; ++i) {
            m_ctrl->WordLeftEnd();
        }
        int pos_end_ye = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_ye, pos_end_ye));
        repeat_command = false;
        m_ctrl->SetCurrentPos(pos_init_ye);
        m_ctrl->CharLeft();
        m_ctrl->CharRight();
        m_newLineCopy = false;
    } break;
    case COMMANDVI::yy: {
        int position_init_yy = m_ctrl->GetCurrentPos();
        int linePos;
        int repeat_yy = std::max(1, m_repeat) * std::max(1, m_actions);
        for(int i = 0; i < repeat_yy; ++i) {
            this->m_listCopiedStr.push_back(m_ctrl->GetCurLine(&linePos));
            m_ctrl->LineDown();
        }
        repeat_command = false;
        m_ctrl->SetCurrentPos(position_init_yy);
        m_ctrl->CharLeft();
        m_ctrl->CharRight();
    } break;

    case COMMANDVI::diesis: {
        m_searchWord = get_text_at_position();
        // m_ctrl->SetCurrentPos( /*FIXME*/ );
        search_word(SEARCH_DIRECTION::BACKWARD);
        m_message_ID = MESSAGES_VIM::SEARCHING_WORD;
    } break;
    /*
    case COMMANDVI::slesh:
        //sim.Char('F', wxMOD_CONTROL);
        m_currentModus = VIM_MODI::SEARCH_MODUS;
        //wxYield();
        this->m_saveCommand = false;
        break;
    */
    case COMMANDVI::N:
        search_word(SEARCH_DIRECTION::BACKWARD);
        m_message_ID = MESSAGES_VIM::SEARCHING_WORD;
        this->m_saveCommand = false;
        break;

    case COMMANDVI::n:
        search_word(SEARCH_DIRECTION::FORWARD);
        this->m_saveCommand = false;
        m_message_ID = MESSAGES_VIM::SEARCHING_WORD;
        break;

    /*=============================== PASTE =========================*/
    case COMMANDVI::p: /*FIXME CharLeft goes the previous line if at position 0!*/
        this->m_saveCommand = false;
        if(this->m_newLineCopy) {
            m_ctrl->LineEnd();
            m_ctrl->NewLine();
        }
        for(std::vector<wxString>::iterator yanked = this->m_listCopiedStr.begin();
            yanked != this->m_listCopiedStr.end(); ++yanked) {
            m_ctrl->AddText(*yanked);
        }
        // FIXME: troppo contorto!
        if(this->m_newLineCopy) m_ctrl->LineDelete();
        break;
    case COMMANDVI::P:
        this->m_saveCommand = false;
        if(this->m_newLineCopy) {
            m_ctrl->LineUp();
            m_ctrl->LineEnd();
            m_ctrl->NewLine();
        } else {
            m_ctrl->CharLeft(); /*Is one char before andd the the same as p!*/
        }
        for(std::vector<wxString>::iterator yanked = this->m_listCopiedStr.begin();
            yanked != this->m_listCopiedStr.end(); ++yanked) {
            m_ctrl->AddText(*yanked);
        }
        // FIXME: troppo contorto!
        if(this->m_newLineCopy) m_ctrl->LineDelete();
        break;
    case COMMANDVI::repeat:
        // RepeatCommand( m_ctrl );
        this->m_saveCommand = false;
        this->m_repeatCommand = true;
        break;

        {
        case COMMANDVI::J:
            int curr_pos = m_ctrl->GetCurrentPos();
            m_ctrl->LineDown();
            m_ctrl->Home();
            m_ctrl->DeleteBack();
            m_ctrl->SetCurrentPos(curr_pos);
            /*FIXME: Workaround to avoid selection between previous
              end of line and current position*/
            m_ctrl->CharLeft();
            m_ctrl->CharRight();
            break;
        }
    default:
        repeat_command = false;
        break;
    }

    return repeat_command;
}

bool VimCommand::Command_call_visual_mode()
{

    wxUIActionSimulator sim;
    bool repeat_command = true;
    this->m_saveCommand = true;
    switch(m_commandID) {
    /*======= MOVEMENT ===========*/

    case COMMANDVI::j:
        m_ctrl->LineDownExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::k:
        m_ctrl->LineUpExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::h:
        m_ctrl->CharLeftExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::l:
        m_ctrl->CharRightExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::_0:
        m_ctrl->HomeExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::_$:
        m_ctrl->LineEndExtend();
        this->m_saveCommand = false;
        break;
    case COMMANDVI::w:
        m_ctrl->WordRightExtend();
        this->m_saveCommand = false;
        break;
    /*CHECK-ME*/
    case COMMANDVI::W: {
        bool single_word = is_space_following();
        m_ctrl->WordRightExtend();
        if(!single_word) {
            bool next_is_space = is_space_following();
            while(!next_is_space) {
                m_ctrl->WordRightExtend();
                next_is_space = is_space_following();
            }
            m_ctrl->WordRightExtend();
        }
        /*FIME - is a sequence of white space rightly jumped?*/
        this->m_saveCommand = false;
        break;
    }
    case COMMANDVI::b:
        m_ctrl->WordLeftExtend();
        this->m_saveCommand = false;
        break;
    /*FIXME*/
    case COMMANDVI::B: {
        m_ctrl->WordLeft();
        bool prev_is_space = is_space_preceding(false, true);
        while(!prev_is_space) {
            m_ctrl->WordLeft();
            prev_is_space = is_space_preceding(false, true);
        }
        this->m_saveCommand = false;
        break;
    }
    case COMMANDVI::e: {
        long pos = m_ctrl->GetCurrentPos();
        long end = m_ctrl->WordEndPosition(pos, false);
        if(pos == end) {
            m_ctrl->WordRight();
            pos = m_ctrl->GetCurrentPos();
            end = m_ctrl->WordEndPosition(pos, false);
        }
        m_ctrl->SetCurrentPos(end);
        break;
    }
    case COMMANDVI::E: {

        bool single_word = is_space_following();
        m_ctrl->WordRight();
        if(!single_word) {
            bool next_is_space = is_space_following();
            while(!next_is_space) {
                m_ctrl->WordRight();
                next_is_space = is_space_following();
            }
            //_ctrl->WordRight();
        }
        /*FIME - is a sequence of white space rightly jumped?*/
        this->m_saveCommand = false;

        long pos = m_ctrl->GetCurrentPos();
        long end = m_ctrl->WordEndPosition(pos, false);
        if(pos == end - 1) {
            m_ctrl->WordRight();
            long i = 1;
            pos = m_ctrl->GetCurrentPos();
            end = m_ctrl->WordEndPosition(pos, false);
            while(m_ctrl->GetCharAt(end + i) == ' ') {
                i++;
                end = m_ctrl->WordEndPosition(pos + i, false);
            }
        }
        m_ctrl->GotoPos(end - 1);

        break;
    }
    case COMMANDVI::F: {
        long i = 1;
        long pos = m_ctrl->GetCurrentPos();
        bool eoline = false;
        char cur_char;
        while((cur_char = m_ctrl->GetCharAt(pos - i)) != m_actionCommand) {
            if(cur_char == '\n') {
                eoline = true;
                break;
            }
            ++i;
        }
        if(eoline != true) m_ctrl->SetCurrentPos(pos - i);
    } break;
    case COMMANDVI::f: {
        long i = 1;
        long pos = m_ctrl->GetCurrentPos();
        bool eoline = false;
        char cur_char;
        while((cur_char = m_ctrl->GetCharAt(pos + i)) != m_actionCommand) {
            if(cur_char == '\n') {
                eoline = true;
                break;
            }
            ++i;
        }
        if(eoline != true) m_ctrl->SetCurrentPos(pos + i);
    } break;

    case COMMANDVI::T: {
        long i = 1;
        long pos = m_ctrl->GetCurrentPos();
        bool eoline = false;
        char cur_char;
        while((cur_char = m_ctrl->GetCharAt(pos - i)) != m_actionCommand) {
            if(cur_char == '\n') {
                eoline = true;
                break;
            }
            ++i;
        }
        if(eoline != true) m_ctrl->SetCurrentPos(pos - i + 1);
    } break;
    case COMMANDVI::t: {
        long i = 1;
        long pos = m_ctrl->GetCurrentPos();
        bool eoline = false;
        char cur_char;
        while((cur_char = m_ctrl->GetCharAt(pos + i)) != m_actionCommand) {
            if(cur_char == '\n') {
                eoline = true;
                break;
            }
            ++i;
        }
        if(eoline != true) m_ctrl->SetCurrentPos(pos + i - 1);
    } break;

    case COMMANDVI::G: /*====== START G =======*/
    {
        /*FIXME extend section*/
        this->m_saveCommand = false;
        switch(m_repeat) {
        case 0:
            m_ctrl->DocumentEndExtend();
            break;
        case 1:
            m_ctrl->DocumentStartExtend();
            break;
        default:
            // m_ctrl->SetSelectionStart( m_ctrl->GetCurrentPos() );
            // m_ctrl->GotoLine(m_repeat - 1);
            // m_ctrl->SetSelectionEnd( m_ctrl->GetCurrentPos() );
            break;
        }
    } break;            /*~~~~~~~ END G ~~~~~~~~*/
    case COMMANDVI::gg: /*====== START G =======*/
    {
        /*// FIXME extend section*/
        //     this->m_saveCommand = false;
        //     if(m_repeat == 0) {
        //         m_repeat = 1;
        //     }
        //     //m_ctrl->SetSelectionStart( m_ctrl->GetCurrentPos() );
        //     m_ctrl->GotoLine(m_repeat - 1);
        //     m_ctrl->SetSelectionEnd( m_ctrl->GetCurrentPos() );
        //     repeat_command = false;
    } break;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*========== DELETE AND COPY =======================*/

    case COMMANDVI::d:
        this->m_listCopiedStr.push_back(m_ctrl->GetSelectedText());
        m_ctrl->DeleteBack();        /*? better use Clear()*/
        this->m_saveCommand = false; /*FIXME: check what is vim-behaviour*/
        m_currentModus = VIM_MODI::NORMAL_MODUS;
        break;

    case COMMANDVI::y:
        this->m_listCopiedStr.push_back(m_ctrl->GetSelectedText());
        this->m_saveCommand = false; /*FIXME: check what is vim-behaviour*/
        m_currentModus = VIM_MODI::NORMAL_MODUS;
        break;
    default:
        break;
    }

    return repeat_command;
}

wxString VimCommand::get_text_at_position(VimCommand::eTypeTextSearch typeTextToSearch)
{

    long pos, start, end;

    pos = m_ctrl->GetCurrentPos();

    switch(typeTextToSearch) {
    case kAllWord:
        start = m_ctrl->WordStartPosition(pos, true);
        end = m_ctrl->WordEndPosition(pos, true);
        break;
    case kFromPosToEndWord:
        start = pos;
        end = m_ctrl->WordEndPosition(pos, true);
        if(start == end) {
            end++;
        }
        break;
    case kFromPosToBeginWord:
        end = pos;
        start = m_ctrl->WordStartPosition(pos, true);
        if(start == end) {
            start--;
        }
        break;
    case kFromPositionToEndLine:
        start = pos;
        end = m_ctrl->GetLineEndPosition(m_ctrl->GetCurrentLine());
        break;
    case kFromPositionToBeginLine:
        end = pos;
        start = m_ctrl->PositionFromLine(m_ctrl->GetCurrentLine());
        break;
    }

    return m_ctrl->GetTextRange(start, end);
}

bool VimCommand::is_space_following()
{
    long pos = m_ctrl->GetCurrentPos();
    if(m_ctrl->GetCharAt(pos + 1) == ' ') return true;
    long end = m_ctrl->WordEndPosition(pos, true);
    if(m_ctrl->GetCharAt(end) == ' ') return true;

    return false;
}

/*FIXME start is right pos?
 * @return true is a space preciding or we reached the start of the file*/
bool VimCommand::is_space_preceding(bool onlyWordChar, bool cross_line)
{
    long pos = m_ctrl->GetCurrentPos();
    if(pos == 0) return true;
    long start = m_ctrl->WordStartPosition(pos, onlyWordChar);
    if(m_ctrl->GetCharAt(start) == ' ') return true;
    if(cross_line && m_ctrl->GetCharAt(start) == '\n') return true;

    return false;
}

wxString VimCommand::add_following_spaces()
{
    wxString white_spaces_buf;
    long pos = m_ctrl->GetCurrentPos();
    long end = m_ctrl->WordEndPosition(pos, true);
    while(m_ctrl->GetCharAt(end) == ' ') {
        white_spaces_buf.Append(' ');
        end++;
    }

    return white_spaces_buf;
}

/*FIXME start is right pos*/
wxString VimCommand::add_preceding_spaces()
{
    wxString white_spaces_buf;
    long pos = m_ctrl->GetCurrentPos();
    long start = m_ctrl->WordStartPosition(pos, true);
    while(m_ctrl->GetCharAt(start) == ' ') {
        white_spaces_buf.Append(' ');
        start--;
    }

    return white_spaces_buf;
}

bool VimCommand::search_word(SEARCH_DIRECTION direction, long start_pos)
{

    // /*flag 2: word separated, Big pr small!*/
    // /*flag 3: same as before*/
    // /*flag 1-0: from the find other*/
    long pos;
    if(start_pos == -1) {
        pos = m_ctrl->GetCurrentPos();
    } else {
        pos = start_pos;
    }
    m_mgr->GetStatusBar()->SetMessage("Searching:" + m_searchWord);
    bool found = false;
    int flag = 0;
    int pos_prev;
    if(direction == SEARCH_DIRECTION::BACKWARD) {
        pos_prev = m_ctrl->FindText(0, pos, m_searchWord, flag);
    } else {
        pos_prev = m_ctrl->FindText(pos, m_ctrl->GetTextLength(), m_searchWord, flag);
        m_ctrl->SetCurrentPos(pos);
    }
    m_ctrl->SearchAnchor();
    if(pos_prev != wxNOT_FOUND) {
        int pos_word;

        if(direction == SEARCH_DIRECTION::BACKWARD) {
            pos_word = m_ctrl->SearchPrev(flag, m_searchWord);
            m_ctrl->GotoPos(pos_word);
        } else {
            pos_word = m_ctrl->SearchNext(flag, m_searchWord);
            /*FIXME error searching next: we get the current*/
            m_ctrl->GotoPos(pos_word + 1);
        }

        evidentiate_word();
        found = true;

    } else {
        found = false;
    }
    return found;
}

bool VimCommand::search_word(SEARCH_DIRECTION direction)
{

    // /*flag 2: word separated, Big pr small!*/
    // /*flag 3: same as before*/
    // /*flag 1-0: from the find other*/
    long pos = m_ctrl->GetCurrentPos();

    bool found = false;
    int flag = 0;
    int pos_prev;
    if(direction == SEARCH_DIRECTION::BACKWARD) {
        pos_prev = m_ctrl->FindText(0, pos, m_searchWord, flag);
    } else {
        m_ctrl->CharRight();
        int pos_end_word = m_ctrl->WordEndPosition(pos, true);
        pos_prev = m_ctrl->FindText(pos_end_word + 1, m_ctrl->GetTextLength(), m_searchWord, flag);
        m_ctrl->SetCurrentPos(pos_end_word);
    }
    m_ctrl->SearchAnchor();
    if(pos_prev != wxNOT_FOUND) {
        int pos_word;

        if(direction == SEARCH_DIRECTION::BACKWARD) {
            pos_word = m_ctrl->SearchPrev(flag, m_searchWord);
            m_ctrl->GotoPos(pos_word);
        } else {
            pos_word = m_ctrl->SearchNext(flag, m_searchWord);
            /*FIXME error searching next: we get the current*/
            m_ctrl->GotoPos(pos_word + 1);
        }

        evidentiate_word();
        found = true;

    } else {
        found = false;
    }
    return found;
}

void VimCommand::evidentiate_word()
{
    long pos = m_ctrl->GetCurrentPos();
    long start = m_ctrl->WordStartPosition(pos, true);
    long end = m_ctrl->WordEndPosition(pos, true);
    m_ctrl->SetSelectionStart(start);
    m_ctrl->SetSelectionEnd(end);
}

/**
 * This function is used to check when a command is complete.
 * In this case it is assign a unque command_id, which will be used by the
 * function Command_call().
 */
bool VimCommand::is_cmd_complete()
{

    bool command_complete = false;
    bool possible_command = false;
    switch(m_baseCommand) {

        /*========================== MOVEMENT =========================*/
        {
        case 'j':
            possible_command = true;
            m_commandID = COMMANDVI::j;
            command_complete = true;
            break;
        case 'h':
            possible_command = true;
            m_commandID = COMMANDVI::h;
            command_complete = true;
            break;
        case 'l':
            possible_command = true;
            m_commandID = COMMANDVI::l;
            command_complete = true;
            break;
        case 'k':
            possible_command = true;
            m_commandID = COMMANDVI::k;
            command_complete = true;
            break;

        case '$':
            possible_command = true;
            m_commandID = COMMANDVI::_$;
            command_complete = true;
            break;
        case '0':
            possible_command = true;
            m_commandID = COMMANDVI::_0;
            command_complete = true;
            break;
        case 'w':
            possible_command = true;
            m_commandID = COMMANDVI::w;
            command_complete = true;
            break;
        case 'W':
            possible_command = true;
            m_commandID = COMMANDVI::W;
            command_complete = true;
            break;
        case 'b':
            possible_command = true;
            m_commandID = COMMANDVI::b;
            command_complete = true;
            break;
        case 'B':
            possible_command = true;
            m_commandID = COMMANDVI::B;
            command_complete = true;
            break;
        case 'e':
            possible_command = true;
            m_commandID = COMMANDVI::e;
            command_complete = true;
            break;
        case 'E':
            possible_command = true;
            m_commandID = COMMANDVI::E;
            command_complete = true;
            break;
        case 'f':
            possible_command = true;
            m_commandID = COMMANDVI::f;
            if(this->m_actionCommand != '\0') {
                command_complete = true;
            }
            break;
        case 'F':
            possible_command = true;
            m_commandID = COMMANDVI::F;
            command_complete = false;
            if(this->m_actionCommand != '\0') {
                command_complete = true;
            }
            break;
        case 't':
            possible_command = true;
            m_commandID = COMMANDVI::t;
            if(this->m_actionCommand != '\0') {
                command_complete = true;
            }
            break;
        case 'T':
            possible_command = true;
            m_commandID = COMMANDVI::T;
            command_complete = false;
            if(this->m_actionCommand != '\0') {
                command_complete = true;
            }
            break;

        case 'G':
            possible_command = true;
            m_commandID = COMMANDVI::G;
            command_complete = true;
            break;
        case 'g':
            possible_command = true;
            if(this->m_actionCommand == 'g') {
                m_commandID = COMMANDVI::gg;
                command_complete = true;
            }
            break;
        }
        /*===================== CANGE INTO INSER ====================*/
        {
        case 'i':
            possible_command = true;
            m_commandID = COMMANDVI::i;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;
        case 'I':
            possible_command = true;
            m_commandID = COMMANDVI::I;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;

        case 'a':
            possible_command = true;
            m_commandID = COMMANDVI::a;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;
        case 'A':
            possible_command = true;
            m_commandID = COMMANDVI::A;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;

        case 'o':
            possible_command = true;
            m_commandID = COMMANDVI::o;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;
        case 'O':
            possible_command = true;
            m_commandID = COMMANDVI::O;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;
        }
    /*================== CHANGE INTO VISUAL MODE ================*/
    case 'v':
        possible_command = true;
        m_commandID = COMMANDVI::v;
        // m_currentModus = VIM_MODI::VISUAL_MODUS;
        command_complete = true;
        m_repeat = 1;
        break;
    case '%':
        possible_command = true;
        m_commandID = COMMANDVI::perc;
        command_complete = true;
        m_repeat = 1;
        break;
    /*===================== UNDO ====================*/
    case 'u':
        possible_command = true;
        m_commandID = COMMANDVI::u;
        command_complete = true;
        break;

    /*==================== REPLACE =================*/
    case 'r':
        possible_command = true;
        if(m_actionCommand == '\0') {
            command_complete = false;
        } else {
            command_complete = true;
            m_commandID = COMMANDVI::r;
        }
        break;

    case 'R':
        possible_command = true;
        if(m_actionCommand == '\0') {
            command_complete = false;
        } else {
            command_complete = true;
            m_commandID = COMMANDVI::R;
        }
        break;

    /*===================== DELETE TEXT ===============*/

    case 'c': /*~~~~~~ c[...] ~~~~~~~*/
        possible_command = true;
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
        case 'b':
            command_complete = true;
            m_commandID = COMMANDVI::cb;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;

        case 'e':
            command_complete = true;
            m_commandID = COMMANDVI::ce;
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        }
        break;
    case 'S':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::S;
        m_currentModus = VIM_MODI::INSERT_MODUS;
        break;

    case 'C':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::C;
        m_currentModus = VIM_MODI::INSERT_MODUS;
        break;
    case 'x':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::x;
        break;
    case 'X':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::X;
        break;
    case 'd': /*~~~~~~~ d[...] ~~~~~~~~*/
        possible_command = true;
        switch(m_actionCommand) {
        case '\0':
            if(m_currentModus == VIM_MODI::VISUAL_MODUS) {
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
        case 'e':
            command_complete = true;
            m_commandID = COMMANDVI::de;
            this->m_listCopiedStr.clear();
            break;
        }
        break;
    case 'D':
        possible_command = true;
        /*FIXME: the event m_ctrl+D event does not reach the editor*/
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
        possible_command = true;
        /*FIXME: the event m_ctrl+U event does not reach the editor*/
        if(this->m_modifierKey == wxMOD_CONTROL) {
            command_complete = true;
            m_commandID = COMMANDVI::ctrl_U;
            m_modifierKey = 0;
        }
        break;
    /*========================== COPY ==================================*/
    /*FIXME To be complete */
    case 'y':
        possible_command = true;
        switch(m_actionCommand) {
        case '\0':
            if(m_currentModus == VIM_MODI::VISUAL_MODUS) {
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
        case 'b':
            command_complete = true;
            m_commandID = COMMANDVI::yb;
            this->m_listCopiedStr.clear();
            break;
        case 'y':
            command_complete = true;
            m_commandID = COMMANDVI::yy;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            break;
        case 'e':
            command_complete = true;
            m_commandID = COMMANDVI::ye;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            break;
        }
        break;
    /*================== SEARCH ====================*/
    case '#':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::diesis;
        m_repeat = 1;
        break;

    case 'N':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::N;
        m_repeat = 1;
        break;

    case 'n':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::n;
        m_repeat = 1;
        break;

    /*We just open quick find*/
    case '/':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::slesh;
        m_repeat = 1;
        break;

    /*================ Repeat , i.e '.' ============*/
    case '.':
        possible_command = true;
        command_complete = true;
        this->m_repeatCommand = true;
        m_commandID = COMMANDVI::repeat;
        break;

    /*=============== Paste ======================*/
    case 'p':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::p;
        this->m_repeat = 1;
        break;
    case 'P':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::P;
        this->m_repeat = 1;
        break;
    case 'J':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::J;
        break;
    case '\0':
        possible_command = true;
    default:
        break;
    }

    if(!possible_command) {
        command_complete = true;
        m_commandID = COMMANDVI::NO_COMMAND;
    }

    return command_complete;
}

void VimCommand::set_current_modus(VIM_MODI modus) { m_currentModus = modus; }

void VimCommand::append_command(wxChar ch) { m_tmpbuf.Append(ch); }

bool VimCommand::repeat_last_cmd() { return m_repeatCommand; }

void VimCommand::reset_repeat_last() { m_repeatCommand = false; }

bool VimCommand::save_current_cmd() { return m_saveCommand; }

void VimCommand::IssueCommand()
{
    if(m_ctrl == NULL) return;
    m_ctrl->BeginUndoAction();
    for(int i = 0; i < this->getNumRepeat(); ++i) {
        if(!this->Command_call()) break; /*If the num repeat is internally implemented do not repeat!*/
    }
    m_ctrl->EndUndoAction();
}

void VimCommand::RepeatIssueCommand(wxString buf)
{
    if(m_ctrl == NULL) return;

    m_ctrl->BeginUndoAction();
    for(int i = 0; i < this->getNumRepeat(); ++i) {
        if(!this->Command_call()) break;
    }

    if(m_currentModus == VIM_MODI::INSERT_MODUS) {
        /*FIXME*/
        m_ctrl->AddText(buf);
    }
    m_ctrl->EndUndoAction();
}

void VimCommand::set_ctrl(wxStyledTextCtrl* ctrl) { m_ctrl = ctrl; }

bool VimCommand::DeleteLastCommandChar()
{
    if(m_currentModus == VIM_MODI::COMMAND_MODUS || m_currentModus == VIM_MODI::SEARCH_MODUS) {
        m_tmpbuf.RemoveLast();
        return true;
    }
    return false;
}

/**
 * @return the position of the matching parentesis
 */
long VimCommand::goToMatchingParentesis(long start_pos)
{
    const wxChar parentesis[] = {
        '(', ')', '[',  ']',  '{', '}',
        /*NOT VIM STANDARD, but useful*/
        '<', '>', '\"', '\"',
    };
    SEARCH_DIRECTION direction;
    long pos = start_pos;
    long max_n_char = m_ctrl->GetTextLength();
    wxChar currChar = m_ctrl->GetCharAt(pos);

    int index_parentesis;
    int increment = 0;
    bool found = false;

    for(index_parentesis = 0; index_parentesis < sizeof(parentesis); ++index_parentesis) {
        if(currChar == parentesis[index_parentesis]) {
            found = true;
            break;
        }
    }

    if(!found) return -1;

    increment = (index_parentesis % 2 == 0) ? +1 : -1;
    int indenting_level = 1;
    while(indenting_level > 0 && pos >= 0 && pos < max_n_char) {
        pos += increment;
        currChar = m_ctrl->GetCharAt(pos);
        if(currChar == parentesis[index_parentesis]) {
            ++indenting_level;
        } else if(currChar == parentesis[index_parentesis + increment]) {
            --indenting_level;
        }
    }

    return (indenting_level == 0) ? pos : -1;
}

/* ###############################################################
 #                      VIM BASE COMMAND                         #
 ################################################################# */

VimBaseCommand::VimBaseCommand(wxString fullpath_name)
    : m_fullpath_name(fullpath_name)
    , m_commandID(COMMANDVI::NO_COMMAND)
    , m_currentCommandPart(COMMAND_PART::REPEAT_NUM)
    , m_currentModus(VIM_MODI::NORMAL_MODUS)
    , m_saveCommand(true)
    , m_repeat(0)
    , m_baseCommand('\0')
    , m_actionCommand('\0')
    , m_actions(0)
    , m_repeatCommand(0)
    , m_modifierKey(0)

{
}

VimBaseCommand::VimBaseCommand(const VimBaseCommand& command)
    : m_fullpath_name(command.m_fullpath_name)
    , m_commandID(command.m_commandID)
    , m_currentCommandPart(command.m_currentCommandPart)
    , m_currentModus(command.m_currentModus)
    , m_saveCommand(command.m_saveCommand)
    , m_repeat(command.m_repeat)
    , m_baseCommand(command.m_baseCommand)
    , m_actionCommand(command.m_actionCommand)
    , m_actions(command.m_actions)
    , m_repeatCommand(command.m_repeatCommand)
    , m_modifierKey(command.m_modifierKey)
{
}

bool VimBaseCommand::isCurrentEditor(const wxString& fullpath_name) { return fullpath_name.IsSameAs(m_fullpath_name); }

void VimBaseCommand::saveCurrentStatus(const VimCommand& command)
{
    m_commandID = command.m_commandID;
    m_currentCommandPart = command.m_currentCommandPart;
    m_currentModus = command.m_currentModus;
    m_saveCommand = command.m_saveCommand;
    m_repeat = command.m_repeat;
    m_baseCommand = command.m_baseCommand;
    m_actionCommand = command.m_actionCommand;
    m_actions = command.m_actions;
    m_repeatCommand = command.m_repeatCommand;
    m_modifierKey = command.m_modifierKey;
}

void VimBaseCommand::setSavedStatus(VimCommand& command)
{
    command.m_commandID = m_commandID;
    command.m_currentCommandPart = m_currentCommandPart;
    command.m_currentModus = m_currentModus;
    command.m_saveCommand = m_saveCommand;
    command.m_repeat = m_repeat;
    command.m_baseCommand = m_baseCommand;
    command.m_actionCommand = m_actionCommand;
    command.m_actions = m_actions;
    command.m_repeatCommand = m_repeatCommand;
    command.m_modifierKey = m_modifierKey;
}
