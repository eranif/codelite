//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buidltab.cpp
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
#include "precompiled_header.h"
#include <wx/ffile.h>
#include "globals.h"
#include "plugin.h"
#include <wx/regex.h>
#include <wx/xrc/xmlres.h>
#include <wx/wxscintilla.h>

#include "macros.h"
#include "editor_config.h"
#include "frame.h"
#include "buildtabsettingsdata.h"
#include "regex_processor.h"
#include "build_settings_config.h"
#include "mainbook.h"
#include "cl_editor.h"
#include "manager.h"
#include "project.h"
#include "buidltab.h"
#include "errorstab.h"

//#define __PERFORMANCE
#include "performance.h"

// from sdk/wxscintilla/src/scintilla/src/LexGCC.cxx:
extern void SetGccColourFunction ( int ( *colorfunc ) ( int, const char*, size_t&, size_t& ) );

BuildTab *BuildTab::s_bt;

BuildTab::BuildTab ( wxWindow *parent, wxWindowID id, const wxString &name )
		: OutputTabWindow ( parent, id, name )
		, m_showMe ( BuildTabSettingsData::ShowOnStart )
		, m_autoHide ( false )
		, m_skipWarnings ( true )
		, m_building ( false )
		, m_errorCount ( 0 )
		, m_warnCount ( 0 )
{
	m_tb->RemoveTool ( XRCID ( "repeat_output" ) );

	m_tb->AddTool ( XRCID ( "advance_settings" ), wxT ( "Set compiler colours..." ),
	                wxXmlResource::Get()->LoadBitmap ( wxT ( "colourise" ) ), wxT ( "Set compiler colours..." ) );
	Connect ( XRCID ( "advance_settings" ),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler ( BuildTab::OnCompilerColours ), NULL, this );
	m_tb->Realize();

	wxTheApp->Connect ( wxEVT_SHELL_COMMAND_STARTED,         wxCommandEventHandler ( BuildTab::OnBuildStarted ),    NULL, this );
	wxTheApp->Connect ( wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, wxCommandEventHandler ( BuildTab::OnBuildStarted ),    NULL, this );
	wxTheApp->Connect ( wxEVT_SHELL_COMMAND_ADDLINE,         wxCommandEventHandler ( BuildTab::OnBuildAddLine ),    NULL, this );
	wxTheApp->Connect ( wxEVT_SHELL_COMMAND_PROCESS_ENDED,   wxCommandEventHandler ( BuildTab::OnBuildEnded ),      NULL, this );
	wxTheApp->Connect ( wxEVT_WORKSPACE_LOADED,              wxCommandEventHandler ( BuildTab::OnWorkspaceLoaded ), NULL, this );
	wxTheApp->Connect ( wxEVT_WORKSPACE_CLOSED,              wxCommandEventHandler ( BuildTab::OnWorkspaceClosed ), NULL, this );
	wxTheApp->Connect ( wxEVT_EDITOR_CONFIG_CHANGED,         wxCommandEventHandler ( BuildTab::OnConfigChanged ),   NULL, this );

	wxTheApp->Connect ( XRCID ( "next_error" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler ( BuildTab::OnNextBuildError ),   NULL, this );
	wxTheApp->Connect ( XRCID ( "next_error" ), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler ( BuildTab::OnNextBuildErrorUI ), NULL, this );

	wxTheApp->Connect ( wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler ( BuildTab::OnActiveEditorChanged ), NULL, this );

	s_bt = this;
	SetGccColourFunction ( ColorLine );
	Initialize();
}

BuildTab::~BuildTab()
{
	s_bt = NULL;
	SetGccColourFunction ( NULL );
}

int BuildTab::ColorLine ( int, const char *text, size_t &start, size_t &len )
{
	wxString txt(text, wxConvUTF8);

	if(txt.Contains(wxT("Entering directory"))) {
		return wxSCI_LEX_GCC_MAKE_ENTER;
	}

	if(txt.Contains(wxT("Leaving directory"))) {
		return wxSCI_LEX_GCC_MAKE_LEAVING;
	}

	std::map<wxString,int>::iterator i = s_bt->m_lineMap.find ( _U ( text ) );
	if ( i == s_bt->m_lineMap.end() )
		return wxSCI_LEX_GCC_OUTPUT;
	std::map<int,LineInfo>::iterator j = s_bt->m_lineInfo.find ( i->second );
	if ( j == s_bt->m_lineInfo.end() )
		return wxSCI_LEX_GCC_OUTPUT;
	start = j->second.filestart;
	len = j->second.filelen;
	return j->second.linecolor;
}

void BuildTab::Initialize()
{
	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject ( wxT ( "build_tab_settings" ), &options );

	m_showMe       = options.GetShowBuildPane();
	m_autoHide     = options.GetAutoHide();
	m_skipWarnings = options.GetSkipWarnings();

    m_autoAppear   = (m_showMe == BuildTabSettingsData::ShowOnStart);

	SetStyles ( m_sci );
}

void BuildTab::SetStyles ( wxScintilla *sci )
{
	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject ( wxT ( "build_tab_settings" ), &options );

	InitStyle ( sci, wxSCI_LEX_GCC, true );

	sci->StyleSetForeground ( wxSCI_LEX_GCC_MAKE_ENTER, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
	sci->StyleSetBackground ( wxSCI_LEX_GCC_MAKE_ENTER, wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	sci->StyleSetForeground ( wxSCI_LEX_GCC_MAKE_LEAVING, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) );
	sci->StyleSetBackground ( wxSCI_LEX_GCC_MAKE_LEAVING, wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW ) );

	sci->StyleSetForeground ( wxSCI_LEX_GCC_OUTPUT, wxT("BLACK") );
	sci->StyleSetBackground ( wxSCI_LEX_GCC_OUTPUT, wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW ) );

	sci->StyleSetForeground ( wxSCI_LEX_GCC_BUILDING, wxT("BLACK") );
	sci->StyleSetBackground ( wxSCI_LEX_GCC_BUILDING, wxSystemSettings::GetColour ( wxSYS_COLOUR_WINDOW ) );

	sci->StyleSetForeground ( wxSCI_LEX_GCC_WARNING, options.GetWarnColour() );
	sci->StyleSetBackground ( wxSCI_LEX_GCC_WARNING, options.GetWarnColourBg() );

	sci->StyleSetForeground ( wxSCI_LEX_GCC_ERROR, options.GetErrorColour() );
	sci->StyleSetBackground ( wxSCI_LEX_GCC_ERROR, options.GetErrorColourBg() );

	sci->StyleSetForeground ( wxSCI_LEX_GCC_FILE_LINK, wxT ( "BLUE" ) );

	sci->StyleSetHotSpot ( wxSCI_LEX_GCC_FILE_LINK, true );
	sci->StyleSetHotSpot ( wxSCI_LEX_GCC_BUILDING, true );

	wxFont defFont = wxSystemSettings::GetFont ( wxSYS_DEFAULT_GUI_FONT );
	wxFont font ( defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL );
	wxFont bold ( defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxFONTWEIGHT_BOLD );

	sci->StyleSetFont ( wxSCI_LEX_GCC_DEFAULT,      font );
	sci->StyleSetFont ( wxSCI_LEX_GCC_OUTPUT,       font );
	sci->StyleSetFont ( wxSCI_LEX_GCC_BUILDING,     bold );
	sci->StyleSetFont ( wxSCI_LEX_GCC_FILE_LINK,    font );
	sci->StyleSetFont ( wxSCI_LEX_GCC_WARNING,      options.GetBoldWarnFont() ? bold : font );
	sci->StyleSetFont ( wxSCI_LEX_GCC_ERROR,        options.GetBoldErrFont()  ? bold : font );
	sci->StyleSetFont ( wxSCI_LEX_GCC_MAKE_ENTER,   font );
	sci->StyleSetFont ( wxSCI_LEX_GCC_MAKE_LEAVING, font );

	sci->Colourise ( 0, sci->GetLength() );
}

