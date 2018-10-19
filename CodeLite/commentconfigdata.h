//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : commentconfigdata.h
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
#ifndef __commentconfigdata__
#define __commentconfigdata__

#include "codelite_exports.h"
#include "serialized_object.h"

class WXDLLIMPEXP_CL CommentConfigData : public SerializedObject
{
    bool m_addStarOnCComment = true;
    bool m_continueCppComment = false;
    bool m_autoInsert = true;
    bool m_useQtStyle = false;
    wxString m_classPattern;
    wxString m_functionPattern;

public:
    CommentConfigData();
    virtual ~CommentConfigData();

public:
    virtual void DeSerialize(Archive& arch);
    virtual void Serialize(Archive& arch);

    // Setters
    void SetAddStarOnCComment(const bool& addStarOnCComment) { this->m_addStarOnCComment = addStarOnCComment; }
    void SetClassPattern(const wxString& classPattern) { this->m_classPattern = classPattern; }
    void SetContinueCppComment(const bool& continueCppComment) { this->m_continueCppComment = continueCppComment; }
    void SetFunctionPattern(const wxString& functionPattern) { this->m_functionPattern = functionPattern; }

    const bool& GetAddStarOnCComment() const { return m_addStarOnCComment; }
    const wxString& GetClassPattern() const { return m_classPattern; }
    const bool& GetContinueCppComment() const { return m_continueCppComment; }
    const wxString& GetFunctionPattern() const { return m_functionPattern; }
    void SetUseQtStyle(bool useQtStyle) { this->m_useQtStyle = useQtStyle; }
    bool IsUseQtStyle() const { return m_useQtStyle; }
    void SetAutoInsert(bool b) { this->m_autoInsert = b; }
    bool IsAutoInsert() const { return m_autoInsert; }
    wxString GetCommentBlockPrefix() const { return IsUseQtStyle() ? wxString("/*!") : wxString("/**"); }
};
#endif // __commentconfigdata__
