//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : tags_parser_search_path_dlg.cpp
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

#include "tags_parser_search_path_dlg.h"
#include "windowattrmanager.h"

TagsParserSearchPathsDlg::TagsParserSearchPathsDlg(wxWindow* parent,
                                                   const wxArrayString& paths,
                                                   const wxArrayString& excludePaths)
    : TagsParserSearchPathsBaseDlg(parent)
{
    m_checkListPaths->Append(paths);
    for(unsigned int i = 0; i < m_checkListPaths->GetCount(); i++) {
        m_checkListPaths->Check(i, true);
    }

    m_checkListExcludePaths->Append(excludePaths);
    for(unsigned int i = 0; i < m_checkListExcludePaths->GetCount(); i++) {
        m_checkListExcludePaths->Check(i, true);
    }
    
    SetName("TagsParserSearchPathsDlg");
    WindowAttrManager::Load(this);
}

wxArrayString TagsParserSearchPathsDlg::GetSearchPaths() const
{
    wxArrayString paths;
    for(unsigned int i = 0; i < m_checkListPaths->GetCount(); i++) {
        if(m_checkListPaths->IsChecked(i)) {
            paths.Add(m_checkListPaths->GetString(i));
        }
    }
    return paths;
}

wxArrayString TagsParserSearchPathsDlg::GetExcludePath() const
{
    wxArrayString paths;
    for(unsigned int i = 0; i < m_checkListExcludePaths->GetCount(); i++) {
        if(m_checkListExcludePaths->IsChecked(i)) {
            paths.Add(m_checkListExcludePaths->GetString(i));
        }
    }
    return paths;
}
