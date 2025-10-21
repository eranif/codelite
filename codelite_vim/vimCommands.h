#ifndef __VIM_COMMANDS__
#define __VIM_COMMANDS__

#include "ieditor.h"
#include "imanager.h"

#include <vector>
#include <wx/stc/stc.h>
// #include <wx/chartype.h>

#define VISUAL_BLOCK_INDICATOR 13

enum class COMMAND_PART {
    REPEAT_NUM,
    FIRST_CMD,
    MOD_NUM, /*MOD == MODIFIER*/
    MOD_CMD,
    REPLACING
};

enum class COMMAND_TYPE { MOVE, REPLACE, INSERT, CHANGE_MODUS, CHANGE_INS };

enum class VIM_MODI {
    NORMAL_MODUS,
    INSERT_MODUS,
    VISUAL_MODUS,
    VISUAL_LINE_MODUS,
    VISUAL_BLOCK_MODUS,
    COMMAND_MODUS,
    SEARCH_MODUS,
    SEARCH_CURR_MODUS,
    ISSUE_CMD,
    REPLACING_MODUS
};

enum class SEARCH_DIRECTION { BACKWARD, FORWARD };

enum class MESSAGES_VIM {
    NO_ERROR_VIM_MSG = 0,
    UNBALNCED_PARENTESIS_VIM_MSG,
    SAVED_VIM_MSG,
    SAVE_AND_CLOSE_VIM_MSG,
    CLOSED_VIM_MSG,
    SEARCHING_WORD
};

/*enumeration of implemented commands*/
enum class COMMANDVI {
    NO_COMMAND,
    j,
    k,
    h,
    l,
    H,
    M,
    L,
    _0,
    _$,
    _V, /*^*/
    w,
    W,
    b,
    B,
    e,
    E, /*FIXME E works/does not work*/
    f,
    F,
    t,
    T,
    semicolon,
    comma,
    G,
    gg,
    i,
    I,
    block_I,
    a,
    A,
    block_A,
    block_c,
    o,
    O,
    perc,
    u,
    r,
    R,
    cw,
    cb,
    ce,
    c0,
    c_caret, /*c^*/
    c$,
    caw,
    ciw,
    cf,
    cF,
    ct,
    cT,
    ci_quot,   /*ci"*/
    ci_apos,   /*ci'*/
    ci_pare,   /*ci(*/
    ci_square, /*ci[*/
    ci_lt,     /*ci<*/
    ci_curly,  /*ci{*/
    C,
    cc,
    s,
    S,
    x,
    X,
    d, /*Visual modeyy delete*/
    dw,
    dd,
    db,
    de,
    d0,
    d$,
    d_caret, /*d^*/
    daw,
    diw,
    df,
    dF,
    dt,
    dT,
    di_quot,   /*di"*/
    di_apos,   /*di'*/
    di_pare,   /*di(*/
    di_square, /*di[*/
    di_lt,     /*di<*/
    di_curly,  /*di{*/
    dG,
    dgg,
    D,
    diesis,
    diesis_N,
    N,
    n,
    slesh,
    repeat,
    ctrl_R,
    ctrl_U,
    ctrl_D, /*One has to 'disattivate' the default behavior of Ctrl+D/U*/
    p,
    P,
    y, /*visual mode*/
    yy,
    yw,
    yb,
    ye,
    y0,
    y_caret, /*y^*/
    y$,
    yaw,
    yiw,
    yf,
    yF,
    yt,
    yT,
    yi_quot,   /*yi"*/
    yi_apos,   /*yi'*/
    yi_pare,   /*yi(*/
    yi_square, /*yi[*/
    yi_lt,     /*yi<*/
    yi_curly,  /*yi{*/
    yG,
    ygg,
    J,
    v,
    ctrl_V,
    V,
    tilde
};

/**
 * @brief class that implements the actual vim key-bindings
 *
 * The general structure of a vim command is hier schematized:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         [start command] ......  bool inserting_num_repeat
                   |
                           |
                   ----------
     <-[Y] | number |-----
        |	   ----------    |                                      bool issue_cmd
        |                    |                                                        /
        -------------->  ['char' ] ... bool inserting_cmd_base  /__  bool mod_num_cmd
                        |                                                          \
                        |                                   \   bool mod_cmd
                 -------------\
                |              |
                |              |
                |          --------|
                |         /        |
                |        |        -----
                |        |       | num |---|
                |     ['char'] <-----------|
                |      /
                |     /
         [ issue_cmd ]  ............................... bool issue_command
                                                        bool save_buf

 */
class VimCommand;

class VimBaseCommand
{
public:
    VimBaseCommand(wxString fullpath_name);
    VimBaseCommand(const VimBaseCommand& command);
    ~VimBaseCommand() = default;

    bool isCurrentEditor(const wxString& fullpath_name);
    void saveCurrentStatus(const VimCommand& command);
    void setSavedStatus(VimCommand& command);

private:
    wxString m_fullpath_name;

    /*~~~~~~~~ INFO ~~~~~~~~~*/
    COMMANDVI m_commandID;             /*!< id of the current command to identify it*/
    COMMAND_PART m_currentCommandPart; /*!< current part of the command */
    VIM_MODI m_currentModus;           /*!< actual mode the editor is in */
    bool m_saveCommand;
    /*~~~~~~~~ COMMAND ~~~~~~~~~*/
    int m_repeat;           /*!< number of repetition for the command */
    wxChar m_baseCommand;   /*!< base command (first char of the cmd)*/
    wxChar m_actionCommand; /*!< eventual command modifier.In 'c3w', "w" */
    wxChar m_externalCommand;
    int m_actions; /*!< repetition of the modifier.In 'c3x', "3" */

