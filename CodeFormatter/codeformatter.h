//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : codeformatter.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #ifndef CODEFORMATTER_H
#define CODEFORMATTER_H

#include "plugin.h"

class CodeFormatter : public IPlugin
{
protected:
	void DoFormatFile(IEditor *editor);

public:
	CodeFormatter(IManager *manager);
	virtual ~CodeFormatter();
	void AstyleFormat(const wxString &input, const wxString &options, wxString &output);
	virtual wxToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
	//event handlers
	void OnFormat(wxCommandEvent &e);
	void OnFormatOptions(wxCommandEvent &e);
	void OnFormatUI(wxUpdateUIEvent &e);
	void OnFormatOptionsUI(wxUpdateUIEvent &e);
};

#endif //CODEFORMATTER_H


