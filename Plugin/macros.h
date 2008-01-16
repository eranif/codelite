#ifndef MACROS_H
#define MACROS_H

//-------------------------------------------------------
// wxWidgets Connect macros
//-------------------------------------------------------

#define ConnectChoice(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectButton(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListBoxDClick(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCheckBox(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListCtrlItemSelected(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(fn), NULL, this);

#define ConnectListCtrlItemActivated(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(fn), NULL, this);

#define ConnectKeyDown(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler(fn), NULL, this);

#define ConnectCharEvent(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_CHAR, wxKeyEventHandler(fn), NULL, this);

#define ConnectCmdTextEntered(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(fn), NULL, this);

#define ConnectCmdTextUpdated(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCombo(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCheckList(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(fn), NULL, this);

#define TrimString(str){\
		str = str.Trim();\
		str = str.Trim(false);\
	}

#define IsSourceFile(ext)\
	(ext == wxT("cpp") || ext == wxT("cxx") || ext == wxT("c") || ext == wxT("c++") || ext == wxT("cc"))

#define BoolToString(b) b ? wxT("yes") : wxT("no")

//-----------------------------------------------------
// Constants
//-----------------------------------------------------

const wxString clCMD_NEW = wxT("<New...>");
const wxString clCMD_EDIT = wxT("<Edit...>");
const wxString clCMD_DELETE = wxT("<Delete...>");

// constant message
const wxString BUILD_START_MSG     = wxT("Build Started...\n");
const wxString BUILD_END_MSG       = wxT("----------Build Ended----------\n");
const wxString SEARCH_IN_WORKSPACE = wxT("Entire Workspace");
const wxString SEARCH_IN_PROJECT   = wxT("Active Project");

#ifdef __WXMSW__
#define PATH_SEP wxT("\\")
#else
#define PATH_SEP wxT("/")
#endif

#define viewAsMenuItemID  23500
#define viewAsMenuItemMaxID  23500+100

#define viewAsSubMenuID 23499
#define RecentFilesSubMenuID 23600
#define RecentWorkspaceSubMenuID 23650

#endif // MACROS_H
