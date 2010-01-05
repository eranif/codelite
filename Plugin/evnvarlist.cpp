//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : evnvarlist.cpp
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
#include "evnvarlist.h"
#include <wx/tokenzr.h>
EvnVarList::EvnVarList()
: m_activeSet(wxT("Default"))
{
}

EvnVarList::~EvnVarList()
{
}

void EvnVarList::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_envVarSets"), m_envVarSets);
	arch.Read(wxT("m_activeSet"),  m_activeSet);
}

void EvnVarList::Serialize(Archive &arch)
{
	arch.Write(wxT("m_envVarSets"), m_envVarSets);
	arch.Write(wxT("m_activeSet"),  m_activeSet);
}

void EvnVarList::AddVariable(const wxString &setName, const wxString& name, const wxString& value)
{
	wxString newEntry, actualSetName;
	newEntry << name << wxT("=") << value;

	wxString currentValueStr = DoGetSetVariablesStr(setName, actualSetName);
	wxArrayString currentValues = wxStringTokenize(currentValueStr, wxT("\r\n"), wxTOKEN_STRTOK);

	if(currentValues.Index(newEntry) == wxNOT_FOUND)
		currentValues.Add(newEntry);

	currentValueStr.Clear();
	for(size_t i=0; i<currentValues.GetCount(); i++){
		currentValueStr << currentValues.Item(i) << wxT("\n");
	}

	if(currentValueStr.IsEmpty() == false)
		currentValueStr.RemoveLast();

	m_envVarSets[actualSetName] = currentValueStr;
}

std::map<wxString,wxString> EvnVarList::GetVariables(const wxString& setName)
{
	std::map<wxString,wxString> variables;
	wxString                    actualSetName;

	wxString      currentValueStr = DoGetSetVariablesStr(setName, actualSetName);
	wxArrayString currentValues   = wxStringTokenize(currentValueStr, wxT("\r\n"), wxTOKEN_STRTOK);
	for(size_t i=0; i<currentValues.GetCount(); i++){
		wxString line = currentValues.Item(i);
		line.Trim().Trim(false);
		wxString varname  = line.BeforeFirst(wxT('='));
		wxString varvalue = line.AfterFirst(wxT('='));
		variables[varname] = varvalue;
	}
	return variables;
}

wxString EvnVarList::DoGetSetVariablesStr(const wxString& setName, wxString& selectedSetName)
{
	wxString currentValueStr;

	selectedSetName = setName;
	std::map<wxString, wxString>::iterator iter = m_envVarSets.find(setName);
	if(iter != m_envVarSets.end())
		currentValueStr = iter->second;
	else {
		iter = m_envVarSets.find(m_activeSet);
		if(iter != m_envVarSets.end()) {
			currentValueStr = iter->second;
			selectedSetName = m_activeSet;
		} else {
			selectedSetName = wxT("Default");
			iter = m_envVarSets.find(selectedSetName);
			if(iter != m_envVarSets.end())
				currentValueStr = iter->second;
		}
	}
	return currentValueStr;
}
