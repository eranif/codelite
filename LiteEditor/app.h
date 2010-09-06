//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : app.h
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
#ifndef LITEEDITOR_APP_H
#define LITEEDITOR_APP_H

#include "frame.h"
class wxSplashScreen;
class wxSingleInstanceChecker;

class CodeLiteApp : public wxApp
{
	wxSplashScreen*          m_splash;
	clMainFrame *                  m_pMainFrame;
	wxSingleInstanceChecker *m_singleInstance;
	wxArrayString            m_parserPaths;
	bool                     m_loadPlugins;

private: // Methods
	bool     CopySettings(const wxString &destDir, wxString& installPath);
	bool     CheckSingularity(const wxCmdLineParser &parser, const wxString &curdir);
	void     MSWReadRegistry();
	wxString DoFindMenuFile(const wxString& installDirectory);

#ifdef __WXMSW__
	HINSTANCE m_handler;
#endif

public:
	CodeLiteApp(void);
	virtual ~CodeLiteApp(void);

	void SetParserPaths(const wxArrayString& parserPaths) {
		this->m_parserPaths = parserPaths;
	}
	const wxArrayString& GetParserPaths() const {
		return m_parserPaths;
	}
	void SetLoadPlugins(bool loadPlugins) {
		this->m_loadPlugins = loadPlugins;
	}
	bool GetLoadPlugins() const {
		return m_loadPlugins;
	}
	void MacOpenFile(const wxString &fileName);

protected:
	virtual bool OnInit();
	virtual int OnExit();
	virtual void OnFatalException();

private:
};

#endif // LITEEDITOR_APP_H
