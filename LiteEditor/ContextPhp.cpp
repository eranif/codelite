//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : context_html.cpp
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

#include "ContextPhp.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "cl_editor_tip_window.h"

ContextPhp::ContextPhp(clEditor* editor)
    : ContextGeneric(editor, "php")
{
    editor->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$"));
    m_completionTriggerStrings.insert("\\");
    m_completionTriggerStrings.insert("->");
    m_completionTriggerStrings.insert("::");
    SetName("php");
}

ContextPhp::ContextPhp()
    : ContextGeneric(wxT("php"))
{
}

ContextPhp::~ContextPhp() {}

void ContextPhp::AddMenuDynamicContent(wxMenu* menu) { wxUnusedVar(menu); }

void ContextPhp::ApplySettings()
{
    SetName(wxT("php"));
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

void ContextPhp::AutoIndent(const wxChar& nChar)
{
    clEditor& rCtrl = GetCtrl();
    int curpos = rCtrl.GetCurrentPos();

    if(rCtrl.GetDisableSmartIndent()) {
        return;
    }
    if(rCtrl.GetLineIndentation(rCtrl.GetCurrentLine()) && nChar == wxT('\n')) {
        int prevpos(wxNOT_FOUND);
        int foundPos(wxNOT_FOUND);

        wxString word;
        wxChar ch = rCtrl.PreviousChar(curpos, prevpos);
        wxUnusedVar(ch);
        word = rCtrl.PreviousWord(curpos, foundPos);

        // user hit ENTER after 'else'?
        if(word == wxT("else")) {
            int prevLine = rCtrl.LineFromPosition(prevpos);
            rCtrl.SetLineIndentation(rCtrl.GetCurrentLine(), rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
            rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(rCtrl.GetCurrentLine()));
            rCtrl.ChooseCaretX(); // set new column as "current" column
        }
        return;
    }

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

        // User typed 'ENTER' immediatly after colons ':'
        if(prevpos != wxNOT_FOUND && ch == wxT(':')) {
            int posWordBeforeColons(wxNOT_FOUND);

            rCtrl.PreviousChar(prevpos, posWordBeforeColons);
            if(posWordBeforeColons != wxNOT_FOUND) {
                word = rCtrl.PreviousWord(posWordBeforeColons, foundPos);
                int prevLine = rCtrl.LineFromPosition(posWordBeforeColons);

                // If we found one of the following keywords, un-indent their line by (foldLevel - 1)*indentSize
                if(word == wxT("public") || word == wxT("private") || word == wxT("protected")) {

                    ContextBase::AutoIndent(nChar);

                    // Indent this line according to the block indentation level
                    int foldLevel = (rCtrl.GetFoldLevel(prevLine) & wxSTC_FOLDLEVELNUMBERMASK) - wxSTC_FOLDLEVELBASE;
                    if(foldLevel) {
                        rCtrl.SetLineIndentation(prevLine, ((foldLevel - 1) * rCtrl.GetIndent()));
                        rCtrl.ChooseCaretX();
                    }
                    return;
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

wxString ContextPhp::CallTipContent() { return wxEmptyString; }

int ContextPhp::DoGetCalltipParamterIndex() { return ContextBase::DoGetCalltipParamterIndex(); }

wxString ContextPhp::GetCurrentScopeName() { return wxT(""); }

wxMenu* ContextPhp::GetMenu() { return ContextBase::GetMenu(); }

TagEntryPtr ContextPhp::GetTagAtCaret(bool scoped, bool impl) { return NULL; }

void ContextPhp::GotoPreviousDefintion() {}

bool ContextPhp::IsCommentOrString(long pos)
{
    int style = GetCtrl().GetStyleAt(pos);
    return IsComment(pos) || style == wxSTC_H_DOUBLESTRING || style == wxSTC_H_SINGLESTRING ||
        style == wxSTC_H_SGML_DOUBLESTRING || style == wxSTC_H_SGML_SIMPLESTRING || style == wxSTC_HJ_DOUBLESTRING ||
        style == wxSTC_HJ_SINGLESTRING || style == wxSTC_HJ_STRINGEOL || style == wxSTC_HJA_DOUBLESTRING ||
        style == wxSTC_HJA_SINGLESTRING || style == wxSTC_HJA_STRINGEOL || style == wxSTC_HB_STRING ||
        style == wxSTC_HBA_STRING || style == wxSTC_HBA_STRINGEOL || style == wxSTC_HP_STRING ||
        style == wxSTC_HPA_STRING || style == wxSTC_HPA_CHARACTER || style == wxSTC_HPHP_HSTRING ||
        style == wxSTC_HPHP_SIMPLESTRING;
}

bool ContextPhp::IsDefaultContext() const { return false; }

ContextBase* ContextPhp::NewInstance(clEditor* container) { return new ContextPhp(container); }

void ContextPhp::OnCallTipClick(wxStyledTextEvent& event) {}

void ContextPhp::OnCalltipCancel() {}

void ContextPhp::OnDbgDwellEnd(wxStyledTextEvent& event) {}

void ContextPhp::OnDbgDwellStart(wxStyledTextEvent& event) {}

void ContextPhp::OnDwellEnd(wxStyledTextEvent& event) {}

void ContextPhp::OnDwellStart(wxStyledTextEvent& event) {}

void ContextPhp::OnEnterHit() {}

void ContextPhp::OnFileSaved() {}

void ContextPhp::OnKeyDown(wxKeyEvent& event) { event.Skip(); }

void ContextPhp::OnSciUpdateUI(wxStyledTextEvent& event)
{
    clEditor& ctrl = GetCtrl();
    if(ctrl.GetFunctionTip()->IsActive()) {
        ctrl.GetFunctionTip()->Highlight(DoGetCalltipParamterIndex());
    }
}

void ContextPhp::RemoveMenuDynamicContent(wxMenu* menu) {}

void ContextPhp::RetagFile() {}

void ContextPhp::SemicolonShift()
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

void ContextPhp::SetActive() {}

bool ContextPhp::IsComment(long pos)
{
    int style = GetCtrl().GetStyleAt(pos);
    return style == wxSTC_H_COMMENT || style == wxSTC_H_XCCOMMENT || style == wxSTC_H_SGML_COMMENT ||
        style == wxSTC_HJ_COMMENT || style == wxSTC_HJ_COMMENTLINE || style == wxSTC_HJ_COMMENTDOC ||
        style == wxSTC_HJA_COMMENT || style == wxSTC_HJA_COMMENTLINE || style == wxSTC_HJA_COMMENTDOC ||
        style == wxSTC_HB_COMMENTLINE || style == wxSTC_HBA_COMMENTLINE || style == wxSTC_HP_COMMENTLINE ||
        style == wxSTC_HPA_COMMENTLINE || style == wxSTC_HPHP_COMMENT || style == wxSTC_HPHP_COMMENTLINE;
}

#define IS_BETWEEN(style, x, y) (style >= x && style <= y)

int ContextPhp::GetActiveKeywordSet() const
{
    // HTML 0,
    // JS 1,
    // Python 3
    // PHP 4
    int style = GetCtrl().GetStyleAt(GetCtrl().GetCurrentPos());
    if(IS_BETWEEN(style, wxSTC_H_DEFAULT, wxSTC_H_SGML_BLOCK_DEFAULT))
        return 0;
    else if(IS_BETWEEN(style, wxSTC_HJ_START, wxSTC_HJA_REGEX))
        return 1;
    else if(IS_BETWEEN(style, wxSTC_HPHP_DEFAULT, wxSTC_HPHP_OPERATOR) || style == wxSTC_HPHP_COMPLEX_VARIABLE)
        return 4;
    else if(IS_BETWEEN(style, wxSTC_HP_START, wxSTC_HP_IDENTIFIER) ||
        IS_BETWEEN(style, wxSTC_HPA_START, wxSTC_HPA_IDENTIFIER))
        return 3;
    return wxNOT_FOUND;
}

bool ContextPhp::IsAtBlockComment() const
{
    int curpos = GetCtrl().GetCurrentPos();
    int cur_style = GetCtrl().GetStyleAt(curpos);
    return cur_style == wxSTC_HPHP_COMMENT;
}

bool ContextPhp::IsAtLineComment() const
{
    int curpos = GetCtrl().GetCurrentPos();
    int cur_style = GetCtrl().GetStyleAt(curpos);
    return cur_style == wxSTC_HPHP_COMMENTLINE;
}

bool ContextPhp::IsStringTriggerCodeComplete(const wxString& str) const
{
    return (m_completionTriggerStrings.count(str) > 0);
}

void ContextPhp::ProcessIdleActions() { ContextGeneric::ProcessIdleActions(); }
