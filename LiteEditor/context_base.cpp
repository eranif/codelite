//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_base.cpp
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
#include "context_base.h"

#include "ServiceProviderManager.h"
#include "cl_command_event.h"
#include "cl_editor.h"
#include "commentconfigdata.h"
#include "ctags_manager.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "frame.h"
#include "macros.h"
#include "plugin.h"

#include <vector>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

ContextBase::ContextBase(clEditor* container)
    : m_container(container)
    , m_name(wxEmptyString)
{
}

ContextBase::ContextBase(const wxString& name)
    : m_name(name)
{
}

ContextBase::~ContextBase() {}

// provide basic indentation
void ContextBase::AutoIndent(const wxChar& ch)
{
    clEditor& rCtrl = GetCtrl();
    int prevpos(wxNOT_FOUND);
    int curpos = rCtrl.GetCurrentPos();
    int line = rCtrl.LineFromPosition(curpos);

    if(ch == wxT('\n')) {
        wxChar prevCh = rCtrl.PreviousChar(curpos, prevpos);
        if(prevCh == '{') {
            // an enter was hit just after an open brace
            int prevLine = rCtrl.LineFromPosition(prevpos);
            rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
            rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));

        } else {
            // just copy the previous line indentation
            int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
            rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line - 1));
            // place the caret at the end of the line
            rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
            rCtrl.ChooseCaretX();
        }
    } else if(ch == '}' && !IsCommentOrString(curpos)) {
        long matchPos = wxNOT_FOUND;
        if(!rCtrl.MatchBraceBack(wxT('}'), rCtrl.PositionBefore(curpos), matchPos))
            return;
        int secondLine = rCtrl.LineFromPosition(matchPos);
        if(secondLine == line)
            return;
        rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(secondLine));
    }
}

void ContextBase::DoApplySettings(LexerConf::Ptr_t lexPtr) { lexPtr->Apply(&GetCtrl()); }

bool ContextBase::GetHyperlinkRange(int& start, int& end)
{
    clEditor& rCtrl = GetCtrl();
    wxPoint pt = wxGetMousePosition();
    if(pt == wxDefaultPosition) {
        return false;
    }
    wxPoint clientPt = rCtrl.ScreenToClient(pt);
    int mouse_pos = rCtrl.PositionFromPointClose(clientPt.x, clientPt.y);
    if(!IsCommentOrString(mouse_pos)) {
        // get tag as hyperlink
        start = rCtrl.WordStartPos(mouse_pos, true);
        end = rCtrl.WordEndPos(mouse_pos, true);
        if(start < end)
            return true;
    }
    return false;
}

wxMenu* ContextBase::GetMenu() { return wxXmlResource::Get()->LoadMenu(wxT("editor_right_click_default")); }

void ContextBase::PrependMenuItem(wxMenu* menu, const wxString& text, wxObjectEventFunction func, int eventId)
{
    wxMenuItem* item;
    wxString menuItemText;
    if(eventId == -1)
        eventId = wxNewId();

    item = new wxMenuItem(menu, eventId, text);
    menu->Prepend(item);
    menu->Connect(eventId, wxEVT_COMMAND_MENU_SELECTED, func, NULL, this);
    m_dynItems.push_back(item);
}

void ContextBase::PrependMenuItem(wxMenu* menu, const wxString& text, int id)
{
    wxMenuItem* item;
    wxString menuItemText;
    item = new wxMenuItem(menu, id, text);
    menu->Prepend(item);
    m_dynItems.push_back(item);
}

void ContextBase::PrependMenuItemSeparator(wxMenu* menu)
{
    wxMenuItem* item;
    item = new wxMenuItem(menu, wxID_SEPARATOR);
    menu->Prepend(item);
    m_dynItems.push_back(item);
}

int ContextBase::DoGetCalltipParamterIndex()
{
    int index(0);
    clEditor& ctrl = GetCtrl();
    int pos = ctrl.DoGetOpenBracePos();
    if(pos != wxNOT_FOUND) {

        // loop over the text from pos -> current position and count the number of commas found
        int depth(0);
        while(pos < ctrl.GetCurrentPos()) {
            wxChar ch = ctrl.SafeGetChar(pos);
            // wxChar ch_before = ctrl.SafeGetChar(ctrl.PositionBefore(pos));

            if(IsCommentOrString(pos)) {
                pos = ctrl.PositionAfter(pos);
                continue;
            }

            switch(ch) {
            case wxT(','):
                if(depth == 0)
                    index++;
                break;
            case wxT('('):
                depth++;
                break;
            case wxT(')'):
                depth--;
                break;
            default:
                break;
            }
            pos = ctrl.PositionAfter(pos);
        }
    } else {
        return wxNOT_FOUND;
    }
    return index;
}

