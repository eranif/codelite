//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : code_completion_box.h
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

#ifndef CODECOMPLETIONBOX_H
#define CODECOMPLETIONBOX_H

#include "cl_editor.h"
#include <wx/event.h>
#include <wx/bitmap.h>

class CCBoxTipWindow;
class CCBox;
class CodeCompletionBox : public wxEvtHandler
{
    typedef std::map<wxString, wxBitmap> BitmapMap_t;

    CCBox* m_ccBox;
    CCBoxTipWindow* m_tip;
    BitmapMap_t m_bitmaps;

private:
    CodeCompletionBox();

protected:
    void DoCreateBox(LEditor* editor);
    void FocusEditor(LEditor* editor);

public:
    virtual ~CodeCompletionBox();
    static CodeCompletionBox& Get();

    void Display(LEditor* editor,
                 const TagEntryPtrVector_t& tags,
                 const wxString& word,
                 bool autoRefreshList,
                 wxEvtHandler* owner = NULL);
    void Hide();
    bool IsShown() const;
    bool SelectWord(const wxString& word);
    void InsertSelection();
    void Previous();
    void Next();
    void PreviousPage();
    void NextPage();
    /**
     * @brief display a tooltip at the caret position of the editor
     */
    void ShowTip(const wxString& msg, LEditor* editor);

    /**
     * @brief display a tooltip at a given point
     * 'pt' is in screen coordinates
     */
    void ShowTip(const wxString& msg, const wxPoint& pt, LEditor* editor);

    /**
     * @brief dismiss the last calltip shown
     */
    void CancelTip();

    /**
     * @brief add an additional image to the code completion box for a given
     * "kind"
     */
    void RegisterImage(const wxString& kind, const wxBitmap& bmp);
};

#endif // CODECOMPLETIONBOX_H
