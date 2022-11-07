#include "vimCommands.h"

#include "codelite_events.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"

#include <cmath>
/*EXPERIMENTAL*/
#include "wx/uiaction.h"

VimCommand::VimCommand(IManager* m_mgr)
    : m_currentCommandPart(COMMAND_PART::REPEAT_NUM)
    , m_currentModus(VIM_MODI::NORMAL_MODUS)
    , m_commandID(COMMANDVI::NO_COMMAND)
    , m_repeat(0)
    , m_baseCommand('\0')
    , m_actionCommand('\0')
    , m_externalCommand('\0')
    , m_actions(0)
    , m_listCopiedStr()
    , m_findKey('\0')
    , m_findStep(1)
    , m_findPosPrev(false)
    , m_cumulativeUndo(0)
    , m_modifierKey(0)
    , m_tmpbuf()
    , m_searchWord()
    , m_newLineCopy(false)
    , m_visualBlockCopy(false)
    , m_repeatCommand(false)
    , m_saveCommand(true)
    , m_message_ID(MESSAGES_VIM::NO_ERROR_VIM_MSG)
    , m_initialVisualPos(0)
    , m_visualBlockBeginLine(0)
    , m_visualBlockEndLine(0)
    , m_visualBlockBeginCol(0)
    , m_visualBlockEndCol(0)

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
    case VIM_MODI::VISUAL_LINE_MODUS:
    case VIM_MODI::VISUAL_BLOCK_MODUS:
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
    if(m_currentModus == VIM_MODI::INSERT_MODUS) {
        if(m_commandID == COMMANDVI::block_I || m_commandID == COMMANDVI::block_A ||
           m_commandID == COMMANDVI::block_c) {
            int begin_line = m_visualBlockBeginLine;
            int end_line = m_visualBlockEndLine;
            int begin_col = m_visualBlockBeginCol;
            int end_col = m_visualBlockEndCol;

            if(end_line < begin_line) {
                std::swap(end_line, begin_line);
            }
            if(begin_col > end_col) {
                std::swap(begin_col, end_col);
            }

            int col = begin_col;
            if(m_commandID == COMMANDVI::block_A) {
                col = end_col + 1;
            }
            int start_pos = m_ctrl->FindColumn(begin_line, col);
            if(m_ctrl->GetCurrentLine() == begin_line && m_ctrl->GetColumn(m_ctrl->GetCurrentPos()) > col) {

                int pos = m_ctrl->GetCurrentPos();
                wxString text = m_ctrl->GetTextRange(start_pos, pos);
                m_ctrl->DeleteRange(start_pos, pos - start_pos);

                m_ctrl->BeginUndoAction();
                m_ctrl->GotoPos(start_pos);

                for(int line = begin_line; line <= end_line && !text.empty();) {
                    pos = m_ctrl->GetCurrentPos();
                    m_ctrl->InsertText(pos, text);
                    m_ctrl->GotoPos(pos);
                    if(++line > end_line) {
                        break;
                    }
                    m_ctrl->LineDown();
                    int curCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
                    while(curCol > col) {
                        m_ctrl->CharLeft();
                        curCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
                    }
                    while(curCol < col) {
                        m_ctrl->AddText(" ");
                        curCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
                    }
                }
                m_ctrl->GotoPos(start_pos);
                m_ctrl->EndUndoAction();
            }
        }
        if(m_ctrl->GetColumn(m_ctrl->GetCurrentPos()) > 0) {
            m_ctrl->CharLeft();
        }
    }
    m_currentCommandPart = COMMAND_PART::REPEAT_NUM;
    m_currentModus = VIM_MODI::NORMAL_MODUS;
    m_tmpbuf.Clear();
    ResetCommand();
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
    m_externalCommand = '\0';
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
    if(m_baseCommand == 'G' || m_baseCommand == 'g')
        return 1;

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
           m_baseCommand != 't' && m_baseCommand != 'T' &&
           !((m_baseCommand == 'c' || m_baseCommand == 'd' || m_baseCommand == 'y') &&
             (m_externalCommand == 'f' || m_externalCommand == 'F' || m_externalCommand == 't' ||
              m_externalCommand == 'T'))) {
            m_actions = m_actions * 10 + (int)ch - shift_ashii_num;
        } else {
            m_externalCommand = m_actionCommand;
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
    if(m_currentCommandPart == COMMAND_PART::REPLACING) {
        m_actionCommand = ch;
    }
    completing_command(ch);
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
    } else if(m_currentModus == VIM_MODI::NORMAL_MODUS) {
        m_ctrl->LineDown();
        skip_event = false;
    }
    return skip_event;
}

wxString VimCommand::getSearchedWord() { return m_searchWord; }

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
        if(all_file)
            pos = 0L;
        search_word(SEARCH_DIRECTION::FORWARD, 0, pos);
    } else if(search_backward && !replace) {
        m_message_ID = MESSAGES_VIM::SEARCHING_WORD;
        long pos = -1L;
        if(all_file)
            pos = 0L;
        search_word(SEARCH_DIRECTION::BACKWARD, 0, pos);
    }
}

/**
 * function dealing with the command status (i.e. ": ... " )
 */
void VimCommand::command_modus(wxChar ch) { m_tmpbuf.Append(ch); }

bool VimCommand::command_move_cmd_call(bool& repeat_command)
{
    switch(m_commandID) {
        /*======= MOVEMENT ===========*/
    case COMMANDVI::j:
        for(int i = 0; i < std::max(1, m_repeat); ++i) {
            m_ctrl->LineDown();
        }
        this->m_saveCommand = false;
        repeat_command = false;
        break;
    case COMMANDVI::k:
        for(int i = 0; i < std::max(1, m_repeat); ++i) {
            m_ctrl->LineUp();
        }
        this->m_saveCommand = false;
        repeat_command = false;
        break;
    case COMMANDVI::h:
        for(int i = 0; i < std::max(1, m_repeat); ++i) {
            m_ctrl->CharLeft();
        }
        this->m_saveCommand = false;
        repeat_command = false;
        break;
    case COMMANDVI::l:
        for(int i = 0; i < std::max(1, m_repeat); ++i) {
            m_ctrl->CharRight();
        }
        this->m_saveCommand = false;
        repeat_command = false;
        break;
    case COMMANDVI::H: {
        int firstLine = m_ctrl->GetFirstVisibleLine();
        int curLine = m_ctrl->GetCurrentLine();
        for(; curLine > firstLine; --curLine) {
            m_ctrl->LineUp();
        }
        this->m_saveCommand = false;
        repeat_command = false;
    } break;
    case COMMANDVI::M: {
        int medLine = m_ctrl->GetFirstVisibleLine() + m_ctrl->LinesOnScreen() / 2;
        int curLine = m_ctrl->GetCurrentLine();
        if(curLine > medLine) {
            for(; curLine > medLine; --curLine) {
                m_ctrl->LineUp();
            }
        } else if(curLine < medLine) {
            for(; curLine < medLine; ++curLine) {
                m_ctrl->LineDown();
            }
        }
        this->m_saveCommand = false;
        repeat_command = false;
    } break;
    case COMMANDVI::L: {
        int lastLine = m_ctrl->GetFirstVisibleLine() + m_ctrl->LinesOnScreen();
        int curLine = m_ctrl->GetCurrentLine();
        for(; curLine < lastLine; ++curLine) {
            m_ctrl->LineDown();
        }
        this->m_saveCommand = false;
        repeat_command = false;
    } break;

    case COMMANDVI::_0:
        m_ctrl->Home();
        this->m_saveCommand = false;
        repeat_command = false;
        break;
    case COMMANDVI::_$:
        m_ctrl->LineEnd();
        m_ctrl->CharLeft();
        this->m_saveCommand = false;
        repeat_command = false;
        break;
    case COMMANDVI::_V:
        m_ctrl->Home();
        if(m_ctrl->GetCharAt(m_ctrl->GetCurrentPos()) <= 32)
            m_ctrl->WordRight();
        this->m_saveCommand = false;
        repeat_command = false;
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
        long pos = m_ctrl->GetCurrentPos() - 1;
        if(pos >= 0 && m_ctrl->GetCharAt(pos) <= 32) {
            while(pos >= 0 && m_ctrl->GetCharAt(pos) <= 32)
                pos--;
        }
        while(pos >= 0 && m_ctrl->GetCharAt(pos) > 32)
            pos--;
        m_ctrl->GotoPos(pos + 1);
        m_ctrl->CharRight();
        m_ctrl->CharLeft();
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
        m_ctrl->CharRight();
        m_ctrl->CharLeft();
        this->m_saveCommand = false;
        break;
    }
    case COMMANDVI::E: {
        long pos = m_ctrl->GetCurrentPos() + 1;
        long len = m_ctrl->GetLength();
        if(pos < len && m_ctrl->GetCharAt(pos) <= 32) {
            while(pos < len && m_ctrl->GetCharAt(pos) <= 32)
                pos++;
        }
        while(pos < len && m_ctrl->GetCharAt(pos) > 32)
            pos++;
        m_ctrl->GotoPos(pos - 1);
        m_ctrl->CharRight();
        m_ctrl->CharLeft();
        this->m_saveCommand = false;
        break;
    }
    case COMMANDVI::F: {
        long pos = findCharInLine(m_actionCommand, -1, false);
        if(pos >= 0) {
            m_ctrl->GotoPos(pos);
            m_ctrl->CharRight();
            m_ctrl->CharLeft();
        }
        m_findKey = m_actionCommand;
        m_findStep = -1;
        m_findPosPrev = false;
        this->m_saveCommand = false;
    } break;
    case COMMANDVI::f: {
        long pos = findCharInLine(m_actionCommand, 1, false);
        if(pos >= 0) {
            m_ctrl->GotoPos(pos);
            m_ctrl->CharRight();
            m_ctrl->CharLeft();
        }
        m_findKey = m_actionCommand;
        m_findStep = 1;
        m_findPosPrev = false;
        this->m_saveCommand = false;
    } break;
    case COMMANDVI::T: {
        long pos = findCharInLine(m_actionCommand, -1, true);
        if(pos >= 0) {
            m_ctrl->GotoPos(pos);
            m_ctrl->CharRight();
            m_ctrl->CharLeft();
        }
        m_findKey = m_actionCommand;
        m_findStep = -1;
        m_findPosPrev = true;
        this->m_saveCommand = false;
    } break;
    case COMMANDVI::t: {
        long pos = findCharInLine(m_actionCommand, 1, true);
        if(pos >= 0) {
            m_ctrl->GotoPos(pos);
            m_ctrl->CharRight();
            m_ctrl->CharLeft();
        }
        m_findKey = m_actionCommand;
        m_findStep = 1;
        m_findPosPrev = true;
        this->m_saveCommand = false;
    } break;
    case COMMANDVI::semicolon: {
        long pos = findCharInLine(m_findKey, m_findStep, m_findPosPrev, true);
        if(pos >= 0) {
            m_ctrl->GotoPos(pos);
            m_ctrl->CharRight();
            m_ctrl->CharLeft();
        }
        this->m_saveCommand = false;
    } break;
    case COMMANDVI::comma: {
        long pos = findCharInLine(m_findKey, -m_findStep, m_findPosPrev, true);
        if(pos >= 0) {
            m_ctrl->GotoPos(pos);
            m_ctrl->CharRight();
            m_ctrl->CharLeft();
        }
        this->m_saveCommand = false;
    } break;
    case COMMANDVI::ctrl_D: {
        int lines = m_ctrl->LinesOnScreen() / 2;
        m_ctrl->SetFirstVisibleLine(std::min(m_ctrl->GetLineCount(), m_ctrl->GetFirstVisibleLine() + lines));
        m_ctrl->MoveCaretInsideView();
        this->m_saveCommand = false;
    } break;
    case COMMANDVI::ctrl_U: {
        int lines = m_ctrl->LinesOnScreen() / 2;
        m_ctrl->SetFirstVisibleLine(std::max(0, m_ctrl->GetFirstVisibleLine() - lines));
        m_ctrl->MoveCaretInsideView();
        this->m_saveCommand = false;
    } break;
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
    default:
        return false;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    }
    return true;
}

