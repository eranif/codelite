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

MacrosDlg::MacrosDlg( wxWindow* parent, int content )
		: MacrosBaseDlg( parent )
		, m_item(wxNOT_FOUND)
		, m_content(content)
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

	switch ( m_content ) {
	case MacrosProject:
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
		break;
	case MacrosCompiler:
		AddMacro(wxT("$(CompilerName)"), wxT("Expands to the compiler name as set in the Tools tab"));
		AddMacro(wxT("$(SourceSwitch)"), wxT("Expands to the source switch (usually, -c)"));
		AddMacro(wxT("$(FileFullPath)"), wxT("The file full path (includes path+name+extnesion)"));
		AddMacro(wxT("$(FileFullName)"), wxT("The file full name (includes name+extnesion)"));
		AddMacro(wxT("$(FileName)"), wxT("The file name (includes name only)"));
		AddMacro(wxT("$(CmpOptions)"), wxT("Expands to the compiler options as set in the project settings"));
		AddMacro(wxT("$(RcCompilerName)"), wxT("Expands to the resource compiler name"));
		AddMacro(wxT("$(IntermediateDirectory)"), wxT("Expands to the current project intermediate directory path, as set in the project settings"));
		AddMacro(wxT("$(ConfigurationName)"), wxT("Expands to the current project selected configuration"));
		AddMacro(wxT("$(OutDir)"), wxT("An alias to $(IntermediateDirectory)"));
		AddMacro(wxT("$(LinkerName)"), wxT("Expands to the linker name as set in the Tools tab"));
		AddMacro(wxT("$(ArchiveTool)"), wxT("Expands to the archive tool (e.g. ar) name as set in the Tools tab"));
		AddMacro(wxT("$(SharedObjectLinkerName)"), wxT("Expands to the shared object linker name as set in the Tools tab"));
		AddMacro(wxT("$(ObjectSuffix)"), wxT("Objects suffix (usually set to .o)"));
		AddMacro(wxT("$(ObjectSuffix)"), wxT("Objects suffix (usually set to .o)"));
		AddMacro(wxT("$(IncludeSwitch)"), wxT("The compiler include switch"));
		AddMacro(wxT("$(LibrarySwitch)"), wxT("The library switch (e.g. -l)"));
		AddMacro(wxT("$(OutputSwitch)"), wxT("The output switch (e.g. -o)"));
		AddMacro(wxT("$(LibraryPathSwitch)"), wxT("Library switch (e.g. -L)"));
		AddMacro(wxT("$(PreprocessorSwitch)"), wxT("Preprocessor switch (e.g. -D)"));
		AddMacro(wxT("$(Preprocessors)"), wxT("Expands to all preprocessors set in the project setting where each entry is prefixed with $(PreprocessorSwitch)"));
		AddMacro(wxT("$(ArchiveOutputSwitch)"), wxT("Archive switch, usually not needed (VC compiler sets it to /OUT:"));
		AddMacro(wxT("$(LinkOptions)"), wxT("The linker options as set in the project settings"));
		AddMacro(wxT("$(IncludePath)"), wxT("All include paths prefixed with $(IncludeSwitch)"));
		AddMacro(wxT("$(RcIncludePath)"), wxT("Resource compiler include path as set in the project settings"));
		AddMacro(wxT("$(Libs)"), wxT("List of libraries to link with. each library is prefixed with $(LibrarySwitch)"));
		AddMacro(wxT("$(LibPath)"), wxT("List of library paths to link with. each library is prefixed with $(LibraryPathSwitch)"));
		AddMacro(wxT("$(OutputFile)"), wxT("The output file"));
		break;
	}

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
	if (m_item != wxNOT_FOUND) {
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
