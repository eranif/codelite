//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBVariable.cpp
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

#include "LLDBVariable.h"
#include "JSON.h"
#include "LLDBEnums.h"

#if BUILD_CODELITE_LLDB
#include <lldb/API/SBValue.h>
#include <lldb/API/SBStream.h>

LLDBVariable::LLDBVariable(lldb::SBValue value)
    : m_isWatch(false)
{
    DoInitFromLLDBValue(value);
}

void LLDBVariable::DoInitFromLLDBValue(lldb::SBValue value)
{
    SetName(value.GetName());
    SetType(value.GetTypeName());
    SetSummary(value.GetSummary());
    SetValue(value.GetValue());
    SetValueChanged(value.GetValueDidChange());
    SetLldbId(value.GetID());

    lldb::SBStream stream;
    if(value.GetExpressionPath(stream)) {
        m_expression.assign(stream.GetData(), stream.GetSize());
    }

    if(value.MightHaveChildren()) {
        m_hasChildren = true;
    }
}
#endif

LLDBVariable::~LLDBVariable() {}

void LLDBVariable::FromJSON(const JSONItem& json)
{
    m_name = json.namedObject("m_name").toString();
    m_value = json.namedObject("m_value").toString();
    m_summary = json.namedObject("m_summary").toString();
    m_type = json.namedObject("m_type").toString();
    m_expression = json.namedObject("m_expression").toString();
    m_valueChanged = json.namedObject("m_valueChanged").toBool(false);
    m_lldbId = json.namedObject("m_lldbId").toInt();
    m_hasChildren = json.namedObject("m_hasChildren").toBool(false);
    m_isWatch = json.namedObject("m_isWatch").toBool(m_isWatch);
}

JSONItem LLDBVariable::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("m_name", m_name);
    json.addProperty("m_value", m_value);
    json.addProperty("m_summary", m_summary);
    json.addProperty("m_type", m_type);
    json.addProperty("m_expression", m_expression);
    json.addProperty("m_valueChanged", m_valueChanged);
    json.addProperty("m_lldbId", m_lldbId);
    json.addProperty("m_hasChildren", m_hasChildren);
    json.addProperty("m_isWatch", m_isWatch);
    return json;
}

wxString LLDBVariable::ToString(const wxString& alternateName) const
{
    wxString asString;
    if(alternateName.IsEmpty()) {
        asString << GetName();
    } else {
        asString << alternateName;
    }

    wxString v;
    if(!GetSummary().IsEmpty()) {
        v << GetSummary();
    }

    if(!GetValue().IsEmpty()) {
        if(!v.IsEmpty()) {
            v << " ";
        }
        v << GetValue();
    }

    v.Trim().Trim(false);
    if(v == "unable to read data") {
        v.Clear();
    }

    if(!v.IsEmpty()) {
        asString << " = " << v;
    }

    if(!m_type.IsEmpty()) {
        asString << " [" << GetType() << "]";
    }
    return asString;
}