void ContextBase::OnUserTypedXChars(const wxString& word)
{
    // user typed more than X chars
    // trigger code complete event (as if the user typed ctrl-space)
    // if no one handles this event, fire a word completion event
    if(IsCommentOrString(GetCtrl().GetCurrentPos())) {
        return;
    }

    // Try to call code completion
    clCodeCompletionEvent ccEvt(wxEVT_CC_CODE_COMPLETE);
    ccEvt.SetInsideCommentOrString(IsCommentOrString(GetCtrl().GetCurrentPos()));
    ccEvt.SetTriggerKind(LSP::CompletionItem::kTriggerKindInvoked);
    ccEvt.SetFileName(GetCtrl().GetFileName().GetFullPath());
    ccEvt.SetWord(word);

    if(!ServiceProviderManager::Get().ProcessEvent(ccEvt)) {
        // This is ugly, since CodeLite should not be calling
        // the plugins... we take comfort in the fact that it
        // merely fires an event and not calling it directly
        wxCommandEvent wordCompleteEvent(wxEVT_MENU, XRCID("simple_word_completion"));
        EventNotifier::Get()->TopFrame()->GetEventHandler()->ProcessEvent(wordCompleteEvent);
    }
}

void ContextBase::AutoAddComment()
{
    clEditor& rCtrl = GetCtrl();

    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    int curpos = rCtrl.GetCurrentPos();
    int line = rCtrl.LineFromPosition(curpos);
    int cur_style = rCtrl.GetStyleAt(curpos);
    wxString text = rCtrl.GetLine(line - 1).Trim(false);

    bool dontadd = false;
    if(IsAtLineComment()) {
        dontadd = !text.StartsWith(wxT("//")) || !data.GetContinueCppComment();
    } else if(IsAtBlockComment()) {
        dontadd = !data.GetAddStarOnCComment();

    } else {
        dontadd = true;
    }

    if(dontadd) {
        ContextBase::AutoIndent(wxT('\n'));
        return;
    }

    wxString toInsert;
    if(IsAtLineComment()) {
        if(text.StartsWith(wxT("//"))) {
            toInsert = wxT("// ");
        }
    } else if(IsAtBlockComment()) {
        // Check the text typed before this char
        int startPos = rCtrl.PositionBefore(curpos);
        startPos -= 3; // for "/**"
        if(startPos >= 0) {
            wxString textTyped = rCtrl.GetTextRange(startPos, rCtrl.PositionBefore(curpos));
            if(((textTyped == "/**") || (textTyped == "/*!")) && data.IsAutoInsert()) {
                // Let the plugins/codelite check if they can provide a doxy comment
                // for the current entry
                clCodeCompletionEvent event(wxEVT_CC_GENERATE_DOXY_BLOCK);
                event.SetFileName(GetCtrl().GetFileName().GetFullPath());
                if(EventNotifier::Get()->ProcessEvent(event) && !event.GetTooltip().IsEmpty()) {
                    rCtrl.BeginUndoAction();

                    // To make the doxy block fit in, we need to prepend each line
                    // with the exact whitespace of the line that starts with "/**"
                    int lineStartPos = rCtrl.PositionFromLine(rCtrl.LineFromPos(startPos));
                    wxString whitespace = rCtrl.GetTextRange(lineStartPos, startPos);
                    // Break the comment, for each line, prepend the 'whitespace' buffer
                    wxArrayString lines = ::wxStringTokenize(event.GetTooltip(), "\n", wxTOKEN_STRTOK);
                    for(size_t i = 0; i < lines.GetCount(); ++i) {
                        if(i) { // don't add it to the first line (it already exists in the editor)
                            lines.Item(i).Prepend(whitespace);
                        }
                    }

                    // Join the lines back
                    wxString doxyBlock = ::clJoinLinesWithEOL(lines, rCtrl.GetEOL());

                    rCtrl.SetSelection(startPos, curpos);
                    rCtrl.ReplaceSelection(doxyBlock);

                    // Try to place the caret after the @brief
                    wxRegEx reBrief("[@\\]brief[ \t]*");
                    if(reBrief.IsValid() && reBrief.Matches(doxyBlock)) {
                        wxString match = reBrief.GetMatch(doxyBlock);
                        // Get the index
                        int where = doxyBlock.Find(match);
                        if(where != wxNOT_FOUND) {
                            where += match.length();
                            int caretPos = startPos + where;
                            rCtrl.SetCaretAt(caretPos);

                            // Remove the @brief as its non standard in the PHP world
                            rCtrl.DeleteRange(caretPos - match.length(), match.length());
                        }
                    }
                    rCtrl.EndUndoAction();
                    return;
                }
            }
        }

        if(rCtrl.GetStyleAt(rCtrl.PositionBefore(rCtrl.PositionBefore(curpos))) == cur_style) {
            toInsert = rCtrl.GetCharAt(rCtrl.GetLineIndentPosition(line - 1)) == wxT('*') ? wxT("* ") : wxT(" * ");
        }
    }

    if(!toInsert.IsEmpty()) {
        rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line - 1));
        int insertPos = rCtrl.GetLineIndentPosition(line);
        rCtrl.InsertText(insertPos, toInsert);
        rCtrl.SetCaretAt(insertPos + toInsert.Length());
        rCtrl.ChooseCaretX(); // set new column as "current" column
    }
}

