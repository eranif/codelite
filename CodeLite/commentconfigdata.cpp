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
{
    m_classPattern << " * @class $(Name)\n";
    m_classPattern << " * @author $(User)\n";
    m_classPattern << " * @date $(Date)\n";
    m_classPattern << " * @file $(CurrentFileName).$(CurrentFileExt)\n";
    m_classPattern << " * @brief \n";
    m_functionPattern << " * @brief \n";
}

CommentConfigData::~CommentConfigData() {}

void CommentConfigData::DeSerialize(Archive& arch)
{
    arch.Read("m_addStarOnCComment", m_addStarOnCComment);
    arch.Read("m_continueCppComment", m_continueCppComment);
    arch.Read("m_autoInsert", m_autoInsert);
    arch.Read("m_useQtStyle", m_useQtStyle);
    m_classPattern.Replace("|", "\n");
    arch.Read("m_functionPattern", m_functionPattern);
    m_functionPattern.Replace("|", "\n");
    arch.Read("m_functionPattern", m_functionPattern);
}

void CommentConfigData::Serialize(Archive& arch)
{
    arch.Write("m_addStarOnCComment", m_addStarOnCComment);
    arch.Write("m_continueCppComment", m_continueCppComment);
    arch.Write("m_autoInsert", m_autoInsert);
    arch.Write("m_useQtStyle", m_useQtStyle);
    m_classPattern.Replace("\n", "|");
    arch.Write("m_classPattern", m_classPattern);
    m_functionPattern.Replace("\n", "|");
    arch.Write("m_functionPattern", m_functionPattern);
}