/**
 * This function call on the controller of the actual editor the vim-command.
 * Here is the actual implementation of the binding.
 */
bool VimCommand::Command_call()
{

    if(m_currentModus == VIM_MODI::VISUAL_MODUS) {
        return Command_call_visual_mode();
    }
    if(m_currentModus == VIM_MODI::VISUAL_LINE_MODUS) {
        return command_call_visual_line_mode();
    }
    if(m_currentModus == VIM_MODI::VISUAL_BLOCK_MODUS) {
        return command_call_visual_block_mode();
    }

    bool repeat_command = true;
    this->m_saveCommand = true;
    switch(m_commandID) {
    /*========= PUT IN INSERT MODE =============*/
    case COMMANDVI::i:
        this->m_tmpbuf.Clear();
        break;
    case COMMANDVI::I: {
        this->m_tmpbuf.Clear();
        m_ctrl->Home();
        int c = m_ctrl->GetCharAt(m_ctrl->GetCurrentPos());
        if(c <= 32 && (c != '\r' && c != '\n'))
            m_ctrl->WordRight();
    } break;
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
        m_ctrl->Home();
        if(m_ctrl->GetCharAt(m_ctrl->GetCurrentPos()) <= 32)
            m_ctrl->WordRight();
        m_ctrl->NewLine();
        m_ctrl->LineUp();
        break;
    case COMMANDVI::block_I: {
        m_visualBlockBeginLine = m_ctrl->LineFromPosition(m_initialVisualPos);
        m_visualBlockEndLine = m_ctrl->LineFromPosition(m_ctrl->GetCurrentPos());
        m_visualBlockBeginCol = m_ctrl->GetColumn(m_initialVisualPos);
        m_visualBlockEndCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());

        int begin_line = m_visualBlockBeginLine;
        int end_line = m_visualBlockEndLine;
        int begin_col = m_visualBlockBeginCol;
        int end_col = m_visualBlockEndCol;

        if(end_line < begin_line) {
            std::swap(end_line, begin_line);
        }
        if(begin_col > end_col) {
            std::swap(begin_col, end_col);
        }
        m_ctrl->GotoPos(m_ctrl->FindColumn(begin_line, begin_col));
    } break;
    case COMMANDVI::block_A: {
        m_visualBlockBeginLine = m_ctrl->LineFromPosition(m_initialVisualPos);
        m_visualBlockEndLine = m_ctrl->LineFromPosition(m_ctrl->GetCurrentPos());
        m_visualBlockBeginCol = m_ctrl->GetColumn(m_initialVisualPos);
        m_visualBlockEndCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());

        int begin_line = m_visualBlockBeginLine;
        int end_line = m_visualBlockEndLine;
        int begin_col = m_visualBlockBeginCol;
        int end_col = m_visualBlockEndCol;

        if(end_line < begin_line) {
            std::swap(end_line, begin_line);
        }
        if(begin_col > end_col) {
            std::swap(begin_col, end_col);
        }
        m_ctrl->GotoPos(findNextCharPos(begin_line, end_col));
        int curCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
        while(curCol <= end_col) {
            ++curCol;
            m_ctrl->AddText(" ");
        }
    } break;
    /*=============== VISUAL MODE ===============*/
    case COMMANDVI::v:
        m_currentModus = VIM_MODI::VISUAL_MODUS;
        this->m_tmpbuf.Clear();
        this->m_initialVisualPos = this->m_ctrl->GetCurrentPos();
        break;

    case COMMANDVI::V: {
        m_currentModus = VIM_MODI::VISUAL_LINE_MODUS;
        this->m_tmpbuf.Clear();
        this->m_initialVisualLine = this->m_ctrl->GetCurrentLine();

        this->m_ctrl->Home();
        this->m_ctrl->SetAnchor(this->m_ctrl->GetLineEndPosition(this->m_initialVisualLine));

    } break;

    case COMMANDVI::ctrl_V: {
        m_currentModus = VIM_MODI::VISUAL_BLOCK_MODUS;
        this->m_tmpbuf.Clear();
        this->m_initialVisualPos = this->m_ctrl->GetCurrentPos();
    } break;

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
        this->m_saveCommand = false;
        break;
    /*========= REDO =============*/
    case COMMANDVI::ctrl_R:
        m_ctrl->Redo();
        this->m_saveCommand = false;
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

    case COMMANDVI::S:
    case COMMANDVI::cc:
        m_ctrl->Home();
    case COMMANDVI::C:
    case COMMANDVI::D: {
        int pos_init_yw = m_ctrl->GetCurrentPos();
        m_ctrl->LineEnd();
        for(int i = 0; i < m_repeat - 1; ++i) {
            m_ctrl->LineDown();
            m_ctrl->LineEnd();
        }
        int pos_end_yw = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_yw, pos_end_yw));
        m_ctrl->DeleteRange(pos_init_yw, pos_end_yw - pos_init_yw);
        repeat_command = false;
        m_newLineCopy = false;
        m_visualBlockCopy = false;
        break;
    }
    case COMMANDVI::s: {
        this->m_tmpbuf.Clear();
        int repeat_s = std::max(1, m_repeat) * std::max(1, m_actions);
        int start_pos = m_ctrl->GetCurrentPos();
        int end_pos = std::min(repeat_s + start_pos, m_ctrl->GetLineEndPosition(m_ctrl->GetCurrentLine()));
        this->m_listCopiedStr.push_back(m_ctrl->GetTextRange(start_pos, end_pos));
        m_ctrl->DeleteRange(start_pos, end_pos - start_pos);
        repeat_command = false;
        m_newLineCopy = true;
        m_visualBlockCopy = false;
    } break;

    case COMMANDVI::x: {
        this->m_tmpbuf.Clear();
        int line = m_ctrl->GetCurrentLine();
        int start = m_ctrl->GetCurrentPos();
        int col = m_ctrl->GetColumn(start);
        int end = findNextCharPos(line, col);

        wxString str = m_ctrl->GetTextRange(start, end);
        this->m_listCopiedStr.push_back(str);
        m_ctrl->DeleteRange(start, end - start);
    } break;
    case COMMANDVI::X: {
        this->m_tmpbuf.Clear();
        int line = m_ctrl->GetCurrentLine();
        int end = m_ctrl->GetCurrentPos();
        int col = m_ctrl->GetColumn(end);
        int start = findPrevCharPos(line, col);

        wxString str = m_ctrl->GetTextRange(start, end);
        this->m_listCopiedStr.push_back(str);
        m_ctrl->DeleteRange(start, end - start);
    } break;

    case COMMANDVI::dw: {
        int repeat_dw = std::max(1, m_actions);
        for(int i = 0; i < repeat_dw; ++i) {
            m_listCopiedStr.push_back(get_text_at_position(kFromPosToEndWord));
            if(is_space_following())
                m_listCopiedStr.push_back(add_following_spaces());
            m_newLineCopy = false;
            m_visualBlockCopy = false;
            m_ctrl->DelWordRight();
        }
    } break;
    case COMMANDVI::cb:
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
        m_visualBlockCopy = false;
    } break;
    case COMMANDVI::cw:
    case COMMANDVI::ce:
    case COMMANDVI::de: {
        int repeat_de = std::max(1, m_repeat) * std::max(1, m_actions);
        for(int i = 0; i < repeat_de - 1; ++i) {
            m_listCopiedStr.push_back(get_text_at_position(kFromPosToEndWord));
            if(is_space_following())
                m_listCopiedStr.push_back(add_following_spaces());
            m_newLineCopy = false;
            m_ctrl->DelWordRight();
        } // last only the end of the word!
        m_listCopiedStr.push_back(get_text_at_position(kFromPosToEndWord));
        repeat_command = false;
        m_newLineCopy = false;
        m_visualBlockCopy = false;
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
    case COMMANDVI::c0:
    case COMMANDVI::d0: {
        int pos_init_yw = m_ctrl->GetCurrentPos();
        m_ctrl->Home();
        int pos_end_yw = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_end_yw, pos_init_yw));
        m_ctrl->DeleteRange(pos_end_yw, pos_init_yw - pos_end_yw);
        repeat_command = false;
        m_newLineCopy = false;
        m_visualBlockCopy = false;
        break;
    }
    case COMMANDVI::c$:
    case COMMANDVI::d$: {
        int repeat = std::max(1, m_repeat) * std::max(1, m_actions);
        int pos_init_yw = m_ctrl->GetCurrentPos();
        m_ctrl->LineEnd();
        for(int i = 0; i < repeat - 1; ++i) {
            m_ctrl->LineDown();
            m_ctrl->LineEnd();
        }
        int pos_end_yw = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_yw, pos_end_yw));
        m_ctrl->DeleteRange(pos_init_yw, pos_end_yw - pos_init_yw);
        repeat_command = false;
        m_newLineCopy = false;
        m_visualBlockCopy = false;
        break;
    }
    case COMMANDVI::c_caret:
    case COMMANDVI::d_caret: {
        int pos_init_cV = m_ctrl->GetCurrentPos();
        m_ctrl->Home();
        if(m_ctrl->GetCharAt(m_ctrl->GetCurrentPos()) <= 32)
            m_ctrl->WordRight();
        int pos_end_cV = m_ctrl->GetCurrentPos();
        if(pos_end_cV < pos_init_cV) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_end_cV, pos_init_cV));
            m_ctrl->DeleteRange(pos_end_cV, pos_init_cV - pos_end_cV);
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        } else {
            m_ctrl->SetCurrentPos(pos_init_cV);
            m_ctrl->CharLeft();
        }
        break;
    }
    case COMMANDVI::dgg:
    case COMMANDVI::dG: {
        int begin_line = m_ctrl->GetCurrentLine();
        int end_line = std::max(0, m_ctrl->GetLineCount() - 1);
        if(m_commandID == COMMANDVI::dgg) {
            end_line = 0;
        }
        int line = std::max(1, m_repeat) * std::max(1, m_actions);
        if(line > 1) {
            end_line = std::min(line - 1, m_ctrl->GetLineCount() - 1);
        }
        if(begin_line > end_line) {
            std::swap(begin_line, end_line);
        }

        m_ctrl->GotoLine(begin_line);
        for(int i = begin_line; i <= end_line; ++i) {
            this->m_listCopiedStr.push_back(m_ctrl->GetCurLine());
            m_ctrl->LineDelete();
        }
        repeat_command = false;
        m_visualBlockCopy = false;
    } break;

    case COMMANDVI::caw:
    case COMMANDVI::daw: {
        int repeat = std::max(1, m_repeat) * std::max(1, m_actions);
        m_ctrl->WordRightEnd();
        m_ctrl->WordLeft();
        long begin = m_ctrl->GetCurrentPos();
        for(int i = 0; i < repeat; ++i) {
            m_ctrl->WordRight();
        }
        int end = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(begin, end));
        m_ctrl->DeleteRange(begin, end - begin);
        repeat_command = false;
        m_newLineCopy = false;
        m_visualBlockCopy = false;
        break;
    }
    case COMMANDVI::yiw:
    case COMMANDVI::ciw:
    case COMMANDVI::diw: {
        int repeat = std::max(1, m_repeat) * std::max(1, m_actions);
        long pos = m_ctrl->GetCurrentPos();
        if(m_ctrl->GetCharAt(pos) <= 32) {
            m_ctrl->WordLeft();
            m_ctrl->GotoPos(m_ctrl->WordEndPosition(m_ctrl->GetCurrentPos(), false));
        } else {
            m_ctrl->WordRightEnd();
            m_ctrl->WordLeft();
        }
        long start_pos = m_ctrl->GetCurrentPos();
        while(repeat > 0) {
            if(m_ctrl->GetCharAt(m_ctrl->GetCurrentPos()) <= 32) {
                m_ctrl->WordRight();
            } else {
                m_ctrl->GotoPos(m_ctrl->WordEndPosition(m_ctrl->GetCurrentPos(), false));
            }
            repeat--;
        }
        long end_pos = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(start_pos, end_pos));
        if(m_commandID != COMMANDVI::yiw) {
            m_ctrl->DeleteRange(start_pos, end_pos - start_pos);
        }
        repeat_command = false;
        m_newLineCopy = false;
        m_visualBlockCopy = false;
        break;
    }
    case COMMANDVI::yf:
    case COMMANDVI::cf:
    case COMMANDVI::df: {
        long start_pos = m_ctrl->GetCurrentPos();
        long end_pos = findCharInLine(m_actionCommand, 1);
        if(end_pos >= 0) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(start_pos, end_pos + 1));
            if(m_commandID != COMMANDVI::yf)
                m_ctrl->DeleteRange(start_pos, end_pos + 1 - start_pos);
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yF:
    case COMMANDVI::cF:
    case COMMANDVI::dF: {
        long start_pos = m_ctrl->GetCurrentPos();
        long end_pos = findCharInLine(m_actionCommand, -1);
        if(end_pos >= 0) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(start_pos, end_pos));
            if(m_commandID != COMMANDVI::yF)
                m_ctrl->DeleteRange(end_pos, start_pos - end_pos);
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yt:
    case COMMANDVI::ct:
    case COMMANDVI::dt: {
        long start_pos = m_ctrl->GetCurrentPos();
        long end_pos = findCharInLine(m_actionCommand, 1, true);
        if(end_pos >= 0) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(start_pos, end_pos + 1));
            if(m_commandID != COMMANDVI::yt)
                m_ctrl->DeleteRange(start_pos, end_pos + 1 - start_pos);
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yT:
    case COMMANDVI::cT:
    case COMMANDVI::dT: {
        long start_pos = m_ctrl->GetCurrentPos();
        long end_pos = findCharInLine(m_actionCommand, -1, true);
        if(end_pos >= 0) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(start_pos, end_pos));
            if(m_commandID != COMMANDVI::yT)
                m_ctrl->DeleteRange(end_pos, start_pos - end_pos);
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yi_quot:
    case COMMANDVI::ci_quot:
    case COMMANDVI::di_quot: {
        int minPos = m_ctrl->PositionFromLine(m_ctrl->GetCurrentLine());
        int maxPos = m_ctrl->PositionFromLine(m_ctrl->GetCurrentLine() + 1);
        long leftPos = -1;
        long rightPos = -1;
        if(findMatchingParentesis('"', '"', minPos, maxPos, leftPos, rightPos)) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(leftPos + 1, rightPos));
            if(m_commandID != COMMANDVI::yi_quot) {
                m_ctrl->SetCurrentPos(leftPos + 1);
                m_ctrl->DeleteRange(leftPos + 1, rightPos - leftPos - 1);
            }
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yi_apos:
    case COMMANDVI::ci_apos:
    case COMMANDVI::di_apos: {
        int minPos = m_ctrl->PositionFromLine(m_ctrl->GetCurrentLine());
        int maxPos = m_ctrl->PositionFromLine(m_ctrl->GetCurrentLine() + 1);
        long leftPos = -1;
        long rightPos = -1;
        if(findMatchingParentesis('\'', '\'', minPos, maxPos, leftPos, rightPos)) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(leftPos + 1, rightPos));
            if(m_commandID != COMMANDVI::yi_apos) {
                m_ctrl->SetCurrentPos(leftPos + 1);
                m_ctrl->DeleteRange(leftPos + 1, rightPos - leftPos - 1);
            }
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yi_pare:
    case COMMANDVI::ci_pare:
    case COMMANDVI::di_pare: {
        int minPos = 0;
        int maxPos = m_ctrl->GetLastPosition();
        long leftPos = -1;
        long rightPos = -1;
        if(findMatchingParentesis('(', ')', minPos, maxPos, leftPos, rightPos)) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(leftPos + 1, rightPos));
            if(m_commandID != COMMANDVI::yi_pare) {
                m_ctrl->SetCurrentPos(leftPos + 1);
                m_ctrl->DeleteRange(leftPos + 1, rightPos - leftPos - 1);
            }
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yi_lt:
    case COMMANDVI::ci_lt:
    case COMMANDVI::di_lt: {
        int minPos = 0;
        int maxPos = m_ctrl->GetLastPosition();
        long leftPos = -1;
        long rightPos = -1;
        if(findMatchingParentesis('<', '>', minPos, maxPos, leftPos, rightPos)) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(leftPos + 1, rightPos));
            if(m_commandID != COMMANDVI::yi_lt) {
                m_ctrl->SetCurrentPos(leftPos + 1);
                m_ctrl->DeleteRange(leftPos + 1, rightPos - leftPos - 1);
            }
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yi_square:
    case COMMANDVI::ci_square:
    case COMMANDVI::di_square: {
        int minPos = 0;
        int maxPos = m_ctrl->GetLastPosition();
        long leftPos = -1;
        long rightPos = -1;
        if(findMatchingParentesis('[', ']', minPos, maxPos, leftPos, rightPos)) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(leftPos + 1, rightPos));
            if(m_commandID != COMMANDVI::yi_square) {
                m_ctrl->SetCurrentPos(leftPos + 1);
                m_ctrl->DeleteRange(leftPos + 1, rightPos - leftPos - 1);
            }
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }
    case COMMANDVI::yi_curly:
    case COMMANDVI::ci_curly:
    case COMMANDVI::di_curly: {
        int minPos = 0;
        int maxPos = m_ctrl->GetLastPosition();
        long leftPos = -1;
        long rightPos = -1;
        if(findMatchingParentesis('{', '}', minPos, maxPos, leftPos, rightPos)) {
            m_listCopiedStr.push_back(m_ctrl->GetTextRange(leftPos + 1, rightPos));
            if(m_commandID != COMMANDVI::yi_curly) {
                m_ctrl->SetCurrentPos(leftPos + 1);
                m_ctrl->DeleteRange(leftPos + 1, rightPos - leftPos - 1);
            }
            repeat_command = false;
            m_newLineCopy = false;
            m_visualBlockCopy = false;
        }
        break;
    }

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
        m_newLineCopy = false;
        m_visualBlockCopy = false;
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
        m_ctrl->SetCurrentPos(pos_end_yb);
        m_ctrl->CharLeft();
        m_newLineCopy = false;
        m_visualBlockCopy = false;
    } break;
    case COMMANDVI::ye: {
        int pos_init_ye = m_ctrl->GetCurrentPos();
        int repeat_ye = std::max(1, m_repeat) * std::max(1, m_actions);
        for(int i = 0; i < repeat_ye; ++i) {
            m_ctrl->WordRightEnd();
        }
        int pos_end_ye = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_ye, pos_end_ye));
        repeat_command = false;
        m_ctrl->SetCurrentPos(pos_init_ye);
        m_ctrl->CharLeft();
        m_newLineCopy = false;
        m_visualBlockCopy = false;
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
    } break;

    case COMMANDVI::y0: {
        int pos_init_yw = m_ctrl->GetCurrentPos();
        int repeat_yw = std::max(1, m_repeat) * std::max(1, m_actions);
        m_ctrl->Home();
        int pos_end_yw = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_yw, pos_end_yw));
        repeat_command = false;
        m_ctrl->SetCurrentPos(pos_init_yw);
        m_ctrl->CharLeft();
        m_newLineCopy = false;
        m_visualBlockCopy = false;
    } break;
    case COMMANDVI::y$: {
        int pos_init_yw = m_ctrl->GetCurrentPos();
        int repeat_yw = std::max(1, m_repeat) * std::max(1, m_actions);
        m_ctrl->LineEnd();
        int pos_end_yw = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_yw, pos_end_yw));
        repeat_command = false;
        m_ctrl->SetCurrentPos(pos_init_yw);
        m_ctrl->CharLeft();
        m_newLineCopy = false;
        m_visualBlockCopy = false;
    } break;
    case COMMANDVI::y_caret: {
        int pos_init_yw = m_ctrl->GetCurrentPos();
        int repeat_yw = std::max(1, m_repeat) * std::max(1, m_actions);
        m_ctrl->Home();
        if(m_ctrl->GetCharAt(m_ctrl->GetCurrentPos()) <= 32)
            m_ctrl->WordRight();
        int pos_end_yw = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(pos_init_yw, pos_end_yw));
        repeat_command = false;
        m_ctrl->SetCurrentPos(pos_init_yw);
        m_ctrl->CharLeft();
        m_newLineCopy = false;
        m_visualBlockCopy = false;
        break;
    }
    case COMMANDVI::ygg:
    case COMMANDVI::yG: {
        int pos = m_ctrl->GetCurrentPos();
        int begin_line = m_ctrl->GetCurrentLine();
        int end_line = std::max(0, m_ctrl->GetLineCount() - 1);
        if(m_commandID == COMMANDVI::ygg) {
            end_line = 0;
        }

        int line = std::max(1, m_repeat) * std::max(1, m_actions);
        if(line > 1) {
            end_line = std::min(line - 1, m_ctrl->GetLineCount() - 1);
        }

        if(begin_line > end_line) {
            std::swap(begin_line, end_line);
        }

        m_ctrl->GotoLine(begin_line);
        for(int i = begin_line; i <= end_line; ++i) {
            this->m_listCopiedStr.push_back(m_ctrl->GetCurLine());
            m_ctrl->LineDown();
        }
        m_ctrl->GotoPos(pos);
        repeat_command = false;
        m_visualBlockCopy = false;
    } break;
    case COMMANDVI::yaw: {
        int repeat = std::max(1, m_repeat) * std::max(1, m_actions);
        m_ctrl->WordRightEnd();
        m_ctrl->WordLeft();
        long begin = m_ctrl->GetCurrentPos();
        for(int i = 0; i < repeat; ++i) {
            m_ctrl->WordRight();
        }
        int end = m_ctrl->GetCurrentPos();
        m_listCopiedStr.push_back(m_ctrl->GetTextRange(begin, end));
        m_ctrl->GotoPos(begin);
        repeat_command = false;
        m_newLineCopy = false;
        m_visualBlockCopy = false;
        break;
    }
    case COMMANDVI::diesis: {
        m_searchWord = get_text_at_position();
        // m_ctrl->SetCurrentPos( /*FIXME*/ );
        search_word(SEARCH_DIRECTION::BACKWARD, wxSTC_FIND_WHOLEWORD);
        m_message_ID = MESSAGES_VIM::SEARCHING_WORD;
    } break;

    case COMMANDVI::diesis_N: {
        m_searchWord = get_text_at_position();
        search_word(SEARCH_DIRECTION::FORWARD, wxSTC_FIND_WHOLEWORD);
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
    case COMMANDVI::p: { /*FIXME CharLeft goes the previous line if at position 0!*/
        bool isLineEnd = false;
        this->m_saveCommand = false;
        if(this->m_newLineCopy) {
            m_ctrl->LineEnd();
            m_ctrl->NewLine();
            m_ctrl->DelLineLeft();
            if(m_listCopiedStr.size() > 0) {
                wxString& str = m_listCopiedStr.back();
                // Causes assert failure to access 'Last()' member of empty string
                while(!str.IsEmpty()) {
                    if(str.Last() == '\r' || str.Last() == '\n') {
                        str.RemoveLast();
                    } else {
                        break;
                    }
                }
            }
        } else {
            int line = m_ctrl->GetCurrentLine();
            m_ctrl->CharRight();
            if(m_ctrl->GetCurrentLine() != line) {
                m_ctrl->CharLeft();
                isLineEnd = true;
            }
        }
        int currentLine = this->m_ctrl->GetCurrentLine();
        if(m_visualBlockCopy) {
            int col = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
            int startPos = m_ctrl->GetCurrentPos();
            for(std::vector<wxString>::iterator yanked = this->m_listCopiedStr.begin();
                yanked != this->m_listCopiedStr.end();) {
                int pos = m_ctrl->GetCurrentPos();
                m_ctrl->AddText(*yanked);
                m_ctrl->GotoPos(pos);
                if(++yanked == m_listCopiedStr.end()) {
                    break;
                }
                if(m_ctrl->GetCurrentLine() + 1 == m_ctrl->GetLineCount()) {
                    m_ctrl->LineEnd();
                    m_ctrl->NewLine();
                } else {
                    m_ctrl->LineDown();
                }
                int curCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
                while(curCol < col) {
                    curCol++;
                    m_ctrl->AddText(" ");
                }
            }
            m_ctrl->GotoPos((isLineEnd) ? startPos : std::max(0, startPos - 1));
        } else {
            int repeat = std::max(1, m_repeat) * std::max(1, m_actions);
            for(int i = 0; i < repeat; ++i) {
                for(std::vector<wxString>::iterator yanked = this->m_listCopiedStr.begin();
                    yanked != this->m_listCopiedStr.end(); ++yanked) {
                    m_ctrl->AddText(*yanked);
                }
                if(this->m_newLineCopy && i < repeat - 1) {
                    m_ctrl->NewLine();
                    int pos = m_ctrl->GetCurrentPos();
                    m_ctrl->Home();
                    m_ctrl->DeleteRange(m_ctrl->GetCurrentPos(), pos - m_ctrl->GetCurrentPos());
                }
            }
            repeat_command = false;
        }
        // FIXME: troppo contorto!
        if(this->m_newLineCopy) {
            this->m_ctrl->GotoLine(currentLine);
        }
    } break;
    case COMMANDVI::P: {
        this->m_saveCommand = false;
        if(this->m_newLineCopy) {
            m_ctrl->Home();
            m_ctrl->NewLine();
            m_ctrl->LineUp();
            if(m_listCopiedStr.size() > 0) {
                wxString& str = m_listCopiedStr.back();
                // Causes assert failure to access 'Last()' member of empty string
                while(!str.IsEmpty()) {
                    if(str.Last() == '\r' || str.Last() == '\n') {
                        str.RemoveLast();
                    } else {
                        break;
                    }
                }
            }
        }

        int currentLine = this->m_ctrl->GetCurrentLine();
        if(m_visualBlockCopy) {
            int col = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
            int startPos = m_ctrl->GetCurrentPos();
            for(std::vector<wxString>::iterator yanked = this->m_listCopiedStr.begin();
                yanked != this->m_listCopiedStr.end();) {
                int pos = m_ctrl->GetCurrentPos();
                m_ctrl->AddText(*yanked);
                m_ctrl->GotoPos(pos);
                if(++yanked == m_listCopiedStr.end()) {
                    break;
                }
                if(m_ctrl->GetCurrentLine() + 1 == m_ctrl->GetLineCount()) {
                    m_ctrl->LineEnd();
                    m_ctrl->NewLine();
                } else {
                    m_ctrl->LineDown();
                }
                int curCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
                while(curCol < col) {
                    curCol++;
                    m_ctrl->AddText(" ");
                }
            }
            m_ctrl->GotoPos(startPos);
        } else {
            for(std::vector<wxString>::iterator yanked = this->m_listCopiedStr.begin();
                yanked != this->m_listCopiedStr.end(); ++yanked) {
                m_ctrl->AddText(*yanked);
            }
        }
        // FIXME: troppo contorto!
        if(this->m_newLineCopy) {
            this->m_ctrl->GotoLine(currentLine);
        }
    } break;
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
            if(m_ctrl->GetCharAt(m_ctrl->GetCurrentPos()) <= 32)
                m_ctrl->DelWordRight();
            m_ctrl->AddText(" ");
            m_ctrl->SetCurrentPos(curr_pos);
            /*FIXME: Workaround to avoid selection between previous
              end of line and current position*/
            m_ctrl->CharLeft();
            m_ctrl->CharRight();
            break;
        }
    case COMMANDVI::tilde: {
        int line = m_ctrl->GetCurrentLine();
        int start = m_ctrl->GetCurrentPos();
        int col = m_ctrl->GetColumn(start);
        int end = findNextCharPos(line, col);

        wxString str = m_ctrl->GetTextRange(start, end);

        bool replace = false;
        for(wxString::iterator it = str.begin(); it != str.end(); ++it) {
            if(wxIslower(*it)) {
                replace = true;
                *it = wxToupper(*it);
            } else if(wxIsupper(*it)) {
                replace = true;
                *it = wxTolower(*it);
            }
        }
        if(replace) {
            m_ctrl->Replace(start, end, str);
        }
        m_ctrl->CharRight();
    } break;
    default:
        if(command_move_cmd_call(repeat_command) == false) {
            repeat_command = false;
        }
        break;
    }

    return repeat_command;
}

