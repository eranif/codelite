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
#include <vector>
#include <wx/regex.h>
#include <wx/filename.h>
#include <wx/stopwatch.h>
#include "outputtabwindow.h"
#include "compiler.h"
#include "buildtabsettingsdata.h"

class LEditor;

class CompiledPattern
{
public:
	wxRegEx* regex;
	wxString fileIndex;
	wxString lineIndex;

public:
	CompiledPattern(wxRegEx *re, const wxString &file, const wxString &line)
			: regex(re)
			, fileIndex(file)
			, lineIndex(line) {}

	~CompiledPattern() {
		if (regex) {
			delete regex;
			regex = NULL;
		}
	}
};

typedef SmartPtr<CompiledPattern> CompiledPatternPtr;

struct CompilerPatterns {
	std::vector<CompiledPatternPtr> errorsPatterns;
	std::vector<CompiledPatternPtr> warningPatterns;
};

class BuildTab : public OutputTabWindow
{
	friend class ErrorsTab;

private:
	/**
	 * Tracks an "important" line in the build log (error, warning, etc)
	 */
	struct LineInfo {
		LineInfo()
				: linenum(0), linecolor(wxSCI_LEX_GCC_OUTPUT), filestart(0), filelen(0) { }
		wxString project;
		wxString configuration;
		wxString linetext;
		wxString filename;
		long     linenum;
		int      linecolor;
		size_t   filestart;
		size_t   filelen;
		wxString rawFilename;
	};

private:
	std::map<int, LineInfo>              m_lineInfo;
	std::map<wxString, int>              m_lineMap;
	std::multimap<wxString,int>          m_fileMap;
	int                                  m_showMe;
	bool                                 m_autoHide;
	bool                                 m_skipWarnings;
	bool                                 m_building;
	int                                  m_errorCount;
	int                                  m_warnCount;
	CompilerPtr                          m_cmp;               // compiler in use during currently building project
	wxStopWatch                          m_sw;                // times the entire build
	static BuildTab *                    s_bt;                // self reference for ColorLine to access the m_line* maps
	std::map<wxString, CompilerPatterns> m_compilerParseInfo;
	wxArrayString                        m_baseDir;

private:
	static int  ColorLine(int, const char *text, size_t &start, size_t &len);
	static void SetStyles(wxScintilla *sci);

	void Initialize       ();
	void DoMarkAndOpenFile  (std::map<int,LineInfo>::iterator i, bool scrollToLine);
	bool ExtractLineInfo    (LineInfo &info, const wxString &text, const wxRegEx &re, const wxString &fileidx, const wxString &lineidx);
	bool GetCompilerPatterns(const wxString &compilerName, CompilerPatterns &patterns);
	void DoProcessLine      (const wxString &text, int lineno);
	void MarkEditor         (LEditor *editor);
	std::map<int,LineInfo>::iterator GetNextBadLine();

	// Event handlers
	void OnActiveEditorChanged(wxCommandEvent   &e);
	void OnBuildStarted       (wxCommandEvent   &e);
	void OnBuildAddLine       (wxCommandEvent   &e);
	void OnBuildEnded         (wxCommandEvent   &e);
	void OnWorkspaceLoaded    (wxCommandEvent   &e);
	void OnWorkspaceClosed    (wxCommandEvent   &e);
	void OnConfigChanged      (wxCommandEvent   &e);
	void OnCompilerColours    (wxCommandEvent   &e);
	void OnClearAll           (wxCommandEvent   &e);
	void OnRepeatOutput       (wxCommandEvent   &e);
	void OnNextBuildError     (wxCommandEvent   &e);
	void OnClearAllUI         (wxUpdateUIEvent  &e);
	void OnRepeatOutputUI     (wxUpdateUIEvent  &e);
	void OnNextBuildErrorUI   (wxUpdateUIEvent  &e);
	void OnMouseDClick        (wxScintillaEvent &e);
protected:
	void Clear();
	void AppendText(const wxString &text);

public:
	BuildTab(wxWindow *parent, wxWindowID id, const wxString &name);
	~BuildTab();

	wxString GetBuildToolTip(const wxString &fileName, int lineno, wxMemoryBuffer &styleBits);

	bool GetBuildEndedSuccessfully() const {return m_errorCount == 0;}
};
#endif // __buidltab__