void BuildTab::Clear()
{
	OutputTabWindow::Clear();
	Initialize();
	m_lineInfo.clear();
	m_lineMap.clear();
    m_fileMap.clear();
	m_errorCount = 0;
	m_warnCount = 0;
	m_cmp.Reset ( NULL );
	Frame::Get()->GetOutputPane()->GetErrorsTab()->ClearLines();
	LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
	if ( editor ) {
		editor->AnnotationClearAll();
		editor->AnnotationSetVisible(0); // Hidden
		editor->DelAllCompilerMarkers();
		editor->Refresh();
	}
}

void BuildTab::AppendText ( const wxString &text )
{
	int lineno = m_sci->GetLineCount()-1;    // get line number before appending new text
    OutputTabWindow::AppendText ( text );
	int newLineno = m_sci->GetLineCount()-1; // Get the new line number
	int lineCount = newLineno - lineno;
	if(lineCount == 0) {
		// We are still at the same line number
		DoProcessLine(m_sci->GetLine(lineno), lineno);
	} else {
		for(int i=0; i<lineCount; i++) {
			DoProcessLine(m_sci->GetLine(lineno+i), lineno+i);
		}
	}
}

bool BuildTab::ExtractLineInfo ( LineInfo &info, const wxString &text, const wxRegEx &re, const wxString &fileidx, const wxString &lineidx )
{
	long fidx, lidx;
	if ( !fileidx.ToLong ( &fidx ) || !lineidx.ToLong ( &lidx ) )
		return false;

	if ( !re.Matches ( text ) )
		return false;

	size_t start;
	size_t len;
	if ( re.GetMatch ( &start, &len, fidx ) ) {
		// sometimes we get leading spaces in the filename match
		while ( len > 0 && text[start] == wxT ( ' ' ) ) {
			start++;
			len--;
		}
		info.filestart = start;
		info.filelen = len;

        // find the actual workspace file (if possible)
		wxString filename = text.Mid( info.filestart, info.filelen).Trim().Trim(false);
		wxFileName fn(filename);
		
		// Use the current base dir
		wxString baseDir ( m_baseDir );
		if(baseDir.IsEmpty()) {
			ProjectPtr project = ManagerST::Get()->GetProject(info.project);
			if(project) {
				baseDir = project->GetFileName().GetPath();
			}
		}
		
		fn.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG, baseDir);
		info.filename = fn.GetFullPath();
	}

	if ( re.GetMatch ( &start, &len, lidx ) ) {
		text.Mid ( start, len ).ToLong ( &info.linenum );
		info.linenum--; // scintilla starts counting lines from 0
	}
	return true;
}

