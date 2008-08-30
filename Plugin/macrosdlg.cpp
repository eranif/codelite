#include <wx/dataobj.h>
#include <wx/app.h>
#include <wx/clipbrd.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include "macrosdlg.h"
#include "globals.h"

BEGIN_EVENT_TABLE(MacrosDlg, MacrosBaseDlg)
EVT_MENU(XRCID("copy_macro"), MacrosDlg::OnCopy)
END_EVENT_TABLE()

MacrosDlg::MacrosDlg( wxWindow* parent )
		: MacrosBaseDlg( parent )
		, m_item(wxNOT_FOUND)
{
	Initialize();
	m_buttonOk->SetFocus();
	GetSizer()->SetMinSize(500, 400);
	GetSizer()->Fit(this);
}

void MacrosDlg::OnItemRightClick( wxListEvent& event )
{
	m_item = event.m_itemIndex;
	wxMenu menu;
	menu.Append(XRCID("copy_macro"), wxT("Copy"), false);
	PopupMenu(&menu);
}

void MacrosDlg::Initialize()
{
	m_listCtrlMacros->InsertColumn(0, wxT("Macro"));
	m_listCtrlMacros->InsertColumn(1, wxT("Description"));

	AddMacro(wxT("$(ProjectPath)"), wxT("Expands to project's path"));
	AddMacro(wxT("$(WorkspacePath)"), wxT("Expands to workspace's path"));
	AddMacro(wxT("$(ProjectName)"), wxT("Expands to the current project name as appears in the 'File View'"));
	AddMacro(wxT("$(IntermediateDirectory)"), wxT("Expands to the current project intermediate directory path, as set in the project settings"));
	AddMacro(wxT("$(ConfigurationName)"), wxT("Expands to the current project selected configuration"));
	AddMacro(wxT("$(OutDir)"), wxT("An alias to $(IntermediateDirectory)"));
	AddMacro(wxT("$(CurrentFileName)"), wxT("Expands to current file name (without extension and path)"));
	AddMacro(wxT("$(CurrentFilePath)"), wxT("Expands to current file path"));
	AddMacro(wxT("$(CurrentFileFullPath)"), wxT("Expands to current file full path (path and full name)"));
	AddMacro(wxT("$(User)"), wxT("Expands to logged-in user as defined by the OS"));
	AddMacro(wxT("$(Date)"), wxT("Expands to current date"));
	AddMacro(wxT("$(CodeLitePath)"), wxT("Expands to CodeLite's startup directory on (e.g. on Unix it exapnds to ~/.codelite/"));
	AddMacro(wxT("`expression`"), wxT("backticks: evaluates the expression inside the backticks into a string"));
	
	// resize columns
	m_listCtrlMacros->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_listCtrlMacros->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void MacrosDlg::AddMacro(const wxString& name, const wxString& desc)
{
	long row = AppendListCtrlRow(m_listCtrlMacros);
	SetColumnText(m_listCtrlMacros, row, 0, name);
	SetColumnText(m_listCtrlMacros, row, 1, desc);
}

MacrosDlg::~MacrosDlg()
{
}

void MacrosDlg::OnCopy(wxCommandEvent& e)
{
	if(m_item != wxNOT_FOUND){
		wxString value = GetColumnText(m_listCtrlMacros, m_item,  0);
#if wxUSE_CLIPBOARD
		if (wxTheClipboard->Open()) {
			wxTheClipboard->UsePrimarySelection(false);
			if (!wxTheClipboard->SetData(new wxTextDataObject(value))) {
				//wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
			}
			wxTheClipboard->Close();
		} else {
			wxPrintf(wxT("Failed to open the clipboard"));
		}
#endif
	}
	m_item = wxNOT_FOUND;
}
