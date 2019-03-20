//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2015 Eran Ifrah
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

#include "ContextJavaScript.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "cl_editor_tip_window.h"

ContextJavaScript::ContextJavaScript(clEditor* editor)
    : ContextBase(editor)
{
    editor->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$"));
    m_completionTriggerStrings.insert(".");
    SetName("javascript");
}

ContextJavaScript::ContextJavaScript()
    : ContextBase(wxT("javascript"))
{
}

ContextJavaScript::~ContextJavaScript() {}

void ContextJavaScript::AddMenuDynamicContent(wxMenu* menu) { wxUnusedVar(menu); }

void ContextJavaScript::ApplySettings()
{
    SetName(wxT("javascript"));
    LexerConf::Ptr_t lexPtr;
    if(EditorConfigST::Get()->IsOk()) { lexPtr = EditorConfigST::Get()->GetLexer(GetName()); }
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

void ContextJavaScript::AutoIndent(const wxChar& nChar)
{
    clEditor& rCtrl = GetCtrl();

    if(rCtrl.GetDisableSmartIndent()) { return; }
    if(rCtrl.GetLineIndentation(rCtrl.GetCurrentLine()) && nChar == wxT('\n')) { return; }

    int curpos = rCtrl.GetCurrentPos();
    if(IsCommentOrString(curpos) && nChar == wxT('\n')) {
        AutoAddComment();
        return;
    }

    if(IsCommentOrString(curpos)) {
        ContextBase::AutoIndent(nChar);
        return;
    }
    int line = rCtrl.LineFromPosition(curpos);
    if(nChar == wxT('\n')) {

        int prevpos(wxNOT_FOUND);
        int foundPos(wxNOT_FOUND);

        wxString word;
        wxChar ch = rCtrl.PreviousChar(curpos, prevpos);
        word = rCtrl.PreviousWord(curpos, foundPos);

        // user hit ENTER after 'else'
        if(word == wxT("else")) {
            int prevLine = rCtrl.LineFromPosition(prevpos);
            rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
            rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
            rCtrl.ChooseCaretX(); // set new column as "current" column
            return;
        }

        // User typed 'ENTER' immediatly after closing brace ')'
        if(prevpos != wxNOT_FOUND && ch == wxT(')')) {

            long openBracePos(wxNOT_FOUND);
            int posWordBeforeOpenBrace(wxNOT_FOUND);

            if(rCtrl.MatchBraceBack(wxT(')'), prevpos, openBracePos)) {
                rCtrl.PreviousChar(openBracePos, posWordBeforeOpenBrace);
                if(posWordBeforeOpenBrace != wxNOT_FOUND) {
                    word = rCtrl.PreviousWord(posWordBeforeOpenBrace, foundPos);

                    // c++ expression with single line and should be treated separatly
                    if(word == wxT("if") || word == wxT("while") || word == wxT("for")) {
                        int prevLine = rCtrl.LineFromPosition(prevpos);
                        rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
                        rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
                        rCtrl.ChooseCaretX(); // set new column as "current" column
                        return;
                    }
                }
            }
        }

        // use the previous line indentation level
        if(prevpos == wxNOT_FOUND || ch != wxT('{') || IsCommentOrString(prevpos)) {
            ContextBase::AutoIndent(nChar);
            return;
        }

        // Open brace? increase indent size
        int prevLine = rCtrl.LineFromPosition(prevpos);
        rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
        rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));

    } else if(nChar == wxT('}')) {

        long matchPos = wxNOT_FOUND;
        if(!rCtrl.MatchBraceBack(wxT('}'), rCtrl.PositionBefore(curpos), matchPos)) return;
        int secondLine = rCtrl.LineFromPosition(matchPos);
        if(secondLine == line) return;
        rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(secondLine));

    } else if(nChar == wxT('{')) {
        wxString lineString = rCtrl.GetLine(line);
        lineString.Trim().Trim(false);

        int matchPos = wxNOT_FOUND;
        wxChar previousChar = rCtrl.PreviousChar(rCtrl.PositionBefore(curpos), matchPos);
        if(previousChar != wxT('{') && lineString == wxT("{")) {
            // indent this line accroding to the previous line
            int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
            rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line - 1));
            rCtrl.ChooseCaretX();
        }
    }

    // set new column as "current" column
    rCtrl.ChooseCaretX();
}