std::map<int,BuildTab::LineInfo>::iterator BuildTab::GetNextBadLine()
{
	// start scanning from currently marked line
	int nFoundLine = m_sci->MarkerNext(0, 255);
	std::map<int,LineInfo>::iterator i = m_lineInfo.upper_bound ( nFoundLine );
	std::map<int,LineInfo>::iterator e = m_lineInfo.end();
	for ( ; i != e && i->second.linecolor != wxSCI_LEX_GCC_ERROR &&
	        ( m_skipWarnings || i->second.linecolor != wxSCI_LEX_GCC_WARNING ); i++ ) { }
	if ( i == e ) {
		// wrap around to beginning
		i = m_lineInfo.begin();
		e = m_lineInfo.lower_bound ( nFoundLine );
		for ( ; i != e && i->second.linecolor != wxSCI_LEX_GCC_ERROR &&
		        ( m_skipWarnings || i->second.linecolor != wxSCI_LEX_GCC_WARNING ); i++ ) { }
	}
	return i != e ? i : m_lineInfo.end();
}

void BuildTab::DoMarkAndOpenFile ( std::map<int,LineInfo>::iterator i, bool scrollToLine )
{
	if ( i == m_lineInfo.end() )
        return;

    const LineInfo &info = i->second;
    if (info.linecolor != wxSCI_LEX_GCC_ERROR && info.linecolor != wxSCI_LEX_GCC_WARNING)
        return;

	wxFileName filename(info.filename);

	ProjectPtr project = ManagerST::Get()->GetProject(info.project);
	if(project) {
		filename.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG, project->GetFileName().GetPath());
	} else {
		filename.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG);
	}

	LEditor *editor = Frame::Get()->GetMainBook()->OpenFile ( filename.GetFullPath(), info.project, info.linenum );
    if (editor == NULL) {
		return;
	}

    // mark the current error/warning line in the output tab
    m_sci->MarkerDeleteAll   ( 0x7           );
	m_sci->MarkerAdd         ( i->first, 0x7 );

	if(scrollToLine)
		m_sci->ScrollToLine      ( i->first      );

    // mark it in the errors tab too
    Frame::Get()->GetOutputPane()->GetErrorsTab()->MarkLine ( i->first );
}

