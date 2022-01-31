//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2021 Eran Ifrah
// file name            : ContextRust.cpp
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
#include "wx/versioninfo.h"

// rust support was added in wx3.1
#if wxCHECK_VERSION(3, 1, 0)

#include "ContextRust.hpp"
#include "cl_editor.h"
#include "cl_editor_tip_window.h"
#include "editor_config.h"
#include <unordered_set>

ContextRust::ContextRust(clEditor* editor)
    : ContextGeneric(editor, "rust")
{
    editor->SetWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$");
    m_completionTriggerStrings.insert(".");
    m_completionTriggerStrings.insert("::");
    SetName("rust");
}

ContextRust::ContextRust()
    : ContextGeneric("rust")
{
}

ContextRust::~ContextRust() {}

void ContextRust::AddMenuDynamicContent(wxMenu* menu) { wxUnusedVar(menu); }

void ContextRust::ApplySettings()
{
    SetName(wxT("rust"));
    LexerConf::Ptr_t lexPtr;
    if(EditorConfigST::Get()->IsOk()) {
        lexPtr = EditorConfigST::Get()->GetLexer(GetName());
    }
    clEditor& rCtrl = GetCtrl();
    if(lexPtr) {
        rCtrl.SetLexer(lexPtr->GetLexerId());
        for(int i = 0; i <= 4; ++i) {
            wxString keyWords = lexPtr->GetKeyWords(i);
            keyWords.Replace(wxT("\n"), wxT(" "));
            keyWords.Replace(wxT("\r"), wxT(" "));
            rCtrl.SetKeyWords(i, keyWords);
        }
    } else {
        rCtrl.SetLexer(wxSTC_LEX_NULL);
    }
    DoApplySettings(lexPtr);
}

void ContextRust::AutoIndent(const wxChar& nChar) { ContextGeneric::AutoIndent(nChar); }

wxString ContextRust::CallTipContent() { return wxEmptyString; }

int ContextRust::DoGetCalltipParamterIndex() { return ContextBase::DoGetCalltipParamterIndex(); }

wxString ContextRust::GetCurrentScopeName() { return wxT(""); }

wxMenu* ContextRust::GetMenu() { return ContextBase::GetMenu(); }

TagEntryPtr ContextRust::GetTagAtCaret(bool scoped, bool impl) { return NULL; }

void ContextRust::GotoPreviousDefintion() {}

bool ContextRust::IsCommentOrString(long pos)
{
    static std::unordered_set<int> string_styles = { wxSTC_RUST_BYTECHARACTER, wxSTC_RUST_BYTESTRING,
                                                     wxSTC_RUST_BYTESTRINGR,   wxSTC_RUST_STRING,
                                                     wxSTC_RUST_STRINGR,       wxSTC_RUST_CHARACTER };
    int style = GetCtrl().GetStyleAt(pos);
    return IsComment(pos) || string_styles.count(style);
}

bool ContextRust::IsDefaultContext() const { return false; }

ContextBase* ContextRust::NewInstance(clEditor* container) { return new ContextRust(container); }

void ContextRust::OnCallTipClick(wxStyledTextEvent& event) { wxUnusedVar(event); }

void ContextRust::OnCalltipCancel() {}

void ContextRust::OnDbgDwellEnd(wxStyledTextEvent& event) { wxUnusedVar(event); }

void ContextRust::OnDbgDwellStart(wxStyledTextEvent& event) { wxUnusedVar(event); }

void ContextRust::OnDwellEnd(wxStyledTextEvent& event) { wxUnusedVar(event); }

void ContextRust::OnDwellStart(wxStyledTextEvent& event) { wxUnusedVar(event); }

void ContextRust::OnEnterHit() {}

void ContextRust::OnFileSaved() {}

void ContextRust::OnKeyDown(wxKeyEvent& event) { event.Skip(); }

void ContextRust::OnSciUpdateUI(wxStyledTextEvent& event)
{
    clEditor& ctrl = GetCtrl();
    if(ctrl.GetFunctionTip()->IsActive()) {
        ctrl.GetFunctionTip()->Highlight(DoGetCalltipParamterIndex());
    }
}

void ContextRust::RemoveMenuDynamicContent(wxMenu* menu) {}

void ContextRust::RetagFile() {}

void ContextRust::SemicolonShift()
{
    int foundPos(wxNOT_FOUND);
    int semiColonPos(wxNOT_FOUND);
    clEditor& ctrl = GetCtrl();
    if(ctrl.NextChar(ctrl.GetCurrentPos(), semiColonPos) == wxT(')')) {

        // test to see if we are inside a 'for' statement
        long openBracePos(wxNOT_FOUND);
        int posWordBeforeOpenBrace(wxNOT_FOUND);

        if(ctrl.MatchBraceBack(wxT(')'), semiColonPos, openBracePos)) {
            ctrl.PreviousChar(openBracePos, posWordBeforeOpenBrace);
            if(posWordBeforeOpenBrace != wxNOT_FOUND) {
                wxString word = ctrl.PreviousWord(posWordBeforeOpenBrace, foundPos);

                // At the current pos, we got a ';'
                // at semiColonPos we got ;
                // switch
                ctrl.DeleteBack();
                ctrl.SetCurrentPos(semiColonPos);
                ctrl.InsertText(semiColonPos, wxT(";"));
                ctrl.SetCaretAt(semiColonPos + 1);
                ctrl.GetFunctionTip()->Deactivate();
            }
        }
    }
}

void ContextRust::SetActive() {}

bool ContextRust::IsComment(long pos)
{
    static std::unordered_set<int> comment_styles = { wxSTC_RUST_COMMENTBLOCK, wxSTC_RUST_COMMENTLINE,
                                                      wxSTC_RUST_COMMENTBLOCKDOC, wxSTC_RUST_COMMENTLINEDOC };
    int style = GetCtrl().GetStyleAt(pos);
    return comment_styles.count(style);
}

#define IS_BETWEEN(style, x, y) (style >= x && style <= y)

int ContextRust::GetActiveKeywordSet() const { return 0; }

bool ContextRust::IsAtBlockComment() const
{
    int pos = PositionBeforeCurrent();
    int cur_style = GetCtrl().GetStyleAt(pos);
    return cur_style == wxSTC_RUST_COMMENTBLOCK || cur_style == wxSTC_RUST_COMMENTBLOCKDOC;
}

bool ContextRust::IsAtLineComment() const
{
    int pos = PositionBeforeCurrent();
    int cur_style = GetCtrl().GetStyleAt(pos);
    return cur_style == wxSTC_RUST_COMMENTLINE || cur_style == wxSTC_RUST_COMMENTLINEDOC;
}

bool ContextRust::IsStringTriggerCodeComplete(const wxString& str) const
{
    return (m_completionTriggerStrings.count(str) > 0);
}

void ContextRust::ProcessIdleActions() { ContextGeneric::ProcessIdleActions(); }

#endif // wxVERSION_NUMBER >= 3100
