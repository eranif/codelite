//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilersFoundDlg.cpp
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

#include "CompilersFoundDlg.h"
#include "ICompilerLocator.h"
#include "globals.h"
#include <GCCMetadata.hpp>
#include <compiler.h>
#include <windowattrmanager.h>

CompilersFoundDlg::CompilersFoundDlg(wxWindow* parent, const ICompilerLocator::CompilerVec_t& compilers)
    : CompilersFoundDlgBase(parent)
{
    m_allCompilers = compilers;

    for(size_t i = 0; i < m_allCompilers.size(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back(::MakeCheckboxVariant(m_allCompilers[i]->GetName(), true, wxNOT_FOUND));
        cols.push_back(m_allCompilers[i]->GetInstallationPath());
        m_dataview->AppendItem(cols, (wxUIntPtr)i);
    }
    ::clSetDialogBestSizeAndPosition(this);
}

CompilersFoundDlg::~CompilersFoundDlg() {}

size_t CompilersFoundDlg::GetSelectedCompilers(ICompilerLocator::CompilerVec_t& compilers)
{
    for(size_t i = 0; i < m_dataview->GetItemCount(); ++i) {
        auto item = m_dataview->RowToItem(i);
        if(m_dataview->IsItemChecked(item)) {
            size_t index = m_dataview->GetItemData(item);
            compilers.push_back(m_allCompilers[index]);
        }
    }
    return compilers.size();
}