wxString ContextJavaScript::CallTipContent() { return wxEmptyString; }

int ContextJavaScript::DoGetCalltipParamterIndex() { return ContextBase::DoGetCalltipParamterIndex(); }

wxString ContextJavaScript::GetCurrentScopeName() { return wxT(""); }

wxMenu* ContextJavaScript::GetMenu() { return ContextBase::GetMenu(); }

TagEntryPtr ContextJavaScript::GetTagAtCaret(bool scoped, bool impl) { return NULL; }

void ContextJavaScript::GoHyperlink(int start, int end, int type, bool alt) {}

void ContextJavaScript::GotoPreviousDefintion() {}

bool ContextJavaScript::IsCommentOrString(long pos)
{
    int style = GetCtrl().GetStyleAt(pos);
    return IsComment(pos) || style == wxSTC_C_STRING || style == wxSTC_C_CHARACTER || style == wxSTC_C_STRINGEOL;
}

bool ContextJavaScript::IsDefaultContext() const { return false; }

ContextBase* ContextJavaScript::NewInstance(clEditor* container) { return new ContextJavaScript(container); }

void ContextJavaScript::OnCallTipClick(wxStyledTextEvent& event) {}

void ContextJavaScript::OnCalltipCancel() {}

void ContextJavaScript::OnDbgDwellEnd(wxStyledTextEvent& event) {}

void ContextJavaScript::OnDbgDwellStart(wxStyledTextEvent& event) {}

void ContextJavaScript::OnDwellEnd(wxStyledTextEvent& event) {}

void ContextJavaScript::OnDwellStart(wxStyledTextEvent& event) {}

void ContextJavaScript::OnEnterHit() {}

void ContextJavaScript::OnFileSaved() {}

void ContextJavaScript::OnKeyDown(wxKeyEvent& event) { event.Skip(); }

void ContextJavaScript::OnSciUpdateUI(wxStyledTextEvent& event)
{
    clEditor& ctrl = GetCtrl();
    if(ctrl.GetFunctionTip()->IsActive()) { ctrl.GetFunctionTip()->Highlight(DoGetCalltipParamterIndex()); }
}

void ContextJavaScript::RemoveMenuDynamicContent(wxMenu* menu) {}

void ContextJavaScript::RetagFile() {}

void ContextJavaScript::SemicolonShift()
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
            }
        }
    }
}

void ContextJavaScript::SetActive() {}

bool ContextJavaScript::IsComment(long pos)
{
    int style = GetCtrl().GetStyleAt(pos);
    return style == wxSTC_C_COMMENT || style == wxSTC_C_COMMENTDOC || style == wxSTC_C_COMMENTDOCKEYWORD ||
           style == wxSTC_C_COMMENTDOCKEYWORDERROR || style == wxSTC_C_COMMENTLINE || style == wxSTC_C_COMMENTLINEDOC;
}

#define IS_BETWEEN(style, x, y) (style >= x && style <= y)

int ContextJavaScript::GetActiveKeywordSet() const { return wxNOT_FOUND; }

bool ContextJavaScript::IsAtBlockComment() const
{
    int curpos = GetCtrl().GetCurrentPos();
    int cur_style = GetCtrl().GetStyleAt(curpos);
    return cur_style == wxSTC_C_COMMENT || cur_style == wxSTC_C_COMMENTDOC || cur_style == wxSTC_C_COMMENTDOCKEYWORD ||
           cur_style == wxSTC_C_COMMENTDOCKEYWORDERROR;
}

bool ContextJavaScript::IsAtLineComment() const
{
    int curpos = GetCtrl().GetCurrentPos();
    int cur_style = GetCtrl().GetStyleAt(curpos);
    return cur_style == wxSTC_C_COMMENTLINE || cur_style == wxSTC_C_COMMENTLINEDOC;
}