bool VimCommand::Command_call_visual_mode()
{

    bool repeat_command = true;
    this->m_saveCommand = true;
    long caretPosition = 0;
    int pos = m_ctrl->GetCurrentPos();
    m_ctrl->SetAnchor(pos);
    switch(m_commandID) {
        /*========== DELETE AND COPY =======================*/

    case COMMANDVI::d:
    case COMMANDVI::x:
    case COMMANDVI::y:
        caretPosition = this->m_ctrl->GetCurrentPos();
        /* vim selects under cursor too, only an issue when selecting forward and
         * deleting or yanking*/
        if(caretPosition > m_initialVisualPos) {
            m_ctrl->SetSelection(m_initialVisualPos, caretPosition + 1);
        } else {
            m_ctrl->SetSelection(caretPosition, m_initialVisualPos + 1);
        }

        this->m_listCopiedStr.push_back(m_ctrl->GetSelectedText());
        this->m_saveCommand = false; /*FIXME: check what is vim-behaviour*/
        m_currentModus = VIM_MODI::NORMAL_MODUS;
        this->m_newLineCopy = false;
        m_visualBlockCopy = false;

        if(this->m_commandID != COMMANDVI::y) {
            m_ctrl->DeleteBack(); /*? better use Clear()*/
        } else {
            m_ctrl->GotoPos(caretPosition);
        }
        return repeat_command;
        break;
    default:
        for(int i = 0; i < this->getNumRepeat(); ++i) {
            if(command_move_cmd_call(repeat_command) == false) {
                break;
            }
            if(repeat_command == false) {
                break;
            }
        }
        repeat_command = false;
        break;
    }

    caretPosition = this->m_ctrl->GetCurrentPos();
    if(caretPosition > m_initialVisualPos) {
        this->m_ctrl->SetAnchor(m_initialVisualPos);
    } else {
        this->m_ctrl->SetAnchor(m_initialVisualPos + 1);
    }

    return repeat_command;
}

