#ifndef WORKSPACE_PANE_H
#define WORKSPACE_PANE_H

#include "wx/panel.h"
#include "tag_tree.h"
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "wx/filename.h"

class FileViewTree;
class SymbolTree;
class CppSymbolTree;
class WindowStack;
class OpenWindowsPanel;
class wxComboBox;
class FileExplorer;
class WorkspaceTab;
class wxVerticalBook;

class WorkspacePane : public wxPanel 
{
public:
	static const wxString SYMBOL_VIEW;
	static const wxString FILE_VIEW;
	static const wxString OPEN_FILES;
	static const wxString EXPLORER;
	
	wxVerticalBook *m_book;
	wxString m_caption;
	wxFlatNotebookImageList m_images;
	WindowStack *m_winStack;
	OpenWindowsPanel *m_openWindowsPane;
	FileExplorer *m_explorer;
	WorkspaceTab *m_workspaceTab;
	
private:
	void CreateGUIControls();
	CppSymbolTree *GetTreeByFilename(const wxFileName &filename);
	
public:
	WorkspacePane(wxWindow *parent, const wxString &caption);
	virtual ~WorkspacePane();
	
	//-----------------------------------------------
	// Operations
	//-----------------------------------------------
	void BuildSymbolTree(const wxFileName &filename);
	void BuildFileTree();
	void DisplaySymbolTree(const wxFileName &filename);
	void DeleteSymbolTree(const wxFileName &filename);
	void DeleteAllSymbolTrees();

	// Return the index of the given tab by name
	int CaptionToIndex(const wxString &caption);

	//-----------------------------------------------
	// Setters/Getters
	//-----------------------------------------------
	wxVerticalBook *GetNotebook() { return m_book; }
	SymbolTree *GetSymbolTree();
	FileViewTree *GetFileViewTree();
	OpenWindowsPanel *GetOpenedWindows() {return m_openWindowsPane;}
	const wxString &GetCaption() const {return m_caption;}
	wxComboBox *GetConfigCombBox();
	FileExplorer *GetFileExplorer(){return m_explorer;}
	
	DECLARE_EVENT_TABLE()
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnSize(wxSizeEvent &e);
	
};

#endif // WORKSPACE_PANE_H

