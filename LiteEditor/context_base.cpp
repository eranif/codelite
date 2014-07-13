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
#include <wx/xrc/xmlres.h>
#include "context_base.h"
#include "drawingutils.h"
#include <vector>
#include "editor_config.h"
#include "cl_editor.h"
#include "frame.h"
#include "ctags_manager.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "plugin.h"
#include "macros.h"
#include "commentconfigdata.h"
#include "editor_config.h"

//static wxColor GetInactiveColor(const wxColor& col)
//{
//    wxUnusedVar(col);
//#ifdef __WXGTK__
//    return wxColor(wxT("GREY"));
//#else
//    return wxColor(wxT("LIGHT GREY"));
//#endif
//}

ContextBase::ContextBase(LEditor *container)
    : m_container(container)
    , m_name(wxEmptyString)
{
}

ContextBase::ContextBase(const wxString &name)
    : m_name(name)
{
}

ContextBase::~ContextBase()
{
}

//provide basic indentation
void ContextBase::AutoIndent(const wxChar &ch)
{
    if (ch == wxT('\n')) {
        //just copy the previous line indentation
        LEditor &rCtrl = GetCtrl();
        int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
        rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line-1));
        //place the caret at the end of the line
        rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
        rCtrl.ChooseCaretX();
    }
}

void ContextBase::DoApplySettings(LexerConf::Ptr_t lexPtr)
{
    lexPtr->Apply( &GetCtrl() );
}

int ContextBase::GetHyperlinkRange(int pos, int &start, int &end)
{
    LEditor &rCtrl = GetCtrl();
    int curstyle = rCtrl.GetStyleAt(pos);
    if (curstyle == wxSTC_C_WORD2 || curstyle == wxSTC_C_GLOBALCLASS || curstyle == wxSTC_C_IDENTIFIER) {
        // get tag as hyperlink
        start = rCtrl.WordStartPos(pos, true);
        end = rCtrl.WordEndPos(pos, true);
        if (start < end)
            return XRCID("find_tag");
    }
    return wxID_NONE;
}

void ContextBase::GoHyperlink(int start, int end, int type, bool alt)
{
    wxUnusedVar(start);
    wxUnusedVar(end);
    wxUnusedVar(type);
    wxUnusedVar(alt);
}

wxMenu* ContextBase::GetMenu()
{
    return wxXmlResource::Get()->LoadMenu(wxT("editor_right_click_default"));
}

void ContextBase::PrependMenuItem(wxMenu *menu, const wxString &text, wxObjectEventFunction func, int eventId)
{
    wxMenuItem *item;
    wxString menuItemText;
    if( eventId == -1 )
        eventId = wxNewId();

    item = new wxMenuItem(menu, eventId, text);
    menu->Prepend(item);
    menu->Connect(eventId, wxEVT_COMMAND_MENU_SELECTED, func, NULL, this);
    m_dynItems.push_back(item);
}

void ContextBase::PrependMenuItem(wxMenu *menu, const wxString &text, int id)
{
    wxMenuItem *item;
    wxString menuItemText;
    item = new wxMenuItem(menu, id, text);
    menu->Prepend(item);
    m_dynItems.push_back(item);
}

void ContextBase::PrependMenuItemSeparator(wxMenu *menu)
{
    wxMenuItem *item;
    item = new wxMenuItem(menu, wxID_SEPARATOR);
    menu->Prepend(item);
    m_dynItems.push_back(item);
}

int ContextBase::DoGetCalltipParamterIndex()
{
    int index(0);
    LEditor &ctrl =  GetCtrl();
    int pos = ctrl.DoGetOpenBracePos();
    if (pos != wxNOT_FOUND) {

        // loop over the text from pos -> current position and count the number of commas found
        int depth(0);
        bool exit_loop(false);

        while ( pos < ctrl.GetCurrentPos() && !exit_loop ) {
            wxChar ch        = ctrl.SafeGetChar(pos);
            wxChar ch_before = ctrl.SafeGetChar(ctrl.PositionBefore(pos));

            if (IsCommentOrString(pos)) {
                pos = ctrl.PositionAfter(pos);
                continue;
            }

            switch (ch) {
            case wxT(','):
                if (depth == 0) index++;
                break;
            case wxT('{'):
            case wxT('}'):
            case wxT(';'):
                // error?
                exit_loop = true;
                break;
            case wxT('<'): 
                if ( ch_before == '<' ) {
                    // operator << 
                    // dont count this as depth ++
                    break;
                }
                // fall thru
            case wxT('('):
            case wxT('['):
                depth++;
                break;
                
            case wxT('>'):
                if ( ch_before == wxT('-') ) {
                    // operator noting to do
                    break;
                }
                // fall through
            case wxT(')'):
            case wxT(']'):
                depth--;
                break;
            default:
                break;
            }
            pos = ctrl.PositionAfter(pos);
        }
    }
    return index;
}

