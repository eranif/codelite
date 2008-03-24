#ifndef __svniconrefreshhandler__
#define __svniconrefreshhandler__

#include "wx/arrstr.h"
#include "wx/treectrl.h"
#include "svnxmlparser.h"
#include "svnpostcmdaction.h"

class IManager;
class SubversionPlugin;

class SvnIconRefreshHandler : public SvnPostCmdAction {
	
	IManager *m_mgr;
	SubversionPlugin *m_plugin;
	
protected:
	virtual void ColourTree(wxTreeCtrl *tree, wxTreeItemId &parent, const wxArrayString &modifiedPaths, const wxArrayString &conflictedPaths);
	virtual void ColourPath(wxTreeCtrl *tree, wxTreeItemId &item, const wxString &fileName, const wxArrayString &modifiedPaths, const wxArrayString &conflictedPaths);
	virtual int  GetIcon(int kind, SvnXmlParser::FileState state);
	virtual void DoColourPath(wxTreeCtrl *tree, wxTreeItemId &item, SvnXmlParser::FileState state);
	virtual void ResetIcons(wxTreeCtrl *tree, wxTreeItemId &item);
	void UpdateIcons();
	
public:
	SvnIconRefreshHandler(IManager *mgr, SubversionPlugin *plugin);
	virtual ~SvnIconRefreshHandler();
	virtual void DoCommand();
	
};
#endif // __svniconrefreshhandler__
