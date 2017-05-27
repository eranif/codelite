#ifndef __VIM_COMMANDS__
#define __VIM_COMMANDS__

#include <wx/chartype.h>
#include <wx/stc/stc.h>
#include "ieditor.h"
#include "imanager.h"
#include <vector>

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
    COMMAND_MODUS,
    SEARCH_MODUS,
    ISSUE_CMD,
    REPLACING_MODUS
};

enum class SEARCH_DIRECTION { BACKWARD, FORWARD };


/*enumeration of implemented commands*/
enum class COMMANDVI {
    NO_COMMAND,
    j,
    k,
    h,
    l,
    _0,
    _$,
    w,
    b,
    G,
    gg,
    i,
    I,
    a,
    A,
    o,
    O,
    perc,
    u,
    r,
    R,
    cw,
    C,
    cc,
    S,
    x,
    d, /*Visual modeyy delete*/
    dw,
    dd,
    db,
    D,
    diesis,
    N,
    n,
    slesh,
    repeat,
    ctrl_U,
    ctrl_D, /*One has to 'disattivate' the default behavior of Ctrl+D/U*/
    p,
    P,
    y, /*visual mode*/        
    yy,
    yw,
    yb,
    J,
    v
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

 * TODO:
 * - repeat command
 * - copy paste (y, dd, x ...)
 */
class VimCommand;

class VimCommand
{
public:
    enum eAction {
        kNone = -1,
        kClose,
        kSave,
        kSaveAndClose,
    };

    enum eTypeTextSearch {
       kAllWord,
       kFromPosToEndWord,
       kFromPosToBeginWord,
       kFromPositionToEndLine,
       kFromPositionToBeginLine
    };
    
public:
    VimCommand();
    ~VimCommand();

    /*~~~~~~~ EVENT HANLDER ~~~~~~~~~~~~~~~~*/
    bool OnNewKeyDown(wxChar ch, int modifier);
    bool OnEscapeDown(wxStyledTextCtrl* ctrl);
    bool OnReturnDown(IEditor* mEditor, IManager* manager, VimCommand::eAction& action);

    /**
     * @brief delete last char from the command buffer
     * @return true on success, false otherwise
     */
    bool DeleteLastCommandChar();

    /*~~~~~~~ GETTER ~~~~~~~~~~~~~~~~*/
    VIM_MODI get_current_modus();
    wxString getTmpBuf();
    bool repeat_last_cmd();
    bool save_current_cmd();

    /*~~~~~~~ Commands-related ~~~~~~~~~~~~~~~~*/
    void issue_cmd(wxStyledTextCtrl* ctrl);
    void repeat_issue_cmd(wxStyledTextCtrl* ctrl, wxString buf);
    bool Command_call(wxStyledTextCtrl* ctrl);
    bool Command_call_visual_mode(wxStyledTextCtrl* ctrl);
    bool is_cmd_complete();
    void set_current_word(wxString word);
    void set_current_modus(VIM_MODI modus);
    void reset_repeat_last();
    void ResetCommand();
    void set_ctrl(wxStyledTextCtrl* ctrl);

private:
    /*~~~~~~~~ PRIVAT METHODS ~~~~~~~~~*/
    int getNumRepeat();
    int getNumActions();
    void evidentiate_word(wxStyledTextCtrl* ctrl);
    void append_command(wxChar ch);
    wxString get_text_at_position(wxStyledTextCtrl* ctrl, VimCommand::eTypeTextSearch typeSearch = VimCommand::eTypeTextSearch::kAllWord);
    bool is_space_following(wxStyledTextCtrl* ctrl);
    bool is_space_preceding(wxStyledTextCtrl* ctrl);
    wxString add_following_spaces(wxStyledTextCtrl* ctrl);
    wxString add_preceding_spaces(wxStyledTextCtrl* ctrl);
    bool search_word(SEARCH_DIRECTION flag, wxStyledTextCtrl* ctrl);
    void normal_modus(wxChar ch);
    void visual_modus(wxChar ch);
    void command_modus(wxChar ch);
    void insert_modus(wxChar ch);

    /*~~~~~~~~ INFO ~~~~~~~~~*/
    COMMANDVI m_commandID;             /*!< id of the current command to identify it*/
    COMMAND_PART m_currentCommandPart; /*!< current part of the command */
    VIM_MODI m_currentModus;           /*!< actual mode the editor is in */
    bool m_saveCommand;
    /*~~~~~~~~ COMMAND ~~~~~~~~~*/
    int m_repeat;           /*!< number of repetition for the command */
    wxChar m_baseCommand;   /*!< base command (first char of the cmd)*/
    wxChar m_actionCommand; /*!< eventual command modifier.In 'c3w', "w" */
    int m_actions;          /*!< repetition of the modifier.In 'c3x', "3" */

    int m_cumulativeUndo; /*!< cumulative actions performed in the editor*/
                          /*in order to currectly do the undo!*/
    /*~~~~~~~~ HELPER ~~~~~~~~~*/
    bool m_repeatCommand;
    int m_modifierKey; /*!< to take into account for some commands*/
    wxString m_tmpbuf;
    wxString m_searchWord;
    bool m_newLineCopy; /*!< take track if we copy/pase the complete line (dd,yy)*/
    std::vector<wxString> m_listCopiedStr;
};

#endif //__VIM_COMMANDS__