bool VimCommand::command_call_visual_line_mode()
{
    bool repeat_command = true;
    this->m_saveCommand = false;

    m_ctrl->SetAnchor(m_ctrl->GetCurrentPos());
    switch(m_commandID) {
    case COMMANDVI::h:
    case COMMANDVI::l:
    case COMMANDVI::_0:
    case COMMANDVI::_$:
    case COMMANDVI::_V:
    case COMMANDVI::w:
    case COMMANDVI::W:
    case COMMANDVI::b:
    case COMMANDVI::B:
    case COMMANDVI::e:
    case COMMANDVI::E:
    case COMMANDVI::F:
    case COMMANDVI::f:
    case COMMANDVI::T:
    case COMMANDVI::t:
    case COMMANDVI::semicolon:
    case COMMANDVI::comma:
        repeat_command = false;
        break;
        /*========== DELETE AND COPY =======================*/
    case COMMANDVI::d:
    case COMMANDVI::x:
    case COMMANDVI::y: {
        int pos = m_ctrl->GetCurrentPos();
        int beginLine = m_ctrl->GetCurrentLine();
        int endLine = m_initialVisualLine;
        if(beginLine > endLine) {
            std::swap(beginLine, endLine);
        }

        m_ctrl->SetSelection(m_ctrl->PositionFromLine(beginLine), m_ctrl->GetLineEndPosition(endLine));

        this->m_listCopiedStr.push_back(this->m_ctrl->GetSelectedText());
        this->m_currentModus = VIM_MODI::NORMAL_MODUS;
        repeat_command = false;
        this->m_newLineCopy = true;
        m_visualBlockCopy = false;

        if(this->m_commandID != COMMANDVI::y) {
            m_ctrl->DeleteBack();
            m_ctrl->LineDelete(); // line is left behind
        } else {
            m_ctrl->GotoPos(pos);
        }
        return repeat_command;
    }
    default:

        for(int i = 0; i < this->getNumRepeat(); ++i) {
            if(command_move_cmd_call(repeat_command) == false) {
                break;
            }
            if(repeat_command == false) {
                break;
            }
        }
        repeat_command = false;
    }

    int currentLine = this->m_ctrl->GetCurrentLine();

    if(currentLine > this->m_initialVisualLine) {
        int newAnchor = this->m_ctrl->PositionFromLine(this->m_initialVisualLine);
        int newCaretPos = this->m_ctrl->GetLineEndPosition(currentLine);

        this->m_ctrl->GotoPos(newCaretPos);
        this->m_ctrl->SetAnchor(newAnchor);
    } else {
        this->m_ctrl->Home();
        this->m_ctrl->SetAnchor(this->m_ctrl->GetLineEndPosition(this->m_initialVisualLine));
    }

    return repeat_command;
}

