//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : buidltab.h              
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
#ifndef __errorstab__
#define __errorstab__

#include <wx/string.h>
#include <wx/filename.h>
#include "map"
#include "outputtabwindow.h"
#include "compiler.h"

// Show build errors and warnings
class ErrorsTab : public OutputTabWindow {
public:
	enum _TYPE {
		TYPE_ERROR   = 1,
		TYPE_WARNING = 2
	};
	
private:
	int m_nextBuildError_lastLine;
	int m_errorCount;
	int m_warningCount;
	int m_IncludeResults;
	
	struct LineInfo {
		wxString project;
		wxString configuration;
		wxString compilerOutput;
		wxString fileName;
		wxString lineNo;
		_TYPE     type;
	};
	std::map<int, LineInfo> m_lineInfo;	

private:
	CompilerPtr GetCompilerByLine(int lineClicked);

	bool DoTryOpenFile(const wxArrayString& files, const wxFileName &fn, int lineNumber, int lineClicked);
	bool DoOpenFile(const wxFileName &fn, int lineNumber, int lineClicked);
	
	void Initialize();
	
protected:
	bool OnBuildWindowDClick(int lineClicked);
	void OnNextBuildError(wxCommandEvent &event);
	void OnMouseDClick(wxScintillaEvent &event);
	void OnHotspotClicked(wxScintillaEvent &event);
	
public:
	ErrorsTab(wxWindow *parent, wxWindowID id, const wxString &name);
	virtual ~ErrorsTab();
	
	virtual void AppendText(const wxString &text);
	virtual void Clear();
	
	int GetViewType();
	// Which results to show (see TYPE enum)
	void SetViewType(int type);
	
	void UpdateView();

	DECLARE_EVENT_TABLE()
};

#endif // __errorstab__
