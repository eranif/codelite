#ifndef CHECKDIRCTRL_H
#define CHECKDIRCTRL_H

#include "checktreectrl.h"
#include "wx/dir.h"
#include "wx/filename.h"

class DirTreeData : public wxTreeItemData
{
	wxFileName m_dirname;
public:
	DirTreeData(const wxFileName &dir) : m_dirname(dir){}
	virtual ~DirTreeData(){}

	const wxFileName& GetDir() const {return m_dirname;}
};


class CheckDirTreeCtrl : public wxCheckTreeCtrl 
{
	wxString m_root;
public:
	CheckDirTreeCtrl(wxWindow *parent, const wxString &rootPath, wxWindowID id = wxID_ANY);
	virtual ~CheckDirTreeCtrl();
	void GetUnselectedDirs(wxArrayString &arr);
	void GetSelectedDirs(wxArrayString &arr);
	void BuildTree(const wxString &rootPath);

	DECLARE_EVENT_TABLE();
	void OnItemExpading(wxTreeEvent &event);
	void OnItemUnchecked(wxCheckTreeCtrlEvent &event);
	void OnItemChecked(wxCheckTreeCtrlEvent &event);
	
private:
	void GetChildren(const wxTreeItemId &parent, const wxDir &dir, const wxString &path);
	void AddChildren(const wxTreeItemId &item);
};

#endif //CHECKDIRCTRL_H