bool VimCommand::command_call_visual_block_mode()
{
    bool repeat_command = true;
    this->m_saveCommand = true;
    switch(m_commandID) {
    case COMMANDVI::o: {
        int pos = m_ctrl->GetCurrentPos();
        m_ctrl->GotoPos(m_initialVisualPos);
        m_ctrl->CharRight();
        m_ctrl->CharLeft();
        m_initialVisualPos = pos;
        this->m_saveCommand = false;
        repeat_command = false;
    } break;
    case COMMANDVI::O: {
        int begin_line = m_ctrl->LineFromPosition(m_initialVisualPos);
        int cur_line = m_ctrl->GetCurrentLine();
        int begin_col = m_ctrl->GetColumn(m_initialVisualPos);
        int cur_col = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
        std::swap(begin_col, cur_col);
        m_initialVisualPos = m_ctrl->FindColumn(begin_line, begin_col);
        m_ctrl->GotoPos(m_ctrl->FindColumn(cur_line, cur_col));
        m_ctrl->CharRight();
        m_ctrl->CharLeft();
        this->m_saveCommand = false;
        repeat_command = false;
    } break;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case COMMANDVI::tilde: {
        int begin_line = m_ctrl->LineFromPosition(m_initialVisualPos);
        int end_line = m_ctrl->GetCurrentLine();
        int begin_col = m_ctrl->GetColumn(m_initialVisualPos);
        int end_col = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
        if(end_line < begin_line) {
            std::swap(end_line, begin_line);
        }
        if(end_col < begin_col) {
            std::swap(end_col, begin_col);
        }

        for(int line = begin_line; line <= end_line; ++line) {
            int start = m_ctrl->FindColumn(line, begin_col);
            int end = findNextCharPos(line, end_col);
            if(end >= start) {
                wxString str = m_ctrl->GetTextRange(start, end);
                bool replace = false;
                for(wxString::iterator it = str.begin(); it != str.end(); ++it) {
                    if(wxIslower(*it)) {
                        replace = true;
                        *it = wxToupper(*it);
                    } else if(wxIsupper(*it)) {
                        replace = true;
                        *it = wxTolower(*it);
                    }
                }
                if(replace) {
                    m_ctrl->Replace(start, end, str);
                }
            }
        }
        m_ctrl->GotoPos(m_ctrl->FindColumn(begin_line, begin_col));
        m_ctrl->CharRight();
        m_ctrl->CharLeft();

        this->m_saveCommand = false; /*FIXME: check what is vim-behaviour*/
        m_currentModus = VIM_MODI::NORMAL_MODUS;

    } break;
    case COMMANDVI::r: {
        int begin_line = m_ctrl->LineFromPosition(m_initialVisualPos);
        int end_line = m_ctrl->GetCurrentLine();
        int begin_col = m_ctrl->GetColumn(m_initialVisualPos);
        int end_col = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
        if(end_line < begin_line) {
            std::swap(end_line, begin_line);
        }
        if(end_col < begin_col) {
            std::swap(end_col, begin_col);
        }

        for(int line = begin_line; line <= end_line; ++line) {
            int start = m_ctrl->FindColumn(line, begin_col);
            int end = findNextCharPos(line, end_col);
            if(end >= start) {
                wxString str(m_actionCommand, end - start);
                m_ctrl->Replace(start, end, str);
            }
        }
        m_ctrl->GotoPos(m_ctrl->FindColumn(begin_line, begin_col));
        m_ctrl->CharRight();
        m_ctrl->CharLeft();

        this->m_saveCommand = false; /*FIXME: check what is vim-behaviour*/
        m_currentModus = VIM_MODI::NORMAL_MODUS;
    } break;

    /*========== DELETE AND COPY =======================*/
    case COMMANDVI::block_c:
    case COMMANDVI::d:
    case COMMANDVI::x:
    case COMMANDVI::y: {

        m_visualBlockBeginLine = m_ctrl->LineFromPosition(m_initialVisualPos);
        m_visualBlockEndLine = m_ctrl->LineFromPosition(m_ctrl->GetCurrentPos());
        m_visualBlockBeginCol = m_ctrl->GetColumn(m_initialVisualPos);
        m_visualBlockEndCol = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());

        int begin_line = m_visualBlockBeginLine;
        int end_line = m_visualBlockEndLine;
        int begin_col = m_visualBlockBeginCol;
        int end_col = m_visualBlockEndCol;
        if(end_line < begin_line) {
            std::swap(end_line, begin_line);
        }
        if(begin_col > end_col) {
            std::swap(begin_col, end_col);
        }
        for(int line = begin_line; line <= end_line; ++line) {
            int start = m_ctrl->FindColumn(line, begin_col);
            int end = findNextCharPos(line, end_col);
            if(start > end) {
                std::swap(start, end);
            }
            wxString str = m_ctrl->GetTextRange(start, end);
            while(!str.IsEmpty()) {
                if(str.Last() == '\r' || str.Last() == '\n') {
                    str.RemoveLast();
                } else {
                    break;
                }
            }
            m_listCopiedStr.push_back(str);
        }

        if(this->m_commandID != COMMANDVI::y) {
            for(int line = end_line; line >= begin_line; --line) {
                int start = m_ctrl->FindColumn(line, begin_col);
                int end = findNextCharPos(line, end_col);
                if(end >= start) {
                    m_ctrl->DeleteRange(start, end - start);
                }
            }
        }
        m_ctrl->GotoPos(m_ctrl->FindColumn(begin_line, begin_col));
        m_ctrl->CharRight();
        m_ctrl->CharLeft();
        if(this->m_commandID == COMMANDVI::block_c) {
            m_currentModus = VIM_MODI::INSERT_MODUS;
        } else {
            m_currentModus = VIM_MODI::NORMAL_MODUS;
        }

        this->m_saveCommand = false; /*FIXME: check what is vim-behaviour*/
        this->m_newLineCopy = false;
        m_visualBlockCopy = true;

    } break;
    default:
        command_move_cmd_call(repeat_command);
        break;
    }

    m_ctrl->SetIndicatorCurrent(VISUAL_BLOCK_INDICATOR);
    m_ctrl->IndicatorClearRange(0, m_ctrl->GetLength());

    if(m_currentModus != VIM_MODI::VISUAL_BLOCK_MODUS) {
        return repeat_command;
    }

    int begin_line = m_ctrl->LineFromPosition(m_initialVisualPos);
    int end_line = m_ctrl->GetCurrentLine();
    int begin_col = m_ctrl->GetColumn(m_initialVisualPos);
    int end_col = m_ctrl->GetColumn(m_ctrl->GetCurrentPos());
    if(end_line < begin_line) {
        std::swap(end_line, begin_line);
    }
    begin_line = std::max(begin_line, m_ctrl->GetFirstVisibleLine());
    end_line = std::min(end_line, m_ctrl->GetFirstVisibleLine() + m_ctrl->LinesOnScreen());

    for(int line = begin_line; line <= end_line; ++line) {
        int start = m_ctrl->FindColumn(line, begin_col);
        int end = m_ctrl->FindColumn(line, end_col);
        if(start > end) {
            std::swap(start, end);
        }
        m_ctrl->IndicatorFillRange(start, end - start + 1);
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
        end = m_ctrl->WordEndPosition(pos, false);
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
    if(m_ctrl->GetCharAt(pos + 1) == ' ')
        return true;
    long end = m_ctrl->WordEndPosition(pos, true);
    if(m_ctrl->GetCharAt(end) == ' ')
        return true;

    return false;
}

