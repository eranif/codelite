//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : generic_context.cpp
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
#include "generic_context.h"
#include "editor_config.h"
#include "cl_editor.h"
#include "clEditorWordCharsLocker.h"
#include "clEditorColouriseLocker.h"
#include "file_logger.h"

ContextGeneric::ContextGeneric(LEditor* container, const wxString& name)
    : ContextBase(container)
{
    SetName(name);
    ApplySettings();
}

ContextGeneric::~ContextGeneric() {}

ContextBase* ContextGeneric::NewInstance(LEditor* container) { return new ContextGeneric(container, GetName()); }

void ContextGeneric::ApplySettings()
{
    LexerConf::Ptr_t lexPtr;
    if(EditorConfigST::Get()->IsOk()) {
        lexPtr = EditorConfigST::Get()->GetLexer(GetName());
    }
    LEditor& rCtrl = GetCtrl();
    if(lexPtr) {
        rCtrl.SetLexer(lexPtr->GetLexerId());
        for(int i = 0; i <= 4; ++i) {
            wxString keyWords = lexPtr->GetKeyWords(i);
            keyWords.Replace(wxT("\n"), wxT(" "));
            keyWords.Replace(wxT("\r"), wxT(" "));
            rCtrl.SetKeyWords(i, keyWords);
        }

        if(lexPtr->GetName() == "css") {
            // set the word characters for the CSS lexer
            GetCtrl().SetWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-@");
        }
    } else {
        rCtrl.SetLexer(wxSTC_LEX_NULL);
    }
    DoApplySettings(lexPtr);
}

void ContextGeneric::ProcessIdleActions()
{
    LEditor& ctrl = GetCtrl();
    if((ctrl.GetLexerId() == wxSTC_LEX_XML) || (ctrl.GetLexerId() == wxSTC_LEX_PHPSCRIPT) ||
       (ctrl.GetLexerId() == wxSTC_LEX_HTML)) {
        // XML lexer, highlight XML tags
        // Update the word chars
        clEditorXmlHelper xmlHelper(ctrl.GetCtrl());
        clEditorColouriseLocker colouriseLocker(ctrl.GetCtrl());

        // Highlight matching tags
        int startPos, endPos;
        wxString word = xmlHelper.GetXmlTagAt(ctrl.GetCurrentPosition(), startPos, endPos);
        if(word.IsEmpty()) {
            ctrl.SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
            ctrl.IndicatorClearRange(0, ctrl.GetLength());
            return;
        }

        static wxRegEx reOpenHtmlTag("<([a-z_\\-0-9]+)", wxRE_ADVANCED | wxRE_ICASE);
        static wxRegEx reCloseHtmlTag("</([a-z_\\-0-9]+)>", wxRE_ADVANCED | wxRE_ICASE);

        wxString searchWhat;
        wxString closeTag;

        if(reOpenHtmlTag.Matches(word)) {
            searchWhat = reOpenHtmlTag.GetMatch(word, 1);
            closeTag << "</" << searchWhat << ">";
            wxRegEx reOpenTag("<" + searchWhat + "[>]?", wxRE_ADVANCED | wxRE_ICASE);

            int pos = endPos;
            int depth = 0;
            int where = FindNext(searchWhat, pos, true);

            while(where != wxNOT_FOUND) {
                int startPos2, endPos2;
                word = xmlHelper.GetXmlTagAt(where, startPos2, endPos2);
                if((closeTag == word) && (depth == 0)) {
                    // We got the closing brace
                    ctrl.SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
                    ctrl.IndicatorClearRange(0, ctrl.GetLength());

                    // Set the new markers
                    ctrl.IndicatorFillRange(startPos, endPos - startPos);
                    ctrl.IndicatorFillRange(startPos2, endPos2 - startPos2);
                    return;

                } else if(closeTag == word) {
                    --depth;
                } else if(reOpenTag.Matches(word)) {
                    depth++;
                }
                where = FindNext(searchWhat, pos, true);
            }

        } else if(reCloseHtmlTag.Matches(word)) {
            searchWhat = reCloseHtmlTag.GetMatch(word, 1);
            closeTag << "</" << searchWhat << ">";
            
            wxString reString = "<" + searchWhat + "[>]?";
            wxRegEx reOpenTag(reString, wxRE_DEFAULT | wxRE_ICASE);
            if(!reOpenTag.IsValid()) {
                clDEBUG() << "Invalid regex:" << reString << clEndl;
            }
            
            int pos = startPos;
            int depth = 0;
            int where = FindPrev(searchWhat, pos, true);

            while(where != wxNOT_FOUND) {
                int startPos2, endPos2;
                word = xmlHelper.GetXmlTagAt(where, startPos2, endPos2);
                if(reOpenTag.Matches(word) && (depth == 0)) {
                    // We got the closing brace
                    ctrl.SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
                    ctrl.IndicatorClearRange(0, ctrl.GetLength());

                    // Set the new markers
                    ctrl.IndicatorFillRange(startPos, endPos - startPos);
                    ctrl.IndicatorFillRange(startPos2, endPos2 - startPos2);
                    return;

                } else if(closeTag == word) {
                    ++depth;
                } else if(reOpenTag.Matches(word)) {
                    --depth;
                }
                where = FindPrev(searchWhat, pos, true);
            }
        }

        // Clear the current selection
        ctrl.SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
        ctrl.IndicatorClearRange(0, ctrl.GetLength());
    }
}
