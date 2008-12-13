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
#ifndef __buidltab__
#define __buidltab__

#include <map>
#include <wx/filename.h>
#include <wx/stopwatch.h>

#include "outputtabwindow.h"
#include "compiler.h"
#include "buildtabsettingsdata.h"

class LEditor;

class BuildTab : protected OutputTabWindow 
{
    friend class ErrorsTab;
    
private:
    /**
     * Tracks an "important" line in the build log (error, warning, etc)
     */ 
    struct LineInfo {
        LineInfo() 
            : linenum(0), linecolor(0), filestart(0), filelen(0) 
            { }
        wxString project;
        wxString configuration;
        wxString filename;
        wxString linetext;
        long     linenum;
        int      linecolor;
        size_t   filestart; 
        size_t   filelen;
    };

	std::map<int, LineInfo> m_lineInfo;
    std::map<wxString, int> m_lineMap;
    
    int  m_showMe;
    bool m_autoHide;
	bool m_skipWarnings;
    int  m_errorCount;
    int  m_warnCount;
    
    CompilerPtr m_cmp; // compiler in use during currently building project
    wxStopWatch m_sw;  // times the entire build
	
    static BuildTab *s_bt; // self reference for ColorLine to access the m_line* maps
    
    static int        ColorLine(int, const char *text, size_t &start, size_t &len);
    static void       SetStyles(wxScintilla *sci, const BuildTabSettingsData &options);
    static bool       OpenFile (const LineInfo &info);
    static wxFileName FindFile (const wxString &fileName, const wxString &project);
    static wxFileName FindFile (const wxArrayString& files, const wxString &fileName);
    
	void Initialize       ();
	void DoMarkAndOpenFile(std::map<int,LineInfo>::iterator i, bool clearsel);
    bool ExtractLineInfo  (LineInfo &info, const wxString &text, const wxString &pattern, 
                           const wxString &fileidx, const wxString &lineidx);
    void MarkEditor       (LEditor *editor);
    std::map<int,LineInfo>::iterator GetNextBadLine();

    // Event handlers
    void OnClearAll           (wxCommandEvent   &e);
    void OnBuildStarted       (wxCommandEvent   &e);
    void OnBuildAddLine       (wxCommandEvent   &e);
    void OnBuildEnded         (wxCommandEvent   &e);
    void OnWorkspaceLoaded    (wxCommandEvent   &e);
    void OnWorkspaceClosed    (wxCommandEvent   &e);
    void OnConfigChanged      (wxCommandEvent   &e);
	void OnCompilerColours    (wxCommandEvent   &e);
	void OnNextBuildError     (wxCommandEvent   &e);
    void OnNextBuildErrorUI   (wxUpdateUIEvent  &e);
    void OnActiveEditorChanged(wxCommandEvent   &e);
	void OnHotspotClicked     (wxScintillaEvent &e);
	void OnMouseDClick        (wxScintillaEvent &e);
    
protected:
	void Clear();
	void AppendText(const wxString &text);
    
public:
	BuildTab(wxWindow *parent, wxWindowID id, const wxString &name);
	~BuildTab();
};
#endif // __buidltab__

