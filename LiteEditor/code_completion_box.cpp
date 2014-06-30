//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : code_completion_box.cpp
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

#include "code_completion_box.h"
#include "cc_box.h"
#include "ctags_manager.h"
#include "cc_box_tip_window.h"
#include "code_completion_manager.h"

CodeCompletionBox::CodeCompletionBox()
    : m_ccBox(NULL)
    , m_tip(NULL)
{
}

CodeCompletionBox::~CodeCompletionBox()
{
}

CodeCompletionBox& CodeCompletionBox::Get()
{
    static CodeCompletionBox mgr;
    return mgr;
}

void CodeCompletionBox::Display(LEditor* editor, const TagEntryPtrVector_t& tags, const wxString& word, bool isKeywordList, wxEvtHandler* owner)
{
    if ( !m_ccBox ) {
        DoCreateBox(editor);
    }

    if ( !CodeCompletionManager::Get().GetWordCompletionRefreshNeeded() ) {
        if ( m_ccBox ) {
            Hide();
        }
        DoCreateBox(editor);
    } else {

        // turn off the flag
        if ( !m_ccBox ) {
            CodeCompletionManager::Get().SetWordCompletionRefreshNeeded(false);
            return;
        }
    }

    m_ccBox->Show(tags, word, isKeywordList, owner);
    // Show takes into account the return value of 'GetWordCompletionRefreshNeeded'
    // this is why we reset the flag *after* the call to Show(..)
    CodeCompletionManager::Get().SetWordCompletionRefreshNeeded(false);
    this->CallAfter(&CodeCompletionBox::FocusEditor, editor);
}

void CodeCompletionBox::Hide()
{
    if ( m_ccBox ) {
        // Cancel the calltip as well
        CancelTip();
        m_ccBox->Destroy();
    }
    m_ccBox = NULL;
}

bool CodeCompletionBox::IsShown() const
{
    return m_ccBox && m_ccBox->IsShown();
}

bool CodeCompletionBox::SelectWord(const wxString& word)
{
    if ( IsShown() ) {
        return m_ccBox->SelectWord(word);
    } else {
        return false;
    }
}

void CodeCompletionBox::InsertSelection()
{
    if ( IsShown() ) {
        m_ccBox->InsertSelection();
    }
}

void CodeCompletionBox::Next()
{
    if ( IsShown() ) {
        m_ccBox->Next();
    }
}

void CodeCompletionBox::NextPage()
{
    if ( IsShown() ) {
        m_ccBox->NextPage();
    }
}

void CodeCompletionBox::Previous()
{
    if ( IsShown() ) {
        m_ccBox->Previous();
    }
}

void CodeCompletionBox::PreviousPage()
{
    if ( IsShown() ) {
        m_ccBox->PreviousPage();
    }
}

void CodeCompletionBox::CancelTip()
{
    if ( m_tip ) {
        m_tip->Destroy();
        m_tip = NULL;
    }
}

void CodeCompletionBox::ShowTip(const wxString& msg, LEditor* editor)
{
    CancelTip();

    if ( !editor )
        return;

    wxPoint pt = editor->PointFromPosition( editor->GetCurrentPos() );
    wxPoint displayPt = editor->ClientToScreen(pt);

    // Dont display the tip if it displays outside of the
    // editor client area
    wxRect editorRect = editor->GetScreenRect();
    if ( editorRect.Contains( displayPt ) == false ) {
        return;
    }

    m_tip = new CCBoxTipWindow(::wxGetTopLevelParent(editor), msg, 1, true);
    m_tip->PositionAt(displayPt);
}

void CodeCompletionBox::ShowTip(const wxString& msg, const wxPoint& pt, LEditor* editor)
{
    CancelTip();
    if ( !editor )
        return;

    wxPoint p = pt;
    p.y += 16; // Place it under the cursor

    // Dont display the tip if it displays outside of the
    // editor client area
    wxRect editorRect = editor->GetScreenRect();
    if ( editorRect.Contains( p ) == false ) {
        return;
    }

    m_tip = new CCBoxTipWindow(::wxGetTopLevelParent(editor), msg, 1, true);
    m_tip->PositionAt(p);
}

void CodeCompletionBox::RegisterImage(const wxString& kind, const wxBitmap& bmp)
{
    // sanity
    if ( bmp.IsOk() == false || kind.IsEmpty() )
        return;

    BitmapMap_t::iterator iter = m_bitmaps.find(kind);
    if ( iter != m_bitmaps.end() ) {
        m_bitmaps.erase(iter);
    }
    m_bitmaps.insert(std::make_pair(kind, bmp));
}

void CodeCompletionBox::DoCreateBox(LEditor* editor)
{
    m_ccBox = new CCBox(editor, false, TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_AUTO_INSERT_SINGLE_CHOICE);
    BitmapMap_t::const_iterator iter = m_bitmaps.begin();
    for(; iter != m_bitmaps.end(); ++iter ) {
        m_ccBox->RegisterImageForKind(iter->first, iter->second);
    }
}

void CodeCompletionBox::FocusEditor(LEditor* editor)
{
    editor->SetActive();
}