/*FIXME start is right pos?
 * @return true is a space preciding or we reached the start of the file*/
bool VimCommand::is_space_preceding(bool onlyWordChar, bool cross_line)
{
    long pos = m_ctrl->GetCurrentPos();
    if(pos == 0)
        return true;
    long start = m_ctrl->WordStartPosition(pos, onlyWordChar);
    if(m_ctrl->GetCharAt(start) == ' ')
        return true;
    if(cross_line && m_ctrl->GetCharAt(start) == '\n')
        return true;

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

bool VimCommand::search_word(SEARCH_DIRECTION direction, int flag, long start_pos)
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
    m_mgr->GetStatusBar()->SetMessage(_("Searching: ") + m_searchWord);
    bool found = false;
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

bool VimCommand::search_word(SEARCH_DIRECTION direction, int flag)
{

    // /*flag 2: word separated, Big pr small!*/
    // /*flag 3: same as before*/
    // /*flag 1-0: from the find other*/
    long pos = m_ctrl->GetCurrentPos();

    bool found = false;
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
            if(pos_word != wxNOT_FOUND) {
                m_ctrl->GotoPos(pos_word);
            }
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
    VIM_MODI currentModus = m_currentModus;
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
        case 'H':
            possible_command = true;
            m_commandID = COMMANDVI::H;
            command_complete = true;
            break;
        case 'M':
            possible_command = true;
            m_commandID = COMMANDVI::M;
            command_complete = true;
            break;
        case 'L':
            possible_command = true;
            m_commandID = COMMANDVI::L;
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
        case '^':
            possible_command = true;
            m_commandID = COMMANDVI::_V;
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
            if(m_currentModus == VIM_MODI::VISUAL_BLOCK_MODUS) {
                m_commandID = COMMANDVI::block_I;
            } else {
                m_commandID = COMMANDVI::I;
            }
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
            if(m_currentModus == VIM_MODI::VISUAL_BLOCK_MODUS) {
                m_commandID = COMMANDVI::block_A;
            } else {
                m_commandID = COMMANDVI::A;
            }
            m_currentModus = VIM_MODI::INSERT_MODUS;
            command_complete = true;
            m_repeat = 1;
            break;

        case 'o':
            possible_command = true;
            m_commandID = COMMANDVI::o;
            if(m_currentModus != VIM_MODI::VISUAL_BLOCK_MODUS) {
                m_currentModus = VIM_MODI::INSERT_MODUS;
            }
            command_complete = true;
            m_repeat = 1;
            break;
        case 'O':
            possible_command = true;
            m_commandID = COMMANDVI::O;
            if(m_currentModus != VIM_MODI::VISUAL_BLOCK_MODUS) {
                m_currentModus = VIM_MODI::INSERT_MODUS;
            }
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
    case 'V':
        possible_command = true;
        if(this->m_modifierKey == wxMOD_CONTROL) {
            this->m_commandID = COMMANDVI::ctrl_V;
        } else {
            this->m_commandID = COMMANDVI::V;
        }

        command_complete = true;
        this->m_repeat = 1;
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
    case 18:
        if(this->m_modifierKey == wxMOD_CONTROL) {
            command_complete = true;
            possible_command = true;
            m_commandID = COMMANDVI::ctrl_R;
            m_modifierKey = 0;
        }
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
        if(m_externalCommand == 'f') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::cf;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                m_currentModus = VIM_MODI::INSERT_MODUS;
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 'F') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::cF;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                m_currentModus = VIM_MODI::INSERT_MODUS;
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 't') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::ct;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                m_currentModus = VIM_MODI::INSERT_MODUS;
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 'T') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::cT;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                m_currentModus = VIM_MODI::INSERT_MODUS;
                command_complete = true;
            }
            break;
        }

        switch(m_actionCommand) {
        case '\0':
            if(m_currentModus == VIM_MODI::VISUAL_BLOCK_MODUS) {
                command_complete = true;
                m_commandID = COMMANDVI::block_c;
                this->m_listCopiedStr.clear();
            } else {
                command_complete = false;
            }
            break;
        case '0':
            command_complete = true;
            m_commandID = COMMANDVI::c0;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case '^':
            command_complete = true;
            m_commandID = COMMANDVI::c_caret;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case '$':
            command_complete = true;
            m_commandID = COMMANDVI::c$;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case 'w':
            if(m_externalCommand == 'a') {
                command_complete = true;
                m_commandID = COMMANDVI::caw;
                this->m_listCopiedStr.clear();
                m_currentModus = VIM_MODI::INSERT_MODUS;
                break;
            } else if(m_externalCommand == 'i') {
                command_complete = true;
                m_commandID = COMMANDVI::ciw;
                this->m_listCopiedStr.clear();
                m_currentModus = VIM_MODI::INSERT_MODUS;
                break;
            }
            command_complete = true;
            m_commandID = COMMANDVI::cw;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case 'c':
            command_complete = true;
            m_commandID = COMMANDVI::cc;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case 'b':
            command_complete = true;
            m_commandID = COMMANDVI::cb;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;

        case 'e':
            command_complete = true;
            m_commandID = COMMANDVI::ce;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case '\"':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::ci_quot;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case '\'':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::ci_apos;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case '{':
        case '}':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::ci_curly;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case '[':
        case ']':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::ci_square;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case '(':
        case ')':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::ci_pare;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        case '<':
        case '>':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::ci_lt;
            this->m_listCopiedStr.clear();
            m_currentModus = VIM_MODI::INSERT_MODUS;
            break;
        }
        break;

    case 's':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::s;
        this->m_listCopiedStr.clear();
        m_currentModus = VIM_MODI::INSERT_MODUS;
        break;

    case 'S':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::S;
        this->m_listCopiedStr.clear();
        m_currentModus = VIM_MODI::INSERT_MODUS;
        break;

    case 'C':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::C;
        this->m_listCopiedStr.clear();
        m_currentModus = VIM_MODI::INSERT_MODUS;
        break;
    case 'x':
        this->m_newLineCopy = false;
        m_visualBlockCopy = false;
        this->m_listCopiedStr.clear();
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::x;
        break;
    case 'X':
        this->m_newLineCopy = false;
        m_visualBlockCopy = false;
        this->m_listCopiedStr.clear();
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::X;
        break;
    case 'd': /*~~~~~~~ d[...] ~~~~~~~~*/
        possible_command = true;
        if(m_externalCommand == 'f') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::df;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 'F') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::dF;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 't') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::dt;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 'T') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::dT;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                command_complete = true;
            }
            break;
        }
        switch(m_actionCommand) {
        case '\0':
            if(m_currentModus == VIM_MODI::VISUAL_MODUS || m_currentModus == VIM_MODI::VISUAL_LINE_MODUS ||
               m_currentModus == VIM_MODI::VISUAL_BLOCK_MODUS) {
                command_complete = true;
                m_commandID = COMMANDVI::d;
                this->m_listCopiedStr.clear();
            } else {
                command_complete = false;
            }
            break;
        case '0':
            if(m_actions != 0)
                break;
            command_complete = true;
            m_commandID = COMMANDVI::d0;
            this->m_listCopiedStr.clear();
            break;
        case '^':
            command_complete = true;
            m_commandID = COMMANDVI::d_caret;
            this->m_listCopiedStr.clear();
            break;
        case '$':
            command_complete = true;
            m_commandID = COMMANDVI::d$;
            this->m_listCopiedStr.clear();
            break;
        case 'w':
            if(m_externalCommand == 'a') {
                command_complete = true;
                m_commandID = COMMANDVI::daw;
                this->m_listCopiedStr.clear();
                break;
            } else if(m_externalCommand == 'i') {
                command_complete = true;
                m_commandID = COMMANDVI::diw;
                this->m_listCopiedStr.clear();
                break;
            }
            command_complete = true;
            m_commandID = COMMANDVI::dw;
            this->m_listCopiedStr.clear();
            break;
        case 'd':
            command_complete = true;
            m_commandID = COMMANDVI::dd;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            m_visualBlockCopy = false;
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
        case 'G':
            command_complete = true;
            m_commandID = COMMANDVI::dG;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            break;
        case 'g':
            if(m_externalCommand != 'g')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::dgg;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            break;
        case '\"':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::di_quot;
            this->m_listCopiedStr.clear();
            break;
        case '\'':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::di_apos;
            this->m_listCopiedStr.clear();
            break;
        case '{':
        case '}':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::di_curly;
            this->m_listCopiedStr.clear();
            break;
        case '[':
        case ']':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::di_square;
            this->m_listCopiedStr.clear();
            break;
        case '(':
        case ')':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::di_pare;
            this->m_listCopiedStr.clear();
            break;
        case '<':
        case '>':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::di_lt;
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
        possible_command = true;
        if(m_externalCommand == 'f') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::yf;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 'F') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::yF;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 't') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::yt;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                command_complete = true;
            }
            break;
        } else if(m_externalCommand == 'T') {
            possible_command = true;
            command_complete = false;
            m_commandID = COMMANDVI::yT;
            if(this->m_actionCommand != '\0') {
                this->m_listCopiedStr.clear();
                command_complete = true;
            }
            break;
        }
        switch(m_actionCommand) {
        case '\0':
            if(m_currentModus == VIM_MODI::VISUAL_MODUS || m_currentModus == VIM_MODI::VISUAL_LINE_MODUS ||
               m_currentModus == VIM_MODI::VISUAL_BLOCK_MODUS) {
                command_complete = true;
                m_commandID = COMMANDVI::y;
                this->m_listCopiedStr.clear();
            } else {
                command_complete = false;
            }
            break;
        case 'w':
            if(m_externalCommand == 'a') {
                command_complete = true;
                m_commandID = COMMANDVI::yaw;
                this->m_listCopiedStr.clear();
                break;
            } else if(m_externalCommand == 'i') {
                command_complete = true;
                m_commandID = COMMANDVI::yiw;
                this->m_listCopiedStr.clear();
                break;
            }
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
            m_visualBlockCopy = false;
            break;
        case 'e':
            command_complete = true;
            m_commandID = COMMANDVI::ye;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            m_visualBlockCopy = false;
            break;
        case '0':
            if(m_actions != 0)
                break;
            command_complete = true;
            m_commandID = COMMANDVI::y0;
            this->m_listCopiedStr.clear();
            break;
        case '^':
            command_complete = true;
            m_commandID = COMMANDVI::y_caret;
            this->m_listCopiedStr.clear();
            break;
        case '$':
            command_complete = true;
            m_commandID = COMMANDVI::y$;
            this->m_listCopiedStr.clear();
            break;
        case 'G':
            command_complete = true;
            m_commandID = COMMANDVI::yG;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            break;
        case 'g':
            if(m_externalCommand != 'g')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::ygg;
            this->m_listCopiedStr.clear();
            this->m_newLineCopy = true;
            break;
        case '\"':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::yi_quot;
            this->m_listCopiedStr.clear();
            break;
        case '\'':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::yi_apos;
            this->m_listCopiedStr.clear();
            break;
        case '{':
        case '}':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::yi_curly;
            this->m_listCopiedStr.clear();
            break;
        case '[':
        case ']':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::yi_square;
            this->m_listCopiedStr.clear();
            break;
        case '(':
        case ')':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::yi_pare;
            this->m_listCopiedStr.clear();
            break;
        case '<':
        case '>':
            if(m_externalCommand != 'i')
                break;
            command_complete = true;
            m_commandID = COMMANDVI::yi_lt;
            this->m_listCopiedStr.clear();
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
    case '*':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::diesis_N;
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

    case ';':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::semicolon;
        break;

    case ',':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::comma;
        break;
    /*=============== Paste ======================*/
    case 'p':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::p;
        break;
    case 'P':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::P;
        break;
    case 'J':
        possible_command = true;
        command_complete = true;
        m_commandID = COMMANDVI::J;
        break;
    case '\0':
        possible_command = true;
        break;
    case '~':
        m_commandID = COMMANDVI::tilde;
        possible_command = true;
        command_complete = true;
        break;
    default:
        break;
    }

    if(!possible_command) {
        command_complete = true;
        m_commandID = COMMANDVI::NO_COMMAND;
    }

    if(currentModus == VIM_MODI::VISUAL_BLOCK_MODUS && m_currentModus != VIM_MODI::VISUAL_BLOCK_MODUS) {
        m_ctrl->SetIndicatorCurrent(VISUAL_BLOCK_INDICATOR);
        m_ctrl->IndicatorClearRange(0, m_ctrl->GetLength());
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
    if(m_ctrl == NULL)
        return;
    m_ctrl->BeginUndoAction();
    for(int i = 0; i < this->getNumRepeat(); ++i) {
        if(!this->Command_call())
            break; /*If the num repeat is internally implemented do not repeat!*/
    }
    m_ctrl->EndUndoAction();
}

