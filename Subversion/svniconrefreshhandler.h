//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svniconrefreshhandler.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
 #ifndef __svniconrefreshhandler__
#define __svniconrefreshhandler__

#include "wx/arrstr.h"
#include "wx/treectrl.h"
#include "svnxmlparser.h"
#include "svnpostcmdaction.h"

class IManager;
class SubversionPlugin;

class SvnIconRefreshHandler : public SvnPostCmdAction {
	
protected:	
	IManager *m_mgr;
	SubversionPlugin *m_plugin;
	
protected:
	virtual void ColourTree(wxTreeCtrl *tree, wxTreeItemId &parent, const wxArrayString &modifiedPaths, const wxArrayString &conflictedPaths);
	virtual void ColourPath(wxTreeCtrl *tree, wxTreeItemId &item, const wxString &fileName, const wxArrayString &modifiedPaths, const wxArrayString &conflictedPaths);
	virtual int  GetIcon(int kind, const wxString &filename, SvnXmlParser::FileState state);
	virtual void DoColourPath(wxTreeCtrl *tree, wxTreeItemId &item, SvnXmlParser::FileState state);
	virtual void ResetIcons(wxTreeCtrl *tree, wxTreeItemId &item);
	void UpdateIcons();
	
public:
	SvnIconRefreshHandler(IManager *mgr, SubversionPlugin *plugin);
	virtual ~SvnIconRefreshHandler();
	virtual void DoCommand();
	
};
#endif // __svniconrefreshhandler__
