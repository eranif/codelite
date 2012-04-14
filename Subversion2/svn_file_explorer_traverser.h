#ifndef SVNFILEEXPLORERTRAVERSER_H
#define SVNFILEEXPLORERTRAVERSER_H

#include "precompiled_header.h"
#include "wx_tree_traverser.h"

struct SvnFileInfo
{
	wxString file;
	int      type;
};

class SvnFileExplorerTraverser : public wxTreeTraverser
{
public:
	enum {
		Ok = 0, // We start from 1 - this is important, do not change it
		Modified , 
		Locked,
		Unversioned,
		Conflicted,
		Deleted,
		New,
		Ignored
	};
	
	typedef std::map<wxString, SvnFileInfo> Map_t;
	typedef std::map<wxString, bool>        MapPaths_t;
	
protected:
	Map_t      m_files;
	size_t     m_imgCount;
	MapPaths_t m_svnPaths;
	
protected:
	bool IsPathUnderSvn(const wxString& path, bool isDir);
	
public:
	SvnFileExplorerTraverser(wxTreeCtrl *tree, const Map_t &files, size_t imgCount);
	virtual ~SvnFileExplorerTraverser();

	void OnItem(const wxTreeItemId& item);

};

#endif // SVNFILEEXPLORERTRAVERSER_H
