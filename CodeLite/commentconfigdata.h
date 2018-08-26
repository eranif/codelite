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

#include "serialized_object.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL CommentConfigData : public SerializedObject
{
    bool m_addStarOnCComment;
    bool m_continueCppComment;
    bool m_useSlash2Stars;
    bool m_useShtroodel;
    wxString m_classPattern;
    wxString m_functionPattern;
    bool m_autoInsertAfterSlash2Stars;

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
    void SetUseShtroodel(const bool& useShtroodel) { this->m_useShtroodel = useShtroodel; }
    void SetUseSlash2Stars(const bool& useSlash2Stars) { this->m_useSlash2Stars = useSlash2Stars; }
    // Getters
    const bool& GetAddStarOnCComment() const { return m_addStarOnCComment; }
    const wxString& GetClassPattern() const { return m_classPattern; }
    const bool& GetContinueCppComment() const { return m_continueCppComment; }
    const wxString& GetFunctionPattern() const { return m_functionPattern; }
    bool GetUseShtroodel() const { return true; }
    bool GetUseSlash2Stars() const { return m_useSlash2Stars; }
    void SetAutoInsertAfterSlash2Stars(bool autoInsertAfterSlash2Stars)
    {
        this->m_autoInsertAfterSlash2Stars = autoInsertAfterSlash2Stars;
    }
    bool IsAutoInsertAfterSlash2Stars() const { return m_autoInsertAfterSlash2Stars; }

    wxString GetCommentBlockPrefix() const { return GetUseSlash2Stars() ? wxString("/**") : wxString("/*!"); }
};
#endif // __commentconfigdata__
