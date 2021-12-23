//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : addincludefiledlg.cpp
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
#include "addincludefiledlg.h"

#include "bitmap_loader.h"
#include "editor_config.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include "wx/filename.h"
#include "wx/regex.h"

#include <algorithm>
#include <wx/wupdlock.h>

wxArrayString AddIncludeFileDlg::m_includePath;

struct SAscendingSort {
    bool operator()(const wxString& s1, const wxString& s2) { return s1.Len() > s2.Len(); }
};

AddIncludeFileDlg::AddIncludeFileDlg(wxWindow* parent, const wxString& fullpath, const wxString& text, int lineNo)
    : AddIncludeFileDlgBase(parent)
    , m_fullpath(fullpath)
    , m_text(text)
    , m_line(lineNo)
{
    if(m_fullpath.StartsWith("\"") || m_fullpath.StartsWith("<")) {
        m_lineToAdd = "#include " + m_fullpath;
        m_isLineToAddProvided = true;
    }

    // Initialise the preview window
    UpdateLineToAdd();
    LexerConf::Ptr_t cppLex = EditorConfigST::Get()->GetLexer("C++");
    cppLex->Apply(m_textCtrlPreview, true);

    clBitmapList* bitmaps = new clBitmapList;
    m_toolbar->AddTool(wxID_UP, _("Move Up"), bitmaps->Add("up"));
    m_toolbar->AddTool(wxID_DOWN, _("Move Up"), bitmaps->Add("down"));
    m_toolbar->AddTool(wxID_CLEAR, _("Clear"), bitmaps->Add("clear"));
    m_toolbar->AssignBitmaps(bitmaps);

    m_toolbar->Bind(wxEVT_TOOL, &AddIncludeFileDlg::OnButtonUp, this, wxID_UP);
    m_toolbar->Bind(wxEVT_TOOL, &AddIncludeFileDlg::OnButtonDown, this, wxID_DOWN);
    m_toolbar->Bind(wxEVT_TOOL, &AddIncludeFileDlg::OnClearCachedPaths, this, wxID_CLEAR);
    m_toolbar->Realize();

    //---------------------------------------------------------
    m_textCtrlPreview->MarkerDefine(0x7, wxSTC_MARK_ARROW);
    m_textCtrlPreview->MarkerSetBackground(0x7, wxT("YELLOW GREEN"));
    m_textCtrlPreview->MarkerSetAlpha(0x7, 70);

    m_textCtrlPreview->Bind(wxEVT_KEY_DOWN, &AddIncludeFileDlg::OnPreviewKeyDown, this);

    // Set the initial text
    m_textCtrlPreview->SetReadOnly(false);
    m_textCtrlPreview->AddText(m_text);
    SetAndMarkLine();
    m_textCtrlPreview->EmptyUndoBuffer();
    m_textCtrlPreview->SetFocus();

    // Only call OnModified when text was deleted or added
    Bind(wxEVT_IDLE, &AddIncludeFileDlg::OnIdle, this);

    int numOfLinesVisible = m_textCtrlPreview->LinesOnScreen();
    int firstVisibleLine = m_line - (numOfLinesVisible / 2);
    if(firstVisibleLine < 0) {
        firstVisibleLine = 0;
    }
    m_textCtrlPreview->SetFirstVisibleLine(firstVisibleLine);
    ::clSetDialogBestSizeAndPosition(this);
}

AddIncludeFileDlg::~AddIncludeFileDlg() {}

void AddIncludeFileDlg::UpdateLineToAdd()
{
    if(m_isLineToAddProvided) {
        m_textCtrlFullPath->ChangeValue(m_fullpath);
        return;
    }

    wxFileName fn(m_fullpath);
    m_textCtrlFullPath->ChangeValue(fn.GetFullPath());

    wxString line;
    // try to get a match in the include path for this file
    wxString pp = fn.GetFullPath();
    pp.Replace(wxT("\\"), wxT("/"));

    wxString rest;
    for(size_t i = 0; i < m_includePath.GetCount(); i++) {
        if(pp.StartsWith(m_includePath.Item(i), &rest)) {
            break;
        }
    }

    if(rest.IsEmpty()) {
        rest = fn.GetFullName();
    }

    wxString errMsg;
    wxString projectName = clGetManager()->GetActiveEditor()->GetProjectName();
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(projectName, errMsg);
    if(proj) {
        wxArrayString incls = proj->GetIncludePaths();
        std::sort(incls.begin(), incls.end(), SAscendingSort());

        for(size_t i = 0; i < incls.GetCount(); i++) {
            wxString path = incls.Item(i);
#ifdef __WXMSW__
            path.MakeLower();
#endif
            if(m_fullpath.StartsWith(path, &rest)) {
                break;
            }
        }
    }

    rest.Replace(wxT("\\"), wxT("/"));
    if(rest.StartsWith(wxT("/"))) {
        rest.Remove(0, 1);
    }

    wxString workspaceDir = clCxxWorkspaceST::Get()->GetFileName().GetPath();
    bool isSystemHeader = !m_fullpath.StartsWith(workspaceDir);

    wxString delimopen = isSystemHeader ? "<" : "\"";
    wxString delimclose = isSystemHeader ? ">" : "\"";
    line << "#include " << delimopen << rest << delimclose;
    m_lineToAdd = line;
}

