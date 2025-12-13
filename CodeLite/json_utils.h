//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2025 Eran Ifrah
// file name            : json_utils.h
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
#pragma once

#include "StringUtils.h"
#include "macros.h"

#include <assistant/common/json.hpp> // <nlohmann/json.hpp>
#include <string>
#include <wx/arrstr.h>
#include <wx/gdicmn.h>
#include <wx/string.h>

namespace JsonUtils
{

inline wxString ToString(const nlohmann::json& json)
{
    if (!json.is_string()) {
        return wxEmptyString;
    }
    return wxString::FromUTF8(json);
}


inline wxArrayString ToArrayString(const nlohmann::json& json)
{
    if (!json.is_array()) {
        return {};
    }
    wxArrayString res;
    for (const auto& item : json) {
        res.push_back(ToString(item));
    }
    return res;
}

inline wxSize ToSize(const nlohmann::json& json)
{
    const auto str = ToString(json);
    wxString x = str.BeforeFirst(',');
    wxString y = str.AfterFirst(',');

    int nX(-1), nY(-1);
    if (!x.ToInt(&nX) || !y.ToInt(&nY))
        return wxDefaultSize;
    return {nX, nY};
}

inline wxStringMap_t ToStringMap(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return {};
    }
    wxStringMap_t res;
    for (const auto& [key, item] : json.items()) {
        res[ToString(key)] = ToString(item);
    }
    return res;
}

inline std::string ToJsonValue(const wxSize& sz) { return std::to_string(sz.x) + "," + std::to_string(sz.y); }

inline nlohmann::json ToJson(const wxStringMap_t& map)
{
    nlohmann::json json;
    for (const auto& [key, value] : map) {
        json[StringUtils::ToStdString(key)] = StringUtils::ToStdString(value);
    }
    return json;
}

} // namespace JsonUtils
