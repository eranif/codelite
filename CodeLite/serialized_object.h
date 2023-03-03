//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : serialized_object.h
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
#ifndef SERIALIZED_OBJECT_H
#define SERIALIZED_OBJECT_H

#include "archive.h"

/**
 * \class SerializedObject
 * \brief an interface to the serialized object, every class who whishes to be
 * serialized and to be read/write from an xml file, must implement this interface
 * \author Eran
 * \date 07/20/07
 */
class SerializedObject
{
    wxString m_version;

public:
    SerializedObject() {}
    virtual ~SerializedObject() {}
    virtual void Serialize(Archive& arch) = 0;
    virtual void DeSerialize(Archive& arch) = 0;

    virtual void SetVersion(const wxString& version) { this->m_version = version; }
    virtual const wxString& GetVersion() const { return m_version; }
};

class WXDLLIMPEXP_CL TabInfo : public SerializedObject
{
    wxString m_fileName;
    int m_firstVisibleLine;
    int m_currentLine;
    wxArrayString m_bookmarks;
    std::vector<int> m_folds;

public:
    // setters
    void SetFileName(const wxString& _fileName) { this->m_fileName = _fileName; }
    void SetFirstVisibleLine(const int& _firstVisibleLine) { this->m_firstVisibleLine = _firstVisibleLine; }
    void SetCurrentLine(const int& _currentLine) { this->m_currentLine = _currentLine; }
    void SetBookmarks(const wxArrayString& _bookmarks) { this->m_bookmarks = _bookmarks; }
    void SetCollapsedFolds(const std::vector<int>& folds) { m_folds = folds; }
    // getters
    const wxString& GetFileName() const { return this->m_fileName; }
    const int& GetFirstVisibleLine() const { return this->m_firstVisibleLine; }
    const int& GetCurrentLine() const { return this->m_currentLine; }
    const wxArrayString& GetBookmarks() const { return this->m_bookmarks; }
    const std::vector<int>& GetCollapsedFolds() const { return m_folds; }

    TabInfo();
    virtual ~TabInfo();

    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);
};

#endif // SERIALIZED_OBJECT_H
