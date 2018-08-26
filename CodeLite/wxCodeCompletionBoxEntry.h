//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : wxCodeCompletionBoxEntry.h
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

#ifndef WXCODECOMPLETIONBOXENTRY_H
#define WXCODECOMPLETIONBOXENTRY_H

#include <wx/clntdata.h>
#include <wx/sharedptr.h>
#include <vector>
#include "codelite_exports.h"
#include <wx/gdicmn.h>
#include <wx/string.h>
#include "entry.h" // TagEntryPtr
#include <wx/bitmap.h>

class wxStyledTextCtrl;
class WXDLLIMPEXP_CL wxCodeCompletionBoxEntry
{
    wxString m_text;
    wxString m_comment;
    int m_imgIndex;
    wxClientData* m_clientData;
    wxRect m_itemRect;
    friend class wxCodeCompletionBox;
    TagEntryPtr m_tag; // Internal
    int m_weight;
    wxBitmap m_alternateBitmap;

public:
    typedef wxSharedPtr<wxCodeCompletionBoxEntry> Ptr_t;
    typedef std::vector<wxCodeCompletionBoxEntry::Ptr_t> Vec_t;

public:
    wxCodeCompletionBoxEntry(const wxString& text, int imgId = wxNOT_FOUND, wxClientData* userData = NULL)
        : m_text(text)
        , m_imgIndex(imgId)
        , m_clientData(userData)
        , m_weight(0)
    {
    }

    wxCodeCompletionBoxEntry(const wxString& text, const wxBitmap& bmp, wxClientData* userData = NULL)
        : m_text(text)
        , m_imgIndex(wxNOT_FOUND)
        , m_clientData(userData)
        , m_weight(0)
        , m_alternateBitmap(bmp)
    {
    }

    wxCodeCompletionBoxEntry(const wxString& text, const wxBitmap& bmp, const wxString& helpText,
                             wxClientData* userData = NULL)
        : m_text(text)
        , m_comment(helpText)
        , m_imgIndex(wxNOT_FOUND)
        , m_clientData(userData)
        , m_weight(0)
        , m_alternateBitmap(bmp)
    {
    }

    virtual ~wxCodeCompletionBoxEntry()
    {
        wxDELETE(m_clientData);
        m_imgIndex = wxNOT_FOUND;
        m_text.Clear();
    }
    
    /**
     * @brief return the associated tag (might be null)
     */
    TagEntryPtr GetTag() const { return m_tag; }
    
    /**
     * @brief helper method for allocating wxCodeCompletionBoxEntry::Ptr
     */
    static wxCodeCompletionBoxEntry::Ptr_t New(const wxString& text, int imgId = wxNOT_FOUND,
                                               wxClientData* userData = NULL)
    {
        wxCodeCompletionBoxEntry::Ptr_t pEntry(new wxCodeCompletionBoxEntry(text, imgId, userData));
        return pEntry;
    }

    /**
     * @brief helper method for allocating wxCodeCompletionBoxEntry::Ptr
     */
    static wxCodeCompletionBoxEntry::Ptr_t New(const wxString& text, const wxBitmap& bmp, wxClientData* userData = NULL)
    {
        wxCodeCompletionBoxEntry::Ptr_t pEntry(new wxCodeCompletionBoxEntry(text, bmp, userData));
        return pEntry;
    }

    /**
     * @brief helper method for allocating wxCodeCompletionBoxEntry::Ptr
     */
    static wxCodeCompletionBoxEntry::Ptr_t New(const wxString& text, const wxString& helpText, const wxBitmap& bmp,
                                               wxClientData* userData = NULL)
    {
        wxCodeCompletionBoxEntry::Ptr_t pEntry(new wxCodeCompletionBoxEntry(text, bmp, helpText, userData));
        return pEntry;
    }

    void SetWeight(int weight) { this->m_weight = weight; }
    int GetWeight() const { return m_weight; }
    void SetImgIndex(int imgIndex) { this->m_imgIndex = imgIndex; }
    void SetText(const wxString& text) { this->m_text = text; }
    int GetImgIndex() const { return m_imgIndex; }
    const wxString& GetText() const { return m_text; }
    /**
     * @brief set client data, deleting the old client data
     * @param clientData
     */
    void SetClientData(wxClientData* clientData)
    {
        wxDELETE(this->m_clientData);
        this->m_clientData = clientData;
    }
    wxClientData* GetClientData() { return m_clientData; }
    void SetComment(const wxString& comment) { this->m_comment = comment; }
    const wxString& GetComment() const { return m_comment; }
    void SetAlternateBitmap(const wxBitmap& alternateBitmap) { this->m_alternateBitmap = alternateBitmap; }
    const wxBitmap& GetAlternateBitmap() const { return m_alternateBitmap; }
};

#endif // WXCODECOMPLETIONBOXENTRY_H
