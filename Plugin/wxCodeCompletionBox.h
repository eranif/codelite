//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : wxCodeCompletionBox.h
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

#ifndef WXCODECOMPLETIONBOX_H
#define WXCODECOMPLETIONBOX_H

#include "LSP/CompletionItem.h"
#include "entry.h"
#include "wxCodeCompletionBoxBase.h"
#include "wxCodeCompletionBoxEntry.hpp"
#include <list>
#include <vector>
#include <wx/arrstr.h>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/font.h>
#include <wx/sharedptr.h>
#include <wx/stc/stc.h>
#include <wxStringHash.h>

class CCBoxTipWindow;
class wxCodeCompletionBox;
class wxCodeCompletionBoxManager;

class WXDLLIMPEXP_SDK wxCodeCompletionBox : public wxCodeCompletionBoxBase
{
public:
    typedef std::vector<wxBitmap> BmpVec_t;
    friend class wxCodeCompletionBoxManager;

public:
    enum eOptions {
        kNone = 0,
        kInsertSingleMatch = (1 << 0),
        kRefreshOnKeyType = (1 << 1),
        kNoShowingEvent = (1 << 2), // Dont send the wxEVT_CCBOX_SHOWING event
    };

protected:
    virtual void OnSelectionActivated(wxDataViewEvent& event);
    virtual void OnSelectionChanged(wxDataViewEvent& event);
    wxCodeCompletionBoxEntry::Vec_t m_allEntries;
    wxCodeCompletionBoxEntry::Vec_t m_entries;
    wxCodeCompletionBox::BmpVec_t m_bitmaps;
    static wxCodeCompletionBox::BmpVec_t m_defaultBitmaps;
    std::unordered_map<int, int> m_lspCompletionItemImageIndexMap;
    wxString m_displayedTip;
    wxStyledTextCtrl* m_stc;
    wxFont m_ccFont;
    int m_startPos;

    /// When firing the various "clCodeCompletionEvent"s, set the event object
    /// to this member. This help distinguish the real trigger of the code completion
    /// box
    wxEvtHandler* m_eventObject;

    /// Code completion tooltip that is shown next to the code completion box
    CCBoxTipWindow* m_tipWindow;

    /// The code completion box flags, see above enum for possible values
    size_t m_flags;

    /// Scrollbar bitmaps
    wxBitmap m_bmpUp;
    wxBitmap m_bmpDown;
    wxBitmap m_bmpUpEnabled;
    wxBitmap m_bmpDownEnabled;

protected:
    void StcModified(wxStyledTextEvent& event);
    void StcCharAdded(wxStyledTextEvent& event);
    void StcLeftDown(wxMouseEvent& event);
    void StcKeyDown(wxKeyEvent& event);
    static void InitializeDefaultBitmaps();
    void DoPopulateList();

public:
    /**
     * @brief return the bitamp associated with this tag entry
     */
    static wxBitmap GetBitmap(TagEntryPtr tag);

    virtual ~wxCodeCompletionBox();
    /**
     * @brief construct a code completion box
     * @param parent the parent window
     * @param eventObject will be passed as the eventObject of the various clCodeCompletionEvent
     * events fired from this object
     */
    wxCodeCompletionBox(wxWindow* parent, wxEvtHandler* eventObject = NULL, size_t flags = 0);

    /**
     * @brief show the completion box
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries);

    /**
     * @brief show the completion box
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrVector_t& tags);

    /**
     * @brief show the completion box  (Language Server Protocol support)
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const LSP::CompletionItem::Vec_t& completions);

    void SetBitmaps(const wxCodeCompletionBox::BmpVec_t& bitmaps) { this->m_bitmaps = bitmaps; }
    const wxCodeCompletionBox::BmpVec_t& GetBitmaps() const { return m_bitmaps; }

    /**
     * @brief convert TagEntryPtr into Code Completion entry
     */
    static wxCodeCompletionBoxEntry::Ptr_t TagToEntry(TagEntryPtr tag);

    /// accessors
    void SetFlags(size_t flags) { this->m_flags = flags; }
    size_t GetFlags() const { return m_flags; }

    void SetStartPos(int startPos) { this->m_startPos = startPos; }
    int GetStartPos() const { return m_startPos; }

protected:
    /**
     * @brief filter the results based on what the user typed in the editor
     * @return Should we refresh the content of the CC box (based on number of "Exact matches" / "Starts with" found)
     */
    bool FilterResults();
    void RemoveDuplicateEntries();
    void InsertSelection();
    wxString GetFilter();

    // For backward compatibility, we support initializing the list with TagEntryPtrVector_t
    // These 2 functions provide conversion between wxCodeCompletionBoxEntry and TagEntryPtr
    wxCodeCompletionBoxEntry::Vec_t TagsToEntries(const TagEntryPtrVector_t& tags);
    wxCodeCompletionBoxEntry::Vec_t LSPCompletionsToEntries(const LSP::CompletionItem::Vec_t& completions);
    static int GetImageId(TagEntryPtr entry);
    int GetImageId(LSP::CompletionItem::Ptr_t entry) const;
    void DoDisplayTipWindow();
    void DoDestroyTipWindow();

    void DoUpdateList();
    void DoDestroy();
    void DoShowCompletionBox();
};
#endif // WXCODECOMPLETIONBOX_H