bool ContextBase::IsStringTriggerCodeComplete(const wxString& str) const
{
    // default behavior is to check if 'str' exists in the m_completionTriggerStrings container
    if(!m_completionTriggerStrings.empty()) {
        return m_completionTriggerStrings.count(str) > 0;
    }

    if(GetCtrl().GetLexer() == wxSTC_LEX_XML) {
        return str == "<" || str == "</";
    } else if(GetCtrl().GetLexer() == wxSTC_LEX_CSS) {
        return str == ":";
    } else if(GetCtrl().GetLexer() == wxSTC_LEX_PYTHON) {
        return str == ".";
    } else if((GetCtrl().GetLexer() == wxSTC_LEX_CPP)) {
        // This can happen for Java
        return str == "." || str == "::";
    } else {
        return false;
    }
}

int ContextBase::FindNext(const wxString& what, int& pos, bool wholePage)
{
    wxStyledTextCtrl* ctrl = GetCtrl().GetCtrl();
    int startpos(wxNOT_FOUND);
    int lastLine(wxNOT_FOUND);
    int endpos(wxNOT_FOUND);
    if(wholePage) {
        startpos = ctrl->GetCurrentPos();
        endpos = ctrl->GetLastPosition();
    } else {
        startpos = ctrl->PositionFromLine(ctrl->GetFirstVisibleLine());
        lastLine = ctrl->GetFirstVisibleLine() + ctrl->LinesOnScreen();
        endpos = ctrl->GetLineEndPosition(lastLine);
    }
    if((pos < startpos) || (pos > endpos))
        return wxNOT_FOUND;
    int where = ctrl->FindText(pos, endpos, what);
    if(where != wxNOT_FOUND) {
        pos = where + what.length();
    }
    return where;
}

int ContextBase::FindPrev(const wxString& what, int& pos, bool wholePage)
{
    wxStyledTextCtrl* ctrl = GetCtrl().GetCtrl();
    int startpos(wxNOT_FOUND);
    int lastLine(wxNOT_FOUND);
    int endpos(wxNOT_FOUND);
    if(wholePage) {
        startpos = 0;
        endpos = ctrl->GetCurrentPos();
    } else {
        startpos = ctrl->PositionFromLine(ctrl->GetFirstVisibleLine());
        lastLine = ctrl->GetFirstVisibleLine() + ctrl->LinesOnScreen();
        endpos = ctrl->GetLineEndPosition(lastLine);
    }
    if((pos < startpos) || (pos > endpos))
        return wxNOT_FOUND;
    int where = ctrl->FindText(pos, startpos, what);
    if(where != wxNOT_FOUND) {
        pos = where;
    }
    return where;
}

void ContextBase::BlockCommentComplete()
{
    wxStyledTextCtrl* stc = GetCtrl().GetCtrl();
    int curPos = stc->GetCurrentPos();
    int start = stc->WordStartPosition(stc->GetCurrentPos(), true);
    if(curPos < start)
        return;

    // Fire an event indicating user typed '@' in a block comment
    clCodeCompletionEvent ccEvent(wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE);
    ccEvent.SetFileName(GetCtrl().GetFileName().GetFullPath());
    EventNotifier::Get()->AddPendingEvent(ccEvent);
}

int ContextBase::PositionBeforeCurrent() const
{
    int curpos = GetCtrl().GetCurrentPos();
    if(curpos == 0) {
        return 0;
    }
    return GetCtrl().PositionBefore(curpos);
}