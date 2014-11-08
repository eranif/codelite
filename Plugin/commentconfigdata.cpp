//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : commentconfigdata.cpp
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
#include "commentconfigdata.h"

CommentConfigData::CommentConfigData()
    : m_addStarOnCComment(true)
    , m_continueCppComment(false)
    , m_useSlash2Stars(true)
    , m_useShtroodel(true)
    , m_autoInsertAfterSlash2Stars(true)
{
    m_classPattern << wxT(" * @class $(Name)\n");
    m_classPattern << wxT(" * @author $(User)\n");
    m_classPattern << wxT(" * @date $(Date)\n");
    m_classPattern << wxT(" * @file $(CurrentFileName).$(CurrentFileExt)\n");
    m_classPattern << wxT(" * @brief \n");

    m_functionPattern << wxT(" * @brief \n");
}

CommentConfigData::~CommentConfigData() {}

void CommentConfigData::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_addStarOnCComment"), m_addStarOnCComment);
    arch.Read(wxT("m_continueCppComment"), m_continueCppComment);
    arch.Read(wxT("m_useSlash2Stars"), m_useSlash2Stars);
    arch.Read(wxT("m_useShtroodel"), m_useShtroodel);
    arch.Read(wxT("m_classPattern"), m_classPattern);
    m_classPattern.Replace(wxT("|"), wxT("\n"));

    arch.Read(wxT("m_functionPattern"), m_functionPattern);
    m_functionPattern.Replace(wxT("|"), wxT("\n"));
    arch.Read("m_autoInsertAfterSlash2Stars", m_autoInsertAfterSlash2Stars);
}

void CommentConfigData::Serialize(Archive& arch)
{
    arch.Write(wxT("m_addStarOnCComment"), m_addStarOnCComment);
    arch.Write(wxT("m_continueCppComment"), m_continueCppComment);
    arch.Write(wxT("m_useSlash2Stars"), m_useSlash2Stars);
    arch.Write(wxT("m_useShtroodel"), m_useShtroodel);
    m_classPattern.Replace(wxT("\n"), wxT("|"));
    arch.Write(wxT("m_classPattern"), m_classPattern);
    m_functionPattern.Replace(wxT("\n"), wxT("|"));
    arch.Write(wxT("m_functionPattern"), m_functionPattern);
    arch.Write("m_autoInsertAfterSlash2Stars", m_autoInsertAfterSlash2Stars);
}