void AddIncludeFileDlg::SetAndMarkLine()
{
// restore the initial text
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(m_textCtrlPreview);
#endif

    m_textCtrlPreview->SetReadOnly(false);

    // Make the line to add at the center of the display
    int numOfLinesVisible = m_textCtrlPreview->LinesOnScreen();
    int firstVisibleLine = m_line - (numOfLinesVisible / 2);
    if(firstVisibleLine < 0) {
        firstVisibleLine = 0;
    }

    m_textCtrlPreview->MarkerDeleteAll(0x7);
    m_textCtrlPreview->SetText(m_text);
    long pos = m_textCtrlPreview->PositionFromLine(m_line);
    m_textCtrlPreview->InsertText(pos, m_lineToAdd + wxT("\n"));
    m_textCtrlPreview->MarkerAdd(m_line, 0x7);
    m_textCtrlPreview->SetFirstVisibleLine(firstVisibleLine);
    m_textCtrlPreview->SetCurrentPos(pos);
    m_textCtrlPreview->SetSelectionStart(pos);
    m_textCtrlPreview->SetSelectionEnd(pos);
    m_textCtrlPreview->EmptyUndoBuffer();
    m_textCtrlPreview->SetReadOnly(true);
}

void AddIncludeFileDlg::OnTextUpdated(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SetAndMarkLine();
}

void AddIncludeFileDlg::OnButtonDown(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_line + 2 >= m_textCtrlPreview->GetLineCount()) {
        return;
    }
    m_line++;
    SetAndMarkLine();
}

void AddIncludeFileDlg::OnButtonUp(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_line - 1 < 0) {
        return;
    }
    m_line--;
    SetAndMarkLine();
}

void AddIncludeFileDlg::OnButtonOK(wxCommandEvent& e)
{
    wxUnusedVar(e);
    // get the include file to add
    wxString fullpath = m_textCtrlFullPath->GetValue();
    static wxRegEx reIncludeFile(wxT("include *[\\\"\\<]{1}([a-zA-Z0-9_/\\.]*)"));
    wxString relativePath;

    if(reIncludeFile.Matches(m_lineToAdd)) {
        relativePath = reIncludeFile.GetMatch(m_lineToAdd, 1);
    }

    fullpath.Replace(wxT("\\"), wxT("/"));
    relativePath.Replace(wxT("\\"), wxT("/"));
    wxFileName fn(fullpath);

    wxString inclPath;
    if(fullpath.EndsWith(relativePath, &inclPath) &&
       fullpath != relativePath &&         // don't save the '.' path this is done by default
       fn.GetFullName() != relativePath) { // if the relative path is only file name, nothing to cache
        m_includePath.Add(inclPath);
    }
    EndModal(wxID_OK);
}

void AddIncludeFileDlg::OnClearCachedPaths(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_includePath.Clear();
    UpdateLineToAdd();
    SetAndMarkLine();
}

void AddIncludeFileDlg::OnPreviewKeyDown(wxKeyEvent& event)
{
    event.Skip(false);
    m_textCtrlPreview->SetEditable(false);
    bool isOnMarkerLine = m_line == m_textCtrlPreview->GetCurrentLine();
    wxCommandEvent dummy;
    if(event.GetKeyCode() == WXK_DOWN) {
        // update the line to add string and move the line down
        // m_lineToAdd = m_textCtrlPreview->GetLine(m_line);
        OnButtonDown(dummy);

    } else if(event.GetKeyCode() == WXK_UP) {
        // update the line to add string and move the line up
        // m_lineToAdd = m_textCtrlPreview->GetLine(m_line);
        OnButtonUp(dummy);
    } else if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        OnButtonOK(dummy);

    } else if(isOnMarkerLine) {
        m_textCtrlPreview->SetEditable(true);
        event.Skip();
    }
}

void AddIncludeFileDlg::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    if(m_textCtrlPreview->MarkerNext(wxNOT_FOUND, (1 << 0x7)) != wxNOT_FOUND) {
        // we have a marker
        m_lineToAdd = m_textCtrlPreview->GetLine(m_line);
        m_lineToAdd.Trim();

        if(m_staticTextPreview->GetLabel() != m_lineToAdd) {
            m_staticTextPreview->CallAfter(&wxStaticText::SetLabel, m_lineToAdd);
        }
    }
}
