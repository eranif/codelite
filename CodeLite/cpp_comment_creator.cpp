//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cpp_comment_creator.cpp
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
#include "cpp_comment_creator.h"
#include "language.h"
#include "precompiled_header.h"
#include <vector>
#include <wx/datetime.h>

#define trimMe(str)            \
    {                          \
        str = str.Trim();      \
        str = str.Trim(false); \
    }

CppCommentCreator::CppCommentCreator(TagEntryPtr tag, wxChar keyPrefix)
    : CommentCreator(keyPrefix)
    , m_tag(tag)
{
}

CppCommentCreator::~CppCommentCreator() {}

wxString CppCommentCreator::CreateComment()
{
    if(m_tag->GetKind() == wxT("class") || m_tag->GetKind() == wxT("struct")) {
        return wxT("$(ClassPattern)\n");
    } else if(m_tag->IsMethod()) {
        return FunctionComment();
    } else {
        return wxEmptyString;
    }
}

wxString CppCommentCreator::FunctionComment()
{
    wxString comment;

    // parse the function signature
    std::vector<TagEntryPtr> tags;
    Language* lang = LanguageST::Get();
    lang->GetLocalVariables(m_tag->GetSignature(), tags, true);

    comment << wxT("$(FunctionPattern)\n");
    for(size_t i = 0; i < tags.size(); i++)
        comment << wxT(" * ") << m_keyPrefix << wxT("param ") << tags.at(i)->GetName() << wxT("\n");

    if(m_tag->GetKind() == wxT("function")) {
        clFunction f;
        if(lang->FunctionFromPattern(m_tag, f)) {
            wxString type = _U(f.m_returnValue.m_type.c_str());
            trimMe(type);
            if(type != wxT("void")) { // void has no return value
                comment << wxT(" * ") << m_keyPrefix << wxT("return \n");
            }
        }

    } else {
        Variable var;
        lang->VariableFromPattern(m_tag->GetPattern(), m_tag->GetName(), var);
        wxString type = _U(var.m_type.c_str());
        wxString name = _U(var.m_name.c_str());
        trimMe(type);
        trimMe(name);

        if(type != wxT("void") // void has no return value
           && name != type) {  // and not a constructor
            comment << wxT(" * ") << m_keyPrefix << wxT("return \n");
        }
    }
    return comment;
}