void VimCommand::RepeatIssueCommand(wxString buf)
{
    if(m_ctrl == NULL)
        return;

    m_ctrl->BeginUndoAction();
    for(int i = 0; i < this->getNumRepeat(); ++i) {
        if(!this->Command_call())
            break;
    }

    if(m_currentModus == VIM_MODI::INSERT_MODUS) {
        /*FIXME*/
        m_ctrl->AddText(buf);
    }
    m_ctrl->EndUndoAction();
}

void VimCommand::set_ctrl(wxStyledTextCtrl* ctrl)
{
    m_ctrl = ctrl;
    if(m_ctrl) {
        m_ctrl->IndicatorSetUnder(VISUAL_BLOCK_INDICATOR, false);
        m_ctrl->IndicatorSetForeground(VISUAL_BLOCK_INDICATOR, "RED");
        m_ctrl->IndicatorSetAlpha(VISUAL_BLOCK_INDICATOR, 150);
        m_ctrl->IndicatorSetStyle(VISUAL_BLOCK_INDICATOR, wxSTC_INDIC_BOX);
    }
}

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
        '(',
        ')',
        '[',
        ']',
        '{',
        '}',
        /*NOT VIM STANDARD, but useful*/
        '<',
        '>',
        '\"',
        '\"',
    };
    SEARCH_DIRECTION direction;
    long pos = start_pos;
    long max_n_char = m_ctrl->GetTextLength();
    wxChar currChar = m_ctrl->GetCharAt(pos);

    int index_parentesis;
    int increment = 0;
    bool found = false;

    for(index_parentesis = 0; index_parentesis < sizeof(parentesis) / sizeof(parentesis[0]); ++index_parentesis) {
        if(currChar == parentesis[index_parentesis]) {
            found = true;
            break;
        }
    }

    if(!found)
        return -1;

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