void BuildTab::MarkEditor ( LEditor *editor )
{
	if ( !editor )
		return;

	editor->DelAllCompilerMarkers();
	editor->AnnotationClearAll();
	editor->AnnotationSetVisible(2); // Visible with box around it

	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);

	// Are annotations enabled?
	if( options.GetErrorWarningStyle() == BuildTabSettingsData::EWS_NoMarkers ) {
		return;
	}

    std::pair<std::multimap<wxString,int>::iterator,
              std::multimap<wxString,int>::iterator> iters = m_fileMap.equal_range(editor->GetFileName().GetFullPath());
	std::multimap<wxString,int>::iterator b = iters.first;
	std::multimap<wxString,int>::iterator e = iters.second;
    if (b == m_fileMap.end())
        return;
		
	std::set<wxString> uniqueSet;
    for (; b != e; b++ ) {
		
		// get the line info related to the BuildTab's line number (b->second)
        std::map<int,LineInfo>::iterator i = m_lineInfo.find ( b->second ) ;

        if ( i == m_lineInfo.end() )
            continue; // safety check -- should not normally happen

		int line_colour = i->second.linecolor;
		if ( line_colour == wxSCI_LEX_GCC_ERROR || line_colour == wxSCI_LEX_GCC_WARNING ) {

			wxMemoryBuffer style_bytes;
			LineInfo lineInfo = i->second;
			
			// For performance, dont add the exact same markers to the same line number/filename 
			// with the exact same tip
			wxString tipMagic;
			tipMagic << lineInfo.linenum << lineInfo.linetext;
			if(uniqueSet.find(tipMagic) != uniqueSet.end()) {
				// we already reported the exact same tip for that line
				// skip this one
				continue;
			} else {
				// add it to the unique set
				uniqueSet.insert(tipMagic);
			}
			
			// format the tip
			int line_number = lineInfo.linenum;
			wxString tip = GetBuildToolTip(editor->GetFileName().GetFullPath(), line_number, style_bytes);
						
			// Set annotations
			if ( options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Annotations ) {
				editor->AnnotationSetText (line_number, tip);
				editor->AnnotationSetStyles(line_number, style_bytes );
			}

			// Set compiler bookmarks
			if ( options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Bookmarks ) {
				if ( line_colour == wxSCI_LEX_GCC_ERROR ) {
					editor->SetErrorMarker( line_number );
				} else if ( line_colour == wxSCI_LEX_GCC_WARNING ) {
					editor->SetWarningMarker( line_number );
				}
			}

		}

    }
	editor->Refresh();
}

void BuildTab::OnClearAll ( wxCommandEvent &e )
{
	Clear();
}

void BuildTab::OnClearAllUI ( wxUpdateUIEvent& e )
{
	e.Enable ( m_sci->GetLength() > 0 && !m_building );
}

void BuildTab::OnRepeatOutput ( wxCommandEvent& e )
{
	e.Skip();
}

void BuildTab::OnRepeatOutputUI ( wxUpdateUIEvent& e )
{
	e.Enable ( m_sci->GetLength() > 0 && !m_building );
}

void BuildTab::OnBuildStarted ( wxCommandEvent &e )
{
	e.Skip();
	
	m_building = true;
	
	// Clear all compiler parsing information
	m_compilerParseInfo.clear();
		
	// Loop over all known compilers and cache the regular expressions
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	while( cmp ) {
		CompilerPatterns cmpPatterns;
		const Compiler::CmpListInfoPattern& errPatterns  = cmp->GetErrPatterns();
		const Compiler::CmpListInfoPattern& warnPatterns = cmp->GetWarnPatterns();
		Compiler::CmpListInfoPattern::const_iterator iter;
		for (iter = errPatterns.begin(); iter != errPatterns.end(); iter++) {
			
			CompiledPatternPtr compiledPatternPtr(new CompiledPattern(new wxRegEx(iter->pattern), iter->fileNameIndex, iter->lineNumberIndex));
			if(compiledPatternPtr->regex->IsValid()) {
				cmpPatterns.errorsPatterns.push_back( compiledPatternPtr );
			}
		}
		
		for (iter = warnPatterns.begin(); iter != warnPatterns.end(); iter++) {
			
			CompiledPatternPtr compiledPatternPtr(new CompiledPattern(new wxRegEx(iter->pattern), iter->fileNameIndex, iter->lineNumberIndex));
			if(compiledPatternPtr->regex->IsValid()) {
				cmpPatterns.warningPatterns.push_back( compiledPatternPtr );
			}
		}
		
		m_compilerParseInfo[cmp->GetName()] = cmpPatterns;
		cmp =  BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
	}
	
	if ( e.GetEventType() != wxEVT_SHELL_COMMAND_STARTED_NOCLEAN ) {
		// Reset output and counters
		Clear();
	}
	
	AppendText ( BUILD_START_MSG );
//	Frame::Get()->SetStatusMessage ( e.GetString(), 3, XRCID ( "build" ) );
    OutputPane *opane = Frame::Get()->GetOutputPane();
	if (m_showMe == BuildTabSettingsData::ShowOnEnd &&
            m_autoHide &&
            ManagerST::Get()->IsPaneVisible(opane->GetCaption()) &&
            (opane->GetNotebook()->GetCurrentPage() == this ||
                opane->GetNotebook()->GetCurrentPage() == opane->GetErrorsTab())) {
        // user prefers to see build/errors tabs only at end of unsuccessful build
        ManagerST::Get()->HidePane(opane->GetName());
    }
	m_sw.Start();

	// notify the plugins that the build had started
	PostCmdEvent(wxEVT_BUILD_STARTED);
}

void BuildTab::OnBuildAddLine ( wxCommandEvent &e )
{
	e.Skip();
	AppendText ( e.GetString() );
//    if (e.GetInt() == QueueCommand::CustomBuild && e.GetString().Contains(BUILD_PROJECT_PREFIX) && !m_lineInfo.empty()) {
//        // try to show more specific progress in custom builds
//        LineInfo &info = m_lineInfo.rbegin()->second;
//        Frame::Get()->SetStatusMessage(wxString::Format(wxT("Building %s (%s)"),
//                                       info.project.c_str(), info.configuration.c_str()), 3, XRCID("build"));
//    }
}

void BuildTab::OnBuildEnded ( wxCommandEvent &e )
{
	e.Skip();

	m_building = false;
	AppendText (BUILD_END_MSG);

	wxString term = wxString::Format ( wxT ( "%d errors, %d warnings" ), m_errorCount, m_warnCount );
	long elapsed = m_sw.Time() / 1000;
	if ( elapsed > 10 ) {
		long sec = elapsed % 60;
		long hours = elapsed / 3600;
		long minutes = ( elapsed % 3600 ) / 60;
		term << wxString::Format ( wxT ( ", total time: %02d:%02d:%02d seconds" ), hours, minutes, sec );

	}
	term << wxT ( '\n' );
	AppendText ( term );

//	Frame::Get()->SetStatusMessage ( wxEmptyString, 3, XRCID ( "build" ) );

	bool success = m_errorCount == 0 && ( m_skipWarnings || m_warnCount == 0 );
	bool viewing = ManagerST::Get()->IsPaneVisible ( Frame::Get()->GetOutputPane()->GetCaption() ) &&
	               ( Frame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() == this ||
	                 Frame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() ==
	                 Frame::Get()->GetOutputPane()->GetErrorsTab() );

	if ( !success ) {
		if ( viewing ) {
			std::map<int,LineInfo>::iterator i = GetNextBadLine();
			m_sci->GotoLine ( i->first-1 ); // minus one line so user can type F4 to open the first error
		} else {
			ManagerST::Get()->ShowOutputPane ( Frame::Get()->GetOutputPane()->GetErrorsTab()->GetCaption() );
		}
	} else if ( m_autoHide && viewing ) {
		ManagerST::Get()->HidePane ( Frame::Get()->GetOutputPane()->GetCaption() );
	} else if ( m_showMe == BuildTabSettingsData::ShowOnEnd && !m_autoHide ) {
		ManagerST::Get()->ShowOutputPane ( m_name );
	}

	MarkEditor ( Frame::Get()->GetMainBook()->GetActiveEditor() );
	
	// notify the plugins that the build had ended
	PostCmdEvent(wxEVT_BUILD_ENDED);
}

void BuildTab::OnWorkspaceLoaded ( wxCommandEvent &e )
{
	e.Skip();
	Clear();
}

void BuildTab::OnWorkspaceClosed ( wxCommandEvent &e )
{
	e.Skip();
	Clear();
}

void BuildTab::OnConfigChanged ( wxCommandEvent &e )
{
	e.Skip();
	const wxString *config = ( const wxString * ) e.GetClientData();
	if ( config && *config == wxT ( "build_tab_settings" ) ) {
		Initialize();
		Frame::Get()->GetOutputPane()->GetErrorsTab()->OnRedisplayLines ( e );
	}
}

void BuildTab::OnCompilerColours ( wxCommandEvent &e )
{
	// tell Frame to open Advance Settings dlg on compiler colors page
	e.SetInt ( 1 );
	e.Skip();
}

void BuildTab::OnNextBuildError ( wxCommandEvent &e )
{
	wxUnusedVar ( e );
	if ( (m_errorCount > 0) || (!m_skipWarnings && m_warnCount > 0) ) {
		std::map<int,LineInfo>::iterator i = GetNextBadLine();
		if ( i != m_lineInfo.end() ) {
			wxString showpane = m_name;
			if ( Frame::Get()->GetOutputPane()->GetNotebook()->GetCurrentPage() ==
			        Frame::Get()->GetOutputPane()->GetErrorsTab() ) {
				showpane = Frame::Get()->GetOutputPane()->GetErrorsTab()->GetCaption();
			}
			ManagerST::Get()->ShowOutputPane ( showpane );
			DoMarkAndOpenFile ( i, true );
		}
	}
}

void BuildTab::OnNextBuildErrorUI ( wxUpdateUIEvent &e )
{
	e.Enable ( (m_errorCount > 0) || (!m_skipWarnings && m_warnCount > 0) );
}

void BuildTab::OnActiveEditorChanged ( wxCommandEvent &e )
{
	e.Skip();
	MarkEditor ( Frame::Get()->GetMainBook()->GetActiveEditor() );
}

