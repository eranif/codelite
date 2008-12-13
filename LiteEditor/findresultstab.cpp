//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findresultstab.cpp
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
#include "findresultstab.h"
#include <wx/xrc/xmlres.h>
#include <wx/tokenzr.h>
#include "wx/string.h"
#include "manager.h"
#include "cl_editor.h"

extern void HSL_2_RGB(float h, float s, float l, float *r, float *g, float *b);
extern void RGB_2_HSL(float r, float g, float b, float *h, float *s, float *l);

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

wxColour lightColour(wxColour color, float level)
{
	float r, g, b;

	// convert the HSL
	float h, s, l;
	RGB_2_HSL((float)color.Red(), (float)color.Green(), (float)color.Blue(), &h, &s, &l);

	// increase the Lum value
	l += (float)((level * 5.0)/100.0);
	if (l < 0) l = 0.0;

	// convert back from HSL to RGB
	HSL_2_RGB(h, s, l, &r, &g, &b);

	return wxColour ((unsigned int)r, (unsigned int)g, (unsigned int)b);
}

FindResultsTab::FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name)
		: OutputTabWindow(parent, id, name)
{
	//SCLEX_FIF
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	// use custom FIF (Find In File) lexer
	m_sci->SetLexer(wxSCI_LEX_FIF);

	// set different colours to the different style
	m_sci->StyleSetForeground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	m_sci->StyleSetBackground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	m_sci->StyleSetForeground(wxSCI_LEX_FIF_FILE, wxT("BLUE"));
	m_sci->StyleSetBackground(wxSCI_LEX_FIF_FILE, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	m_sci->StyleSetForeground(wxSCI_LEX_FIF_FILE_SHORT, wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));
	m_sci->StyleSetBackground(wxSCI_LEX_FIF_FILE_SHORT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	m_sci->StyleSetForeground(wxSCI_LEX_FIF_MATCH, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	m_sci->StyleSetBackground(wxSCI_LEX_FIF_MATCH, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	m_sci->StyleSetFont(wxSCI_LEX_FIF_FILE, font);
	m_sci->StyleSetFont(wxSCI_LEX_FIF_DEFAULT, font);
	m_sci->StyleSetFont(wxSCI_LEX_FIF_MATCH, font);
	m_sci->StyleSetFont(wxSCI_LEX_FIF_FILE_SHORT, font);

	// make the hotspot colour its background with light green colour
	m_sci->StyleSetHotSpot(wxSCI_LEX_FIF_MATCH, true);
	m_sci->StyleSetHotSpot(wxSCI_LEX_FIF_FILE, true);

	m_sci->SetHotspotActiveUnderline(false);
	m_sci->SetHotspotActiveBackground(true, lightColour(wxT("BLUE"), 8.0));

	// enable folding
	m_sci->SetMarginMask(4, wxSCI_MASK_FOLDERS);
	m_sci->SetMarginWidth(4, 16);
	m_sci->SetProperty(wxT("fold"), wxT("1"));
	m_sci->SetMarginSensitive(4, true);

	// mark folded lines with line
	m_sci->SetFoldFlags(16);

	// make the fold lines grey
	m_sci->StyleSetForeground(wxSCI_STYLE_DEFAULT, wxT("GREY"));

	DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
	DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	// handle folding
	m_sci->Connect(wxEVT_SCI_MARGINCLICK, wxScintillaEventHandler(FindResultsTab::OnMarginClick), NULL, this);

	// add 'FoldAll' button to the toolbar
	m_tb->AddTool(XRCID("collapse_all"),
	              _("Fold All Results"),
	              wxXmlResource::Get()->LoadBitmap(wxT("fold_airplane")),
	              _("Fold All Results"));
	Connect( XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FindResultsTab::OnCollapseAll ), NULL, this);
	m_tb->Realize();

}

FindResultsTab::~FindResultsTab()
{
}

void FindResultsTab::OnHotspotClicked(wxScintillaEvent &event)
{
	//same as double click
	long pos = event.GetPosition();
	if (m_sci->GetStyleAt(pos) == wxSCI_LEX_FIF_FILE) {
		m_sci->ToggleFold(m_sci->LineFromPosition(pos));
		m_sci->SetCurrentPos(pos);
		m_sci->SetSelectionStart(pos);
		m_sci->SetSelectionEnd(pos);
	} else {
		DoMatchClicked(pos);
	}

}

void FindResultsTab::OnMouseDClick(wxScintillaEvent &event)
{
	DoMatchClicked(event.GetPosition());
}

void FindResultsTab::AppendText(const wxString& line)
{
	if (line.StartsWith(wxT("="))) {
		fifMatchInfo info;
		m_matchInfo.push_back(info);
		OutputTabWindow::AppendText(line);
	} else {
		wxString modLine(line);
		modLine.Replace(wxT("\r\n"), wxT("\n"));
		modLine.Replace(wxT("\r"), wxT("\n"));

		wxArrayString lines = wxStringTokenize(modLine, wxT("\n"), wxTOKEN_STRTOK);
		for (size_t i=0; i<lines.GetCount(); i++) {

			// extract the file name
			wxString singleLine (lines.Item(i));
			wxString fileName = singleLine.BeforeFirst(wxT('('));

			wxString lastFileName = m_matchInfo.empty() ? wxT("") : m_matchInfo.at(m_matchInfo.size()-1).file_name;

			// add entry which specifies to which file this match belongs to
			if (lastFileName != fileName) {
				fifMatchInfo info(fileName);
				m_matchInfo.push_back(info);

				OutputTabWindow::AppendText(fileName + wxT("\n"));
			}

			wxString locationInfoStr = singleLine.AfterFirst(wxT('('));
			locationInfoStr = locationInfoStr.BeforeFirst(wxT(')'));
			fifMatchInfo info(locationInfoStr, fileName);

			// set the file name
			m_matchInfo.push_back(info);

			// add the results
			wxString res = singleLine.AfterFirst(wxT(')'));
			res = res.AfterFirst(wxT(':'));

			res.Trim().Trim(false);
			wxString lineStr;
			lineStr << info.line_number;
			res.Prepend(wxT(" Ln ") + lineStr + wxT(": "));
			OutputTabWindow::AppendText(res + wxT("\n"));
		}
	}
}

void FindResultsTab::Clear()
{
	m_matchInfo.clear();
	OutputTabWindow::Clear();
}

void FindResultsTab::DefineMarker(int marker, int markerType, wxColor fore, wxColor back)
{
	m_sci->MarkerDefine(marker, markerType);
	m_sci->MarkerSetForeground(marker, fore);
	m_sci->MarkerSetBackground(marker, back);
}

void FindResultsTab::OnMarginClick(wxScintillaEvent& event)
{
	switch (event.GetMargin()) {
	case 4:
		//fold margin
	{
		int nLine = m_sci->LineFromPosition(event.GetPosition());
		m_sci->ToggleFold(nLine);
	}
	default:
		break;
	}
}

void FindResultsTab::DoMatchClicked(long pos)
{
	int line = m_sci->LineFromPosition(pos);
	wxString lineText = m_sci->GetLine(line);

	//remove selection
	m_sci->SetSelectionStart(pos);
	m_sci->SetSelectionEnd(pos);

	// each line has the format of
	// (line, col, len): text
	if (line > (int)m_matchInfo.size() || line < 0) {
		return;
	}

	wxString fileName = m_matchInfo.at(line).file_name;
	if (fileName.IsEmpty()) {
		return;
	}
	long lineNumber = m_matchInfo.at(line).line_number;
	long col = m_matchInfo.at(line).col;
	long matchLen = m_matchInfo.at(line).match_len;

	// open the file in the editor
	if (ManagerST::Get()->OpenFile(fileName, wxEmptyString, lineNumber - 1 )) {
		//select the matched result
		LEditor *editor = ManagerST::Get()->GetActiveEditor();
		if (editor) {
			if (col >= 0 && matchLen >= 0) {
				int offset = editor->PositionFromLine(lineNumber-1);
				editor->EnsureVisible(lineNumber-1);
				editor->SetSelection(offset + col, offset + col + matchLen);
			}
		}
	}
}

void FindResultsTab::OnCollapseAll(wxCommandEvent& e)
{
	// go through the whole document, toggling folds that are expanded
	int maxLine = m_sci->GetLineCount();
	for (int line = 0; line < maxLine; line++) {  // For every line
		int level = m_sci->GetFoldLevel(line);
		// The next statement means: If this level is a Fold start
		if (level & wxSCI_FOLDLEVELHEADERFLAG) {
			if ( m_sci->GetFoldExpanded(line) == true ) m_sci->ToggleFold( line );
		}
	}
}


//---------------------------------------------------------------------------------------------

void fifMatchInfo::FromString(const wxString& line, const wxString &fileName)
{
	wxString strLen = line.AfterFirst(wxT(',')).AfterFirst(wxT(',')).BeforeFirst(wxT(')'));
	wxString strCol = line.AfterFirst(wxT(',')).BeforeFirst(wxT(','));

	wxString strLineNumber = line.BeforeFirst(wxT(','));
	strLineNumber = strLineNumber.Trim();

	line_number = wxNOT_FOUND;
	match_len = wxNOT_FOUND;
	col = wxNOT_FOUND;
	file_name = fileName;

	strLineNumber.ToLong(&line_number);

	strLen = strLen.Trim().Trim(false);
	strLen.ToLong( &match_len );

	strCol = strCol.Trim().Trim(false);
	strCol.ToLong( &col );
}

fifMatchInfo::fifMatchInfo()
		: line_number(wxNOT_FOUND)
		, match_len(wxNOT_FOUND)
		, col(wxNOT_FOUND)
{
}

fifMatchInfo::fifMatchInfo(const wxString &locationInfoStr, const wxString &fileName)
{
	FromString(locationInfoStr, fileName);
}


fifMatchInfo::fifMatchInfo(const wxString& fileName)
		: file_name(fileName)
		, line_number(wxNOT_FOUND)
		, match_len(wxNOT_FOUND)
		, col(wxNOT_FOUND)
{
}
