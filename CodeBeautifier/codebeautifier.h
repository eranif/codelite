//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : codebeautifier.h
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

#ifndef CODEBEAUTIFIER_H
#define CODEBEAUTIFIER_H

#include "plugin.h"

class CodeBeautifier : public IPlugin
{
protected:
	void DoFormatFile(IEditor *editor);
	wxString ExecuteUncrustify(const wxString &inputString, const wxString &ext);
	
public:
	CodeBeautifier(IManager *manager);
	virtual ~CodeBeautifier();
	
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void UnPlug();
	
	//event handlers
	void OnFormat(wxCommandEvent &e);
};

#endif //CODEBEAUTIFIER_H

