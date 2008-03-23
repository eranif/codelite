#ifndef __workspacetab__
#define __workspacetab__

#include <wx/panel.h>

class FileViewTree;
class wxComboBox;

class WorkspaceTab : public wxPanel {
	FileViewTree *m_fileView;
	wxComboBox *m_workspaceConfig;
	
protected:
	void OnCollapseAll(wxCommandEvent &event);
	void OnCollapseAllUI(wxUpdateUIEvent &event);
	
	void CreateGUIControls();
	
public:
	WorkspaceTab(wxWindow *parent);
	~WorkspaceTab();
	
	void BuildFileTree();
	FileViewTree *GetFileView(){return m_fileView;}
	wxComboBox *GetComboBox(){return m_workspaceConfig;}
	
};
#endif // __workspacetab__
