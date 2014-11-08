//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : commentpage.cpp
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
#include "editor_config.h"
#include "commentconfigdata.h"
#include "commentpage.h"

CommentPage::CommentPage(wxWindow* parent)
    : CommentPageBase(parent)
{
    Initialize();
}

void CommentPage::Save()
{
    CommentConfigData data;
    data.SetClassPattern(m_textCtrlClassPattern->GetValue());
    data.SetFunctionPattern(m_textCtrlFunctionPattern->GetValue());
    data.SetAddStarOnCComment(m_checkBoxContCComment->IsChecked());
    data.SetContinueCppComment(m_checkBoxContinueCppComment->IsChecked());
    data.SetUseShtroodel(m_checkBoxUseShtroodel->IsChecked());
    data.SetUseSlash2Stars(m_checkBoxUseSlash2Starts->IsChecked());

    EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &data);
}

void CommentPage::Initialize()
{
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    m_textCtrlClassPattern->SetValue(data.GetClassPattern());
    m_textCtrlFunctionPattern->SetValue(data.GetFunctionPattern());
    m_checkBoxContCComment->SetValue(data.GetAddStarOnCComment());
    m_checkBoxContinueCppComment->SetValue(data.GetContinueCppComment());
    m_checkBoxUseShtroodel->SetValue(data.GetUseShtroodel());
    m_checkBoxUseSlash2Starts->SetValue(data.GetUseSlash2Stars());

    DoSetPrefix();
}

void CommentPage::OnUseAtPrefix(wxCommandEvent& e) { DoSetPrefix(); }

void CommentPage::DoSetPrefix()
{
    bool useShtroodel = m_checkBoxUseShtroodel->IsChecked();

    wxString classPrefix = m_textCtrlClassPattern->GetValue();
    wxString funcPrefix = m_textCtrlFunctionPattern->GetValue();

    if(useShtroodel) {
        classPrefix.Replace(wxT("\\"), wxT("@"));
        funcPrefix.Replace(wxT("\\"), wxT("@"));
    } else {
        classPrefix.Replace(wxT("@"), wxT("\\"));
        funcPrefix.Replace(wxT("@"), wxT("\\"));
    }
    m_textCtrlClassPattern->SetValue(classPrefix);
    m_textCtrlFunctionPattern->SetValue(funcPrefix);
}
