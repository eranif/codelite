//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : wxCodeCompletionBoxManager.h
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

#ifndef WXCODECOMPLETIONBOXMANAGER_H
#define WXCODECOMPLETIONBOXMANAGER_H

#include "codelite_exports.h"
#include "wxCodeCompletionBox.h"

#include <wx/event.h>

class wxStyledTextCtrl;
class WXDLLIMPEXP_SDK wxCodeCompletionBoxManager : public wxEvtHandler
{
    friend class wxCodeCompletionBox;

protected:
    wxCodeCompletionBox* m_box;
    wxStyledTextCtrl* m_stc;

private:
    wxCodeCompletionBoxManager();
    ~wxCodeCompletionBoxManager();

protected:
    void DestroyCurrent();
    void InsertSelection(wxCodeCompletionBoxEntry::Ptr_t match, bool userTriggered);
    void InsertSelectionTemplateFunction(const wxString& selection);
    void ShowAddIncludeDialog(const wxString& include);

    // Handle the current editor events
    void OnStcModified(wxStyledTextEvent& event);
    void OnStcCharAdded(wxStyledTextEvent& event);
    void OnStcKeyDown(wxKeyEvent& event);
    void OnStcLeftDown(wxMouseEvent& event);
    void OnDismissBox(wxCommandEvent& event);
    void OnAppActivate(wxActivateEvent& event);

    void DoShowCCBoxTags(const TagEntryPtrVector_t& tags);
    void DoShowCCBoxLSPItems(const LSP::CompletionItem::Vec_t& items);
    void DoShowCCBoxEntries(const wxCodeCompletionBoxEntry::Vec_t& entries);
    void DoConnectStcEventHandlers(wxStyledTextCtrl* ctrl);

public:
    static wxCodeCompletionBoxManager& Get();

    /**
     * @brief uninitialize all event handlers and destroy the CC box manager
     */
    static void Free();

    /**
     * @brief destroy the current CC box
     */
    void DestroyCCBox();

    /**
     * @brief show the completion box
     * @param ctrl the wxSTC control requesting the completion box
     * @param entries list of entries to display
     * @param flags code completion box options. See wxCodeCompletionBox::eOptions
     * for possible values
     * @param eventObject all events fired by the cc box will have eventObject
     * as the event object (wxEvent::GetEventObject())
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries, size_t flags,
                           int startPos, wxEvtHandler* eventObject = NULL);
    /**
     * @brief show the completion box
     * @param ctrl the wxSTC control requesting the completion box
     * @param entries list of entries to display
     * @param bitmaps alternative bitmap list to use
     * @param flags code completion box options. See wxCodeCompletionBox::eOptions
     * for possible values
     * @param eventObject all events fired by the cc box will have eventObject
     * as the event object (wxEvent::GetEventObject())
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries,
                           const wxCodeCompletionBox::BmpVec_t& bitmaps, size_t flags, int startPos,
                           wxEvtHandler* eventObject = NULL);

    /**
     * @brief show the completion box
     * @param ctrl the wxSTC control requesting the completion box
     * @param tags list of entries to display
     * @param flags code completion box options. See wxCodeCompletionBox::eOptions
     * for possible values
     * @param eventObject all events fired by the cc box will have eventObject
     * as the event object (wxEvent::GetEventObject())
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrVector_t& tags, size_t flags, int startPos,
                           wxEvtHandler* eventObject = NULL);

    /**
     * @brief show the completion box. Language Server Protocol version
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const LSP::CompletionItem::Vec_t& completions, size_t flags = 0,
                           int startPos = wxNOT_FOUND, wxEvtHandler* eventObject = NULL);

    /**
     * @brief do we have a completion box shown?
     */
    bool IsShown() const { return m_box != NULL; }

    /**
     * @brief return the code completion box
     */
    wxCodeCompletionBox* GetCCWindow() { return m_box; }
};

#endif // WXCODECOMPLETIONBOXMANAGER_H
