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

enum class MESSAGES_VIM {
    NO_ERROR_VIM_MSG = 0, 
    UNBALNCED_PARENTESIS_VIM_MSG,
    SAVED_VIM_MSG,
    SAVE_AND_CLOSE_VIM_MSG,
    CLOSED_VIM_MSG
};

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
    e,
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
	cb,
	ce,
    C,
    cc,
    S,
    x,
    d, /*Visual modeyy delete*/
    dw,
    dd,
    db,
	de,
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
    ye,
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

class VimBaseCommand
{
public:
    VimBaseCommand(wxString fullpath_name);
    VimBaseCommand(const VimBaseCommand& command);
    ~VimBaseCommand();

    bool isCurrentEditor(const wxString& fullpath_name);
    void saveCurrentStatus(const VimCommand &command);
    void setSavedStatus( VimCommand &command);
    
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
    int m_actions;          /*!< repetition of the modifier.In 'c3x', "3" */

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
    bool is_cmd_complete();
    void set_current_word(wxString word);
    void set_current_modus(VIM_MODI modus);
    void reset_repeat_last();
    void ResetCommand();
    void set_ctrl(wxStyledTextCtrl *ctrl);

private:
    /*~~~~~~~~ PRIVAT METHODS ~~~~~~~~~*/
    int getNumRepeat();
    int getNumActions();
    void evidentiate_word();
    void append_command(wxChar ch);
    wxString get_text_at_position(VimCommand::eTypeTextSearch typeSearch = VimCommand::eTypeTextSearch::kAllWord);
    bool is_space_following();
    bool is_space_preceding();
    wxString add_following_spaces();
    wxString add_preceding_spaces();
    bool search_word(SEARCH_DIRECTION flag);
    long goToMatchingParentesis(long start_pos);
    void normal_modus(wxChar ch);
    void visual_modus(wxChar ch);
    void command_modus(wxChar ch);
    void insert_modus(wxChar ch);

    /*~~~~~~~~ INFO ~~~~~~~~~*/
    COMMANDVI m_commandID;             /*!< id of the current command to identify it*/
    MESSAGES_VIM m_message_ID;
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

	/*~~~~~~~ EDITOR ~~~~~~~~~*/
	wxStyledTextCtrl *m_ctrl;
    friend VimBaseCommand;
};

#endif //__VIM_COMMANDS__
