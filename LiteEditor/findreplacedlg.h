//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findreplacedlg.h
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
#ifndef FIND_REPLACE_DLG_H
#define FIND_REPLACE_DLG_H

#include "clFilesCollector.h"
#include "cl_config.h"

class FindReplaceData : public clConfigItem
{
    wxArrayString m_replaceString;
    wxArrayString m_findString;
    wxArrayString m_findWhere;
    size_t m_flags;
    wxString m_encoding;
    wxArrayString m_fileMask;
    wxString m_selectedMask;
    size_t m_file_scanner_flags = clFilesScanner::SF_DEFAULT;

protected:
    void TruncateArray(wxArrayString& arr, size_t maxSize);

public:
    /**
     * @brief
     * @param json
     */
    virtual void FromJSON(const JSONItem& json);
    /**
     * @brief
     * @return
     */
    virtual JSONItem ToJSON() const;
    FindReplaceData();
    virtual ~FindReplaceData() {}

    void SetFileScannerFlags(size_t f) { m_file_scanner_flags = f; }
    size_t GetFileScannerFlags() const { return m_file_scanner_flags; }

    wxString GetFindString() const;
    wxString GetReplaceString() const;
    void SetFindString(const wxString& str);
    void SetReplaceString(const wxString& str);

    wxArrayString GetFindStringArr() const;

    wxArrayString GetReplaceStringArr() const;

    void SetEncoding(const wxString& encoding) { this->m_encoding = encoding; }
    void SetFileMask(const wxArrayString& fileMask) { this->m_fileMask = fileMask; }
    void SetFlags(size_t flags) { this->m_flags = flags; }
    void SetWhereOptions(const wxArrayString& where);
    wxString GetWhere() const;
    void SetSelectedMask(const wxString& selectedMask) { this->m_selectedMask = selectedMask; }
    const wxString& GetEncoding() const { return m_encoding; }
    const wxArrayString& GetFileMask() const { return m_fileMask; }
    size_t GetFlags() const { return m_flags; }
    const wxArrayString& GetWhereOptions() const { return m_findWhere; }
    const wxString& GetSelectedMask() const { return m_selectedMask; }
    void SetFindStrings(const wxArrayString& findStrings) { this->m_findString = findStrings; }
    void SetReplaceStrings(const wxArrayString& replaceStrings) { this->m_replaceString = replaceStrings; }
};

#endif // FIND_REPLACE_DLG_H
