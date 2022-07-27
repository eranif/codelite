//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingscomments.cpp
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

#include "editorsettingscomments.h"

#include "commentconfigdata.h"

EditorSettingsComments::EditorSettingsComments(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &m_data);

    AddHeader(_("Comments"));
    AddProperty(_("Hitting ENTER in C comment, adds `*`"), m_data.GetAddStarOnCComment(),
                [this](const wxString& label, const wxAny& value) mutable {
                    bool value_bool;
                    if(value.GetAs(&value_bool)) {
                        m_data.SetAddStarOnCComment(value_bool);
                        EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &m_data);
                    }
                });

    AddProperty(_("Hitting ENTER in C++ comment, adds `//`"), m_data.GetContinueCppComment(),
                [this](const wxString& label, const wxAny& value) mutable {
                    bool value_bool;
                    if(value.GetAs(&value_bool)) {
                        m_data.SetContinueCppComment(value_bool);
                        EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &m_data);
                    }
                });

    AddHeader(_("Code navigation shortcut is: Mouse left button and:"));
    bool use_alt = m_options->HasOption(OptionsConfig::Opt_NavKey_Alt);
    bool use_ctrl = m_options->HasOption(OptionsConfig::Opt_NavKey_Control);
    AddProperty(_("Control Key"), use_ctrl, UPDATE_OPTION_CB(Opt_NavKey_Control));
    AddProperty(_("Alt Key"), use_alt, UPDATE_OPTION_CB(Opt_NavKey_Alt));

    AddHeader(_("Generated doc comments"));
    AddProperty(_("Class template"), m_data.GetClassPattern(),
                [this](const wxString& label, const wxAny& value) mutable {
                    wxString value_str;
                    if(value.GetAs(&value_str)) {
                        m_data.SetClassPattern(value_str);
                        EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &m_data);
                    }
                });

    AddProperty(_("Function template"), m_data.GetFunctionPattern(),
                [this](const wxString& label, const wxAny& value) mutable {
                    wxString value_str;
                    if(value.GetAs(&value_str)) {
                        m_data.SetFunctionPattern(value_str);
                        EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &m_data);
                    }
                });

    AddProperty(_("Auto generate on ENTER"), m_data.IsAutoInsert(),
                [this](const wxString& label, const wxAny& value) mutable {
                    bool value_bool;
                    if(value.GetAs(&value_bool)) {
                        m_data.SetAutoInsert(value_bool);
                        EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &m_data);
                    }
                });

    AddProperty(_("Use Qt style docs"), m_data.IsUseQtStyle(),
                [this](const wxString& label, const wxAny& value) mutable {
                    bool value_bool;
                    if(value.GetAs(&value_bool)) {
                        m_data.SetUseQtStyle(value_bool);
                        EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &m_data);
                    }
                });
}