void BuildTab::OnMouseDClick ( wxScintillaEvent &e )
{
	PERF_START("BuildTab::OnMouseDClick");

	int pos = e.GetPosition();
	int style = m_sci->GetStyleAt(pos);
	int line = m_sci->LineFromPosition(pos);

	if ( style == wxSCI_LEX_GCC_BUILDING ) {
		m_sci->ToggleFold ( line );
		m_sci->SetSelection ( wxNOT_FOUND, pos );

	} else {
		PERF_BLOCK("DoMarkAndOpenFile") {
			DoMarkAndOpenFile ( m_lineInfo.find ( m_sci->LineFromPosition ( e.GetPosition() ) ), false );
		}
	}
	PERF_END();
}

wxString BuildTab::GetBuildToolTip(const wxString& fileName, int lineno, wxMemoryBuffer &styleBits)
{
	std::pair<std::multimap<wxString,int>::iterator,
              std::multimap<wxString,int>::iterator> iters = m_fileMap.equal_range(fileName);

	std::multimap<wxString,int>::iterator i1 = iters.first;
	std::multimap<wxString,int>::iterator i2 = iters.second;

	if(i1 == m_fileMap.end())
		return wxEmptyString;

	wxString tip;
	for ( ; i1 != i2;  i1++ ) {
        std::map<int,LineInfo>::iterator i = m_lineInfo.find ( i1->second ) ;
        if ( i != m_lineInfo.end() && i->second.linenum == lineno && (i->second.linecolor == wxSCI_LEX_GCC_ERROR || i->second.linecolor == wxSCI_LEX_GCC_WARNING )) {
            wxString text = i->second.linetext.Mid(i->second.filestart+i->second.filelen);
            if (!text.IsEmpty() && text[0] == wxT(':')) {
                text.erase(0, 1);
            }

			wxString tmpTip (wxT(" ") + text.Trim(false).Trim() + wxT("\n"));

#if defined(__WXGTK__) || defined (__WXMAC__)
			// Remove any non ascii characters from the tip
			wxString asciiTip;

			for(size_t at=0; at<tmpTip.Length(); at++) {
				if( isprint((char)tmpTip.GetChar(at) ) || tmpTip.GetChar(at) == wxT('\n') ) {
					asciiTip.Append( tmpTip.GetChar(at) );
				}
			}

			tmpTip = asciiTip;
			tmpTip.Replace(wxT("\r"), wxT(""));
			tmpTip.Replace(wxT("\t"), wxT(" "));
#endif
			if( tip.Contains(tmpTip) == false ) {
				for(size_t j=0; j<tmpTip.Length(); j++) {
					if( i->second.linecolor == wxSCI_LEX_GCC_WARNING ) {
						styleBits.AppendByte((char)eAnnotationStyleWarning);
					} else {
						styleBits.AppendByte((char)eAnnotationStyleError);
					}
				}
				tip << tmpTip;
			}

        }
    }

	if(tip.IsEmpty() == false) {
		tip.RemoveLast();
		styleBits.SetDataLen( styleBits.GetDataLen()-1 );
	}

	return tip ;
}