void ContextBase::OnUserTypedXChars(const wxString& word)
{
    // user typed more than 3 chars, display completion box with C++ keywords
    if ( IsCommentOrString(GetCtrl().GetCurrentPos()) ) {
        return;
    }

    TagEntryPtrVector_t tags;
    if (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_CPP_KEYWORD_ASISST) {
        clCodeCompletionEvent ccEvt(wxEVT_CC_CODE_COMPLETE_LANG_KEYWORD);
        ccEvt.SetEditor( &GetCtrl() );
        ccEvt.SetWord( word );

        if ( EventNotifier::Get()->ProcessEvent( ccEvt ) ) {
            tags = ccEvt.GetTags();

        } else if ( GetActiveKeywordSet() != wxNOT_FOUND ) {

            // the default action is to use the lexer keywords
            LexerConf::Ptr_t lexPtr;
            // Read the configuration file
            if (EditorConfigST::Get()->IsOk()) {
                lexPtr = EditorConfigST::Get()->GetLexer( GetName() );
            }

            if ( !lexPtr )
                return;

            wxString Words = lexPtr->GetKeyWords( GetActiveKeywordSet() );

            wxString s1(word);
            wxStringSet_t uniqueWords;
            wxArrayString wordsArr = ::wxStringTokenize(Words, wxT(" \r\t\n"));
            for (size_t i=0; i<wordsArr.GetCount(); i++) {

                // Dont add duplicate words
                if(uniqueWords.count(wordsArr.Item(i)))
                    continue;

                uniqueWords.insert(wordsArr.Item(i));
                wxString s2(wordsArr.Item(i));
                if (s2.StartsWith(s1) || s2.Lower().StartsWith(s1.Lower())) {
                    TagEntryPtr tag ( new TagEntry() );
                    tag->SetName(wordsArr.Item(i));
                    tag->SetKind("cpp_keyword");
                    tags.push_back(tag);
                }
            }
        }

        if ( tags.empty() == false ) {
            GetCtrl().ShowCompletionBox(tags,   // list of tags
                                        word,   // partial word
                                        false,  // dont show full declaration
                                        true,   // auto hide if there is no match in the list
                                        false); // do not automatically insert word if there is only single choice
        }
    }
}

void ContextBase::AutoAddComment()
{
    LEditor &rCtrl = GetCtrl();

    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    int curpos = rCtrl.GetCurrentPos();
    int line = rCtrl.LineFromPosition(curpos);
    int cur_style = rCtrl.GetStyleAt(curpos);
    wxString text = rCtrl.GetLine(line-1).Trim(false);

    bool dontadd = false;
    if ( IsAtLineComment() ) {
        dontadd = ! text.StartsWith( wxT("//") ) || !data.GetContinueCppComment();
    } else if ( IsAtBlockComment() ) {
        dontadd = !data.GetAddStarOnCComment();
        
    } else {
        dontadd = true;
    }
    
    if (dontadd) {
        ContextBase::AutoIndent(wxT('\n'));
        return;
    }

    wxString toInsert;
    if ( IsAtLineComment() ) {
        if ( text.StartsWith( wxT("//") ) ) {
            toInsert = wxT("// ");
        }
    } else if ( IsAtBlockComment() ) {
        if (rCtrl.GetStyleAt(rCtrl.PositionBefore(rCtrl.PositionBefore(curpos))) == cur_style) {
            toInsert = rCtrl.GetCharAt(rCtrl.GetLineIndentPosition(line-1)) == wxT('*') ? wxT("* ") : wxT(" * ");
        }
    }
    
    if ( !toInsert.IsEmpty() ) {
        rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line-1));
        int insertPos = rCtrl.GetLineIndentPosition(line);
        rCtrl.InsertText(insertPos, toInsert);
        rCtrl.SetCaretAt(insertPos + toInsert.Length());
        rCtrl.ChooseCaretX(); // set new column as "current" column
    }
}
