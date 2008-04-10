#include "workspace_pane.h"
#include "custom_notebook.h"
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

BEGIN_EVENT_TABLE(WorkspacePane, wxPanel)
	EVT_PAINT(WorkspacePane::OnPaint)
	EVT_ERASE_BACKGROUND(WorkspacePane::OnEraseBg)
	EVT_SIZE(WorkspacePane::OnSize)
END_EVENT_TABLE()

WorkspacePane::WorkspacePane(wxWindow *parent, const wxString &caption)
		: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
		, m_caption(caption)
{
	CreateGUIControls();
}


WorkspacePane::~WorkspacePane()
{

}

int WorkspacePane::CaptionToIndex(const wxString &caption)
{
	size_t i = 0;
	for (; i<m_book->GetPageCount(); i++) {
		if (m_book->GetPageText(i) == caption)
			return i;
	}
	return wxNOT_FOUND;
}

void WorkspacePane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_LEFT|wxVB_TAB_DECORATION);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 1);

	m_workspaceTab = new WorkspaceTab(m_book);
	m_book->AddPage(m_workspaceTab, WorkspacePane::FILE_VIEW, wxNullBitmap, true);

	m_explorer = new FileExplorer(m_book, wxT("Explorer"));
	m_book->AddPage(m_explorer, WorkspacePane::EXPLORER, wxNullBitmap, false);
	
	m_winStack = new WindowStack(m_book, wxID_ANY); 
	m_book->AddPage(m_winStack, WorkspacePane::SYMBOL_VIEW, wxNullBitmap, false);

	m_openWindowsPane = new OpenWindowsPanel(m_book);
	m_book->AddPage(m_openWindowsPane, WorkspacePane::OPEN_FILES, wxNullBitmap, false);
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

void WorkspacePane::OnEraseBg(wxEraseEvent &e)
{
	wxUnusedVar(e);
}

void WorkspacePane::OnPaint(wxPaintEvent &e)
{
	wxUnusedVar(e);
	wxBufferedPaintDC dc(this);
	
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	
	dc.DrawRectangle(GetClientSize());
}

void WorkspacePane::OnSize(wxSizeEvent &e)
{
	Refresh();
	e.Skip();
}
void WorkspacePane::CollpaseAll()
{
	m_workspaceTab->CollpaseAll();
}