void BuildTab::DoProcessLine(const wxString& text, int lineno)
{
	LineInfo info;
	info.linetext = text;

	if ( text.Contains ( BUILD_PROJECT_PREFIX ) ) {
		// now building the next project
		wxString prj = text.AfterFirst ( wxT ( '[' ) ).BeforeFirst ( wxT ( ']' ) );
		info.project       = prj.BeforeFirst ( wxT ( '-' ) ).Trim ( false ).Trim();
		info.configuration = prj.AfterFirst ( wxT ( '-' ) ).Trim ( false ).Trim();
		info.linecolor     = wxSCI_LEX_GCC_BUILDING;
		m_cmp.Reset ( NULL );
		// need to know the compiler in use for this project to extract
		// file/line and error/warning status from the text
		ProjectPtr proj = ManagerST::Get()->GetProject ( info.project );
		if ( proj ) {
			ProjectSettingsPtr settings = proj->GetSettings();
			if ( settings ) {
				BuildConfigPtr bldConf = settings->GetBuildConfiguration ( info.configuration );
				if ( !bldConf ) {
					// no buildconf matching the named conf, so use first buildconf instead
					ProjectSettingsCookie cookie;
					bldConf = settings->GetFirstBuildConfiguration ( cookie );
				}
				if ( bldConf ) {
					m_cmp = BuildSettingsConfigST::Get()->GetCompiler ( bldConf->GetCompilerType() );
				}
			}
		} else {
			// probably custom build with project names incorret
			// assign the default compiler for this purpose
			if ( BuildSettingsConfigST::Get()->IsCompilerExist(wxT("gnu g++")) ) {
				m_cmp = BuildSettingsConfigST::Get()->GetCompiler( wxT("gnu g++") );
			}
		}
	} else if ( !m_lineInfo.empty() ) {
		// consider this line part of the currently building project
		info.project       = m_lineInfo.rbegin()->second.project;
		info.configuration = m_lineInfo.rbegin()->second.configuration;
	}

	// check for start-of-build or end-of-build messages
	if ( text.StartsWith ( BUILD_START_MSG ) || text.StartsWith ( BUILD_END_MSG ) ) {
		info.linecolor = wxSCI_LEX_GCC_BUILDING;
	}
	
	// Check for makefile directory changes lines
	if(text.Contains(wxT("Entering directory `"))) {
		wxString currentDir = text.AfterFirst(wxT('`'));
		currentDir = currentDir.BeforeLast(wxT('\''));
		m_baseDir = currentDir;
	}
	
	if ( info.linecolor == wxSCI_LEX_GCC_BUILDING || !m_cmp ) {
		// no more line info to get
	} else {
		
		// Find error first
		bool isError = false;
		
		CompilerPatterns cmpPatterns;
		if(!GetCompilerPatterns(m_cmp->GetName(), cmpPatterns)) {
			return;
		}
		
		for(size_t i=0; i<cmpPatterns.errorsPatterns.size(); i++) {
			CompiledPatternPtr cmpPatterPtr = cmpPatterns.errorsPatterns.at(i);
			if ( ExtractLineInfo(info, text, *(cmpPatterPtr->regex), cmpPatterPtr->fileIndex, cmpPatterPtr->lineIndex) ) {
				info.linecolor = wxSCI_LEX_GCC_ERROR;
				m_errorCount++;
				isError = true;
				break;
			}
		}
		if (!isError) {
			// If it is not an error, maybe it's a warning
			for(size_t i=0; i<cmpPatterns.warningPatterns.size(); i++) {
				CompiledPatternPtr cmpPatterPtr = cmpPatterns.warningPatterns.at(i);
				if ( ExtractLineInfo(info, text, *(cmpPatterPtr->regex), cmpPatterPtr->fileIndex, cmpPatterPtr->lineIndex) ) {
					info.linecolor = wxSCI_LEX_GCC_WARNING;
					m_warnCount++;
					isError = true;
					break;
				}
			}
		}
	}

	if ( info.linecolor != wxSCI_LEX_GCC_OUTPUT ) {
		m_lineInfo[lineno] = info;
		m_lineMap[text] = lineno;
        if (!info.filename.IsEmpty() && (info.linecolor == wxSCI_LEX_GCC_ERROR || info.linecolor == wxSCI_LEX_GCC_WARNING)) {
            m_fileMap.insert(std::make_pair(info.filename, lineno));
        }
		Frame::Get()->GetOutputPane()->GetErrorsTab()->AppendLine ( lineno );
	}
}

bool BuildTab::GetCompilerPatterns(const wxString& compilerName, CompilerPatterns& patterns)
{
	std::map<wxString, CompilerPatterns>::iterator iter = m_compilerParseInfo.find(compilerName);
	if(iter == m_compilerParseInfo.end()) {
		return false;
	}
	patterns = iter->second;
	return true;
}