bool VimCommand::findMatchingParentesis(wxChar lch, wxChar rch, long minPos, long maxPos, long& leftPos, long& rightPos)
{
    long curPos = m_ctrl->GetCurrentPos();
    int level = 1;
    leftPos = -1;
    rightPos = -1;
    for(long i = curPos; i >= minPos; --i) {
        if(m_ctrl->GetCharAt(i) == lch) {
            level--;
        } else if(m_ctrl->GetCharAt(i) == rch) {
            level++;
        }
        if(level == 0) {
            leftPos = i;
            break;
        }
    }
    level = 1;
    for(long i = std::max(curPos, leftPos + 1); i < maxPos; ++i) {
        if(m_ctrl->GetCharAt(i) == rch) {
            level--;
        } else if(m_ctrl->GetCharAt(i) == lch) {
            level++;
        }
        if(level == 0) {
            rightPos = i;
            break;
        }
    }
    return leftPos < rightPos && leftPos != -1;
}

long VimCommand::findCharInLine(wxChar key, long setup, bool posPrev, bool reFind)
{
    long i = setup + ((posPrev && reFind) ? setup : 0);
    long line_start_pos = m_ctrl->PositionFromLine(m_ctrl->GetCurrentLine());
    long line_end_pos = m_ctrl->GetLineEndPosition(m_ctrl->GetCurrentLine());
    long pos = m_ctrl->GetCurrentPos();
    while(pos + i >= line_start_pos && pos + i <= line_end_pos) {
        if(m_ctrl->GetCharAt(pos + i) == key) {
            return pos + i + ((posPrev) ? -setup : 0);
        }
        i += setup;
    }
    return -1;
}

long VimCommand::findNextCharPos(int line, int col)
{
    int i = 1;
    int indent = m_ctrl->GetIndent();
    int begin_pos = m_ctrl->FindColumn(line, col);
    int pos = m_ctrl->FindColumn(line, col + i);
    int line_end_col = m_ctrl->GetColumn(m_ctrl->GetLineEndPosition(line));
    while(begin_pos == pos && i < indent && col + i < line_end_col) {
        i++;
        pos = m_ctrl->FindColumn(line, col + i);
    }
    return pos;
}

long VimCommand::findPrevCharPos(int line, int col)
{
    int i = 1;
    int indent = m_ctrl->GetIndent();
    int begin_pos = m_ctrl->FindColumn(line, col);
    int pos = m_ctrl->FindColumn(line, col - i);
    while(begin_pos == pos && i < indent && col - i > 0) {
        i++;
        pos = m_ctrl->FindColumn(line, col - i);
    }
    return pos;
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
    , m_externalCommand('\0')
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
    , m_externalCommand(command.m_externalCommand)
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
    m_externalCommand = command.m_externalCommand;
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
    m_externalCommand = command.m_externalCommand;
    command.m_actions = m_actions;
    command.m_repeatCommand = m_repeatCommand;
    command.m_modifierKey = m_modifierKey;
}
