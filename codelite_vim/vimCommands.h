#ifndef __VIM_COMMANDS__
#define __VIM_COMMANDS__

#include <wx/chartype.h>
#include <wx/stc/stc.h>
#include "ieditor.h"
#include "imanager.h"

enum class COMMAND_PART {
	REPEAT_NUM,
	FIRST_CMD,
	MOD_NUM, /*MOD == MODIFIER*/
	MOD_CMD,
	REPLACING	
};

enum class COMMAND_TYPE	{
	MOVE,
	REPLACE,
	INSERT,
	CHANGE_MODUS,
	CHANGE_INS	
};

enum class VIM_MODI {
	NORMAL_MODUS,
	INSERT_MODUS,
	VISUAL_MODUS,
	COMMAND_MODUS,
	SEARCH_MODUS,
	ISSUE_CMD,
	REPLACING_MODUS	
};

/*enumeration of implemented commands*/
enum class COMMANDVI {
	NO_COMMAND,
	j, k, h, l,
	_0, _$, w, b, G,
	i, I, a, A, o, O,
	u,
	r,R,cw,C,cc, S,
	x, dw, dd, D,
	diesis
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

class VimCommand
{
 public:
	VimCommand();
	~VimCommand();

	bool OnNewKeyDown(wxChar ch);
	bool OnEscapeDown();
	bool OnReturnDown(IEditor **mEditor, IManager* manager);
	void ResetCommand();
	int  getNumRepeat();
	int  getNumActions();
	wxString getTmpBuf();
	
	VIM_MODI get_current_modus();
	bool Command_call( wxStyledTextCtrl *ctrl);
	bool is_cmd_complete();
	
 private:

	void normal_modus( wxChar ch );
	void command_modus( wxChar ch );

	COMMANDVI    command_id;           /*!< id of the current command to identify it*/
	COMMAND_PART current_cmd_part;     /*!< current part of the command */ 
	VIM_MODI     current_modus;        /*!< actual mode the editor is in */
	int          mRepeat;              /*!< number of repetition for the command */
	wxChar       mBaseCmd;             /*!< base command (first char of the cmd)*/
	wxChar       mActionCmd;           /*!< eventual command modifier.In 'c3w', "w" */ 
	int          nActions;             /*!< repetition of the modifier.In 'c3x', "3" */
	int          nCumulativeUndo;      /*!< cumulative actions performed in the editor*/
	                                   /*in order to currectly do the undo!*/
	wxString     tmpBuf;
};

#endif //__VIM_COMMANDS__
