//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : treebooknodebase.h
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

#ifndef __TREEBOOKNODEBASE__
#define __TREEBOOKNODEBASE__

#include "clPropertiesPage.hpp"
#include "cl_config.h"
#include "editor_config.h"

#define UPDATE_BOOL_CB(SetterMethod)                    \
    [this](const wxString& label, const wxAny& value) { \
        wxUnusedVar(label);                             \
        bool value_bool = false;                        \
        if(value.GetAs(&value_bool)) {                  \
            m_options->SetterMethod(value_bool);        \
        }                                               \
    }

#define UPDATE_CLCONFIG_BOOL_CB(PropertyName)                \
    [](const wxString& label, const wxAny& value) {          \
        wxUnusedVar(label);                                  \
        bool value_bool = false;                             \
        if(value.GetAs(&value_bool)) {                       \
            clConfig::Get().Write(PropertyName, value_bool); \
        }                                                    \
    }

#define UPDATE_CLCONFIG_INT_CB(PropertyName)                 \
    [](const wxString& label, const wxAny& value) {          \
        wxUnusedVar(label);                                  \
        int value_long = false;                              \
        if(value.GetAs(&value_long)) {                       \
            clConfig::Get().Write(PropertyName, value_long); \
        }                                                    \
    }

#define UPDATE_CLCONFIG_TEXT_CB(PropertyName)               \
    [](const wxString& label, const wxAny& value) {         \
        wxUnusedVar(label);                                 \
        wxString value_str;                                 \
        if(value.GetAs(&value_str)) {                       \
            clConfig::Get().Write(PropertyName, value_str); \
        }                                                   \
    }

#define UPDATE_OPTION_CB(OPTION_NAME)                                        \
    [this](const wxString& label, const wxAny& value) {                      \
        wxUnusedVar(label);                                                  \
        bool value_bool = false;                                             \
        if(value.GetAs(&value_bool)) {                                       \
            m_options->EnableOption(OptionsConfig::OPTION_NAME, value_bool); \
        }                                                                    \
    }

#define UPDATE_COLOUR_CB(SetterMethod)                          \
    [this](const wxString& label, const wxAny& value) {         \
        wxUnusedVar(label);                                     \
        wxColour colour_value;                                  \
        if(value.GetAs(&colour_value) && colour_value.IsOk()) { \
            m_options->SetterMethod(colour_value);              \
        }                                                       \
    }

#define UPDATE_TEXT_CB(SetterMethod)                    \
    [this](const wxString& label, const wxAny& value) { \
        wxUnusedVar(label);                             \
        wxString str_value;                             \
        if(value.GetAs(&str_value)) {                   \
            m_options->SetterMethod(str_value);         \
        }                                               \
    }

#define UPDATE_INT_CB(SetterMethod)                     \
    [this](const wxString& label, const wxAny& value) { \
        wxUnusedVar(label);                             \
        long long_value;                                \
        if(value.GetAs(&long_value)) {                  \
            m_options->SetterMethod(long_value);        \
        }                                               \
    }

class OptionsConfigPage : public clPropertiesPage
{
protected:
    OptionsConfigPtr m_options;
    bool m_isRestartRequired = false;

public:
    OptionsConfigPage(wxWindow* parent, OptionsConfigPtr options)
        : clPropertiesPage(parent)
        , m_options(options)
    {
    }
    virtual ~OptionsConfigPage() {}
    bool IsRestartRequired() const { return m_isRestartRequired; }
};

#endif // __TREEBOOKNODEBASE__
