#include "workspace_pane.h"
#include "fileview.h"
#include "cpp_symbol_tree.h"
#include <wx/xrc/xmlres.h>
#include "frame.h"
#include "checkdirtreectrl.h"
#include "windowstack.h"
#include "openwindowspanel.h"
#include "macros.h"
#include "fileexplorer.h"
#include "workspacetab.h"

const wxString WorkspacePane::SYMBOL_VIEW 	= wxT("Outline");
const wxString WorkspacePane::FILE_VIEW   	= wxT("Workspace");
const wxString WorkspacePane::OPEN_FILES  	= wxT("Tabs");
const wxString WorkspacePane::EXPLORER  	= wxT("Explorer");

extern wxImageList* CreateSymbolTreeImages();

WorkspacePane::WorkspacePane(wxWindow *parent, const wxString &caption)
		: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(250, 400))
		, m_caption(caption)
{
	CreateGUIControls();
}


WorkspacePane::~WorkspacePane()
{

}

int WorkspacePane::CaptionToIndex(const wxString &caption)
{
	int i = 0;
	for (; i<m_book->GetPageCount(); i++) {
		if (m_book->GetPageText((size_t)i) == caption)
			return i;
	}
	return wxNOT_FOUND;
}

void WorkspacePane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	long style = wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_FF2 | wxFNB_BACKGROUND_GRADIENT | wxFNB_CUSTOM_DLG | wxFNB_TABS_BORDER_SIMPLE;
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	
	m_book->SetCustomizeOptions(wxFNB_CUSTOM_LOCAL_DRAG | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_TAB_LOOK);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 1);

	// Add the class view tree
	m_winStack = new WindowStack(m_book, wxID_ANY);
	m_book->AddPage(m_winStack, WorkspacePane::SYMBOL_VIEW, false);
	
	m_workspaceTab = new WorkspaceTab(this);
	m_book->AddPage(m_workspaceTab, WorkspacePane::FILE_VIEW, true);

	m_explorer = new FileExplorer(m_book, wxT("Explorer"));
	m_book->AddPage(m_explorer, WorkspacePane::EXPLORER, false);

	m_openWindowsPane = new OpenWindowsPanel(m_book);
	m_book->AddPage(m_openWindowsPane, WorkspacePane::OPEN_FILES, false);
}

CppSymbolTree *WorkspacePane::GetTreeByFilename(const wxFileName &filename)
{
	wxWindow *win = m_winStack->Find(filename.GetFullPath());
	if (win) {
		return dynamic_cast<CppSymbolTree *>(win);
	}
	return NULL;
}

void WorkspacePane::BuildSymbolTree(const wxFileName &filename)
{
	CppSymbolTree *tree = GetTreeByFilename(filename);
	if (!tree) {
		tree = new CppSymbolTree(m_winStack, wxID_ANY);
		tree->SetSymbolsImages(CreateSymbolTreeImages());
		m_winStack->Add(tree, filename.GetFullPath());
		m_winStack->Select(filename.GetFullPath());
	}
	tree->BuildTree(filename);
}

void WorkspacePane::BuildFileTree()
{
	m_workspaceTab->BuildFileTree();
}

SymbolTree *WorkspacePane::GetSymbolTree()
{
	int id = Frame::Get()->GetNotebook()->GetSelection();
	if (id != wxNOT_FOUND) {
		LEditor *editor = dynamic_cast<LEditor*>( Frame::Get()->GetNotebook()->GetPage((size_t)id));
		if (editor) {
			return GetTreeByFilename(editor->GetFileName());
		}
	}
	return NULL;
}

void WorkspacePane::DisplaySymbolTree(const wxFileName &filename)
{
	m_winStack->Select(filename.GetFullPath());
}

void WorkspacePane::DeleteSymbolTree(const wxFileName &filename)
{
	m_winStack->Delete(filename.GetFullPath());
}

void WorkspacePane::DeleteAllSymbolTrees()
{
	m_winStack->Clear();
}

FileViewTree* WorkspacePane::GetFileViewTree()
{
	return m_workspaceTab->GetFileView();
}

wxComboBox* WorkspacePane::GetConfigCombBox()
{
	return m_workspaceTab->GetComboBox();
}