    /*~~~~~~~~ HELPER ~~~~~~~~~*/
    bool m_repeatCommand;
    int m_modifierKey; /*!< to take into account for some commands*/
};

class VimCommand
{
public:
    enum eAction {
        kNone = -1,
        kClose,
        kSave,
        kSaveAndClose,
        kSearch,
    };

    enum eTypeTextSearch {
        kAllWord,
        kFromPosToEndWord,
        kFromPosToBeginWord,
        kFromPositionToEndLine,
        kFromPositionToBeginLine
    };

public:
    VimCommand(IManager* m_mgr);
    ~VimCommand() = default;

    /*~~~~~~~ EVENT HANLDER ~~~~~~~~~~~~~~~~*/
    bool OnNewKeyDown(wxChar ch, int modifier);
    bool OnEscapeDown();
    bool OnReturnDown(VimCommand::eAction& action);

    /**
     * @brief delete last char from the command buffer
     * @return true on success, false otherwise
     */
    bool DeleteLastCommandChar();
    MESSAGES_VIM getError();

    /*~~~~~~~ GETTER ~~~~~~~~~~~~~~~~*/
    VIM_MODI get_current_modus();
    wxString getTmpBuf();
    bool repeat_last_cmd();
    bool save_current_cmd();

    /*~~~~~~~ Commands-related ~~~~~~~~~~~~~~~~*/
    void IssueCommand();
    void RepeatIssueCommand(wxString buf);
    bool Command_call();
    bool Command_call_visual_mode();
    bool command_call_visual_line_mode();
    bool command_call_visual_block_mode();
    bool command_move_cmd_call(bool& repeat_command);
    bool is_cmd_complete();
    void set_current_word(wxString word);
    void set_current_modus(VIM_MODI modus);
    void reset_repeat_last();
    void ResetCommand();
    wxString getSearchedWord();
    void set_ctrl(wxStyledTextCtrl* ctrl);

private:
    /*~~~~~~~~ PRIVATE METHODS ~~~~~~~~~*/
    int getNumRepeat();
    void evidentiate_word();
    wxString get_text_at_position(VimCommand::eTypeTextSearch typeSearch = VimCommand::eTypeTextSearch::kAllWord);
    bool is_space_following();
    wxString add_following_spaces();
    bool search_word(SEARCH_DIRECTION direction, int flag = 0);
    bool search_word(SEARCH_DIRECTION direction, int flag, long pos);
    long goToMatchingParenthesis(long start_pos);
    bool findMatchingParenthesis(wxChar lch, wxChar rch, long minPos, long maxPos, long& leftPos, long& rightPos);
    long findCharInLine(wxChar key, long setup = 1, bool posPrev = false, bool reFind = false);
    long findNextCharPos(int line, int col);
    long findPrevCharPos(int line, int col);
    void normal_modus(wxChar ch);
    void visual_modus(wxChar ch);
    void command_modus(wxChar ch);
    void insert_modus(wxChar ch);
    void parse_cmd_string();
    void completing_command(wxChar ch);
    /*~~~~~~~~ INFO ~~~~~~~~~*/
    COMMANDVI m_commandID{COMMANDVI::NO_COMMAND}; /*!< id of the current command to identify it*/
    MESSAGES_VIM m_message_ID{MESSAGES_VIM::NO_ERROR_VIM_MSG};
    COMMAND_PART m_currentCommandPart{COMMAND_PART::REPEAT_NUM}; /*!< current part of the command */
    VIM_MODI m_currentModus{VIM_MODI::NORMAL_MODUS};             /*!< actual mode the editor is in */
    bool m_saveCommand{true};
    int m_initialVisualPos{0};  /*!< initial position of cursor when changing to visual mode*/
    int m_initialVisualLine{0}; /*!< initial line which cursor is on when changing to visual line mode*/

    /* visual block insert */
    int m_visualBlockBeginLine{0};
    int m_visualBlockEndLine{0};
    int m_visualBlockBeginCol{0};
    int m_visualBlockEndCol{0};

    /*~~~~~~~~ COMMAND ~~~~~~~~~*/
    int m_repeat{0};              /*!< number of repetition for the command */
    wxChar m_baseCommand{'\0'};   /*!< base command (first char of the cmd)*/
    wxChar m_actionCommand{'\0'}; /*!< eventual command modifier.In 'c3w', "w" */
    wxChar m_externalCommand{'\0'};
    int m_actions{0}; /*!< repetition of the modifier.In 'c3x', "3" */

    /*~~~~~~~~ HELPER ~~~~~~~~~*/
    bool m_repeatCommand{false};
    int m_modifierKey{0}; /*!< to take into account for some commands*/
    wxString m_tmpbuf;
    wxString m_searchWord;
    bool m_newLineCopy{false}; /*!< take track if we copy/pase the complete line (dd,yy)*/
    bool m_visualBlockCopy{false};
    std::vector<wxString> m_listCopiedStr;
    wxChar m_findKey{'\0'};
    long m_findStep{1};
    bool m_findPosPrev{false};

    /*~~~~~~~ EDITOR ~~~~~~~~~*/
    wxStyledTextCtrl* m_ctrl{nullptr};
    IManager* m_mgr{nullptr};
    friend VimBaseCommand;
};

#endif //__VIM_COMMANDS__
