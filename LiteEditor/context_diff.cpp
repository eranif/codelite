//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_diff.cpp
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
#include <wx/xrc/xmlres.h>
#include <wx/regex.h>
#include "frame.h"
#include "manager.h"
#include "editor_config.h"
#include "cl_editor.h"
#include "context_diff.h"

ContextDiff::ContextDiff()
    : ContextBase(wxT("Diff"))
{
}

ContextDiff::ContextDiff(clEditor *container)
    : ContextBase(container)
{
    SetName(wxT("diff"));
    ApplySettings();
}

ContextDiff::~ContextDiff()
{
}

ContextBase* ContextDiff::NewInstance(clEditor* container)
{
    return new ContextDiff(container);
}

void ContextDiff::ApplySettings()
{
	LexerConf::Ptr_t lexPtr;
	if (EditorConfigST::Get()->IsOk()) {
		lexPtr = EditorConfigST::Get()->GetLexer(GetName());
	}
	GetCtrl().SetLexer(lexPtr ? lexPtr->GetLexerId() : wxSTC_LEX_NULL);
	DoApplySettings(lexPtr);
}

int ContextDiff::GetHyperlinkRange(int pos, int& start, int& end)
{
    clEditor &rCtrl = GetCtrl();
    int lineNum = rCtrl.LineFromPosition(pos);
    wxString line = rCtrl.GetLine(lineNum);
    wxString fileName;
    if (line.StartsWith(wxT("Index: "), &fileName) || line.StartsWith(wxT("--- "), &fileName) || line.StartsWith(wxT("+++ "), &fileName)) {
        fileName = fileName.BeforeFirst(wxT('(')).Trim(false).Trim();
        start = rCtrl.PositionFromLine(lineNum) + line.find(fileName);
        end = start + fileName.size();
        return start <= pos && pos <= end ? XRCID("open_working_copy") : wxID_NONE;
    }
    wxString edit;
    if (line.StartsWith(wxT("@@ ")) && line.Trim().EndsWith(wxT(" @@"))) {
        start = rCtrl.PositionFromLine(lineNum);
        end = rCtrl.GetLineEndPosition(lineNum);
        return XRCID("open_at_line");
    }
    return wxID_NONE;
}

void ContextDiff::GoHyperlink(int start, int end, int type, bool alt)
{
    clEditor &rCtrl = GetCtrl();

    wxString text = rCtrl.GetTextRange(start, end);
    wxString fileName;
    int lineNum = wxNOT_FOUND;

    if (type == XRCID("open_working_copy")) {
        fileName = text;
    } else if (type == XRCID("open_at_line")) {
        wxRegEx re(wxT("@@ -[0-9]+,[0-9]+ \\+([0-9]+),[0-9]+ @@"));
        size_t start, len;
        long line;
        if (re.IsValid() && re.Matches(text) && re.GetMatch(&start, &len, 1) && text.Mid(start,len).ToLong(&line)) {
            lineNum = line;
        }
        // FIXME: search backwards for "+++ filename", since this could be a directory diff
        rCtrl.GetFileName().GetFullName().EndsWith(wxT(".diff"), &fileName);
    }
	
    wxFileName fn = ManagerST::Get()->FindFile(fileName);
    if (fn.IsOk()) {
        clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString, lineNum);
    }
}

