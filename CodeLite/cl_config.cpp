//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cl_config.cpp
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

#include "cl_config.h"
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/log.h>
#include <algorithm>
#include "cl_standard_paths.h"

#define ADD_OBJ_IF_NOT_EXISTS(parent, objName) \
    if ( !parent.hasNamedObject( objName ) ) { \
        JSONElement obj = JSONElement::createObject( objName ); \
        parent.append( obj ); \
    }\

#define ADD_ARR_IF_NOT_EXISTS(parent, arrName) \
    if ( !parent.hasNamedObject( arrName ) ) { \
        JSONElement arr = JSONElement::createArray( arrName ); \
        parent.append( arr ); \
    }\
    
clConfig::clConfig(const wxString& filename)
{
    if ( wxFileName(filename).IsAbsolute() ) {
        m_filename = filename;
    } else {
        m_filename = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" + wxFileName::GetPathSeparator() + filename;
    }
    
    {
        // Make sure that the directory exists
        wxLogNull noLog;
        wxMkdir( clStandardPaths::Get().GetUserDataDir() );
        wxMkdir( m_filename.GetPath() );
    }
    
    if ( m_filename.FileExists() ) {
        m_root = new JSONRoot(m_filename);
        
    } else {
        m_root = new JSONRoot(cJSON_Object);
    }
}

clConfig::~clConfig()
{
    m_root->save( m_filename );
    wxDELETE(m_root);
}

clConfig& clConfig::Get()
{
    static clConfig config;
    return config;
}

bool clConfig::GetWorkspaceTabOrder(wxArrayString& tabs, int& selected)
{
    if ( m_root->toElement().hasNamedObject("workspaceTabOrder") ) {
        JSONElement element = m_root->toElement().namedObject("workspaceTabOrder");
        tabs = element.namedObject("tabs").toArrayString();
        selected = element.namedObject("selected").toInt();
        return true;
    }
    return false;
}

void clConfig::SetWorkspaceTabOrder(const wxArrayString& tabs, int selected)
{
    DoDeleteProperty("workspaceTabOrder");
    
    // first time
    JSONElement e = JSONElement::createObject("workspaceTabOrder");
    e.addProperty("tabs", tabs);
    e.addProperty("selected", selected);
    m_root->toElement().append( e );
    
    m_root->save(m_filename);
}

void clConfig::DoDeleteProperty(const wxString& property)
{
    if ( m_root->toElement().hasNamedObject(property) ) {
        m_root->toElement().removeProperty(property);
    }
}

bool clConfig::ReadItem(clConfigItem* item, const wxString &differentName)
{
    wxString nameToUse = differentName.IsEmpty() ? item->GetName() : differentName;
    if ( m_root->toElement().hasNamedObject(nameToUse) ) {
        item->FromJSON( m_root->toElement().namedObject(nameToUse));
        return true;
    }
    return false;
}

void clConfig::WriteItem(const clConfigItem* item, const wxString &differentName)
{
    wxString nameToUse = differentName.IsEmpty() ? item->GetName() : differentName;
    DoDeleteProperty(nameToUse);
    m_root->toElement().append(item->ToJSON());
    m_root->save(m_filename);
}

void clConfig::Reload() 
{
    if ( m_filename.FileExists() == false )
        return;
        
    delete m_root;
    m_root = new JSONRoot(m_filename);
}

wxArrayString clConfig::MergeArrays(const wxArrayString& arr1, const wxArrayString& arr2) const
{
    wxArrayString sArr1, sArr2;
    sArr1.insert(sArr1.end(), arr1.begin(), arr1.end());
    sArr2.insert(sArr2.end(), arr2.begin(), arr2.end());
    
    // Sort the arrays
    std::sort(sArr1.begin(), sArr1.end());
    std::sort(sArr2.begin(), sArr2.end());
    
    wxArrayString output;
    std::set_union(sArr1.begin(), sArr1.end(), sArr2.begin(), sArr2.end(), std::back_inserter(output));
    return output;
}

JSONElement::wxStringMap_t clConfig::MergeStringMaps(const JSONElement::wxStringMap_t& map1, const JSONElement::wxStringMap_t& map2) const
{
    JSONElement::wxStringMap_t output;
    output.insert(map1.begin(), map1.end());
    output.insert(map2.begin(), map2.end());
    return output;
}

void clConfig::Save()
{
    if ( m_root )
        m_root->save(m_filename);
}

void clConfig::Save(const wxFileName& fn)
{
    if ( m_root )
        m_root->save(fn);
}

JSONElement clConfig::GetGeneralSetting()
{
    if ( !m_root->toElement().hasNamedObject("General") ) {
        JSONElement general = JSONElement::createObject("General");
        m_root->toElement().append(general);
    }
    return m_root->toElement().namedObject("General");
}

void clConfig::Write(const wxString& name, bool value)
{
    JSONElement general = GetGeneralSetting();
    if (general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

bool clConfig::Read(const wxString& name, bool defaultValue)
{
    JSONElement general = GetGeneralSetting();
    if (general.namedObject(name).isBool()) {
        return general.namedObject(name).toBool();
    }

    return defaultValue;
}

void clConfig::Write(const wxString& name, int value)
{
    JSONElement general = GetGeneralSetting();
    if (general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

int clConfig::Read(const wxString& name, int defaultValue)
{
    JSONElement general = GetGeneralSetting();
    return general.namedObject(name).toInt(defaultValue);
}

void clConfig::Write(const wxString& name, const wxString& value)
{
    JSONElement general = GetGeneralSetting();
    if (general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

wxString clConfig::Read(const wxString& name, const wxString& defaultValue)
{
    JSONElement general = GetGeneralSetting();
    if (general.namedObject(name).isString()) {
        return general.namedObject(name).toString();
    }

    return defaultValue;
}

int clConfig::GetAnnoyingDlgAnswer(const wxString& name, int defaultValue)
{
    if ( m_root->toElement().hasNamedObject("AnnoyingDialogsAnswers") ) {
        
        JSONElement element = m_root->toElement().namedObject("AnnoyingDialogsAnswers");
        if ( element.hasNamedObject(name) ) {
            return element.namedObject(name).toInt(defaultValue);
        }
    }
    return defaultValue;
}

void clConfig::SetAnnoyingDlgAnswer(const wxString& name, int value)
{
    if ( !m_root->toElement().hasNamedObject("AnnoyingDialogsAnswers") ) {
        JSONElement element = JSONElement::createObject("AnnoyingDialogsAnswers");
        m_root->toElement().append(element);
    }
    
    JSONElement element =m_root->toElement().namedObject("AnnoyingDialogsAnswers");
    if ( element.hasNamedObject(name) ) {
        element.removeProperty(name);
    }
    element.addProperty(name, value);
    Save();
}

void clConfig::ClearAnnoyingDlgAnswers()
{
    DoDeleteProperty("AnnoyingDialogsAnswers");
    Save();
    Reload();
}

void clConfig::AddQuickFindReplaceItem(const wxString& str)
{
    ADD_OBJ_IF_NOT_EXISTS( m_root->toElement(), "QuickFindBar" );
    
    JSONElement quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    ADD_ARR_IF_NOT_EXISTS( quickFindBar, "ReplaceHistory" );
    
    JSONElement arr = quickFindBar.namedObject("ReplaceHistory");
    wxArrayString items = arr.toArrayString();
    
    // Update the array
    int where = items.Index(str);
    if ( where != wxNOT_FOUND ) {
        items.RemoveAt(where);
        items.Insert(str, 0);
        
    } else {
        // remove overflow items if needed
        if ( items.GetCount() > 20 ) {
            // remove last item
            items.RemoveAt( items.GetCount() - 1);
        }
        items.Insert(str, 0);
    }
    
    quickFindBar.removeProperty( "ReplaceHistory" );
    quickFindBar.addProperty("ReplaceHistory", items);
    Save();
}

void clConfig::AddQuickFindSearchItem(const wxString& str)
{
    ADD_OBJ_IF_NOT_EXISTS( m_root->toElement(), "QuickFindBar" );
    
    JSONElement quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    ADD_ARR_IF_NOT_EXISTS( quickFindBar, "SearchHistory" );
    
    JSONElement arr = quickFindBar.namedObject("SearchHistory");
    wxArrayString items = arr.toArrayString();
    
    // Update the array
    int where = items.Index(str);
    if ( where != wxNOT_FOUND ) {
        items.RemoveAt(where);
        items.Insert(str, 0);
        
    } else {
        // remove overflow items if needed
        if ( items.GetCount() > 20 ) {
            // remove last item
            items.RemoveAt( items.GetCount() - 1);
        }
        items.Insert(str, 0);
    }

    quickFindBar.removeProperty( "SearchHistory" );
    quickFindBar.addProperty("SearchHistory", items);
    Save();
}

wxArrayString clConfig::GetQuickFindReplaceItems() const
{
    ADD_OBJ_IF_NOT_EXISTS( m_root->toElement(), "QuickFindBar" );
    JSONElement quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    ADD_ARR_IF_NOT_EXISTS( quickFindBar, "ReplaceHistory" );
    return quickFindBar.namedObject("ReplaceHistory").toArrayString();
}

wxArrayString clConfig::GetQuickFindSearchItems() const
{
    ADD_OBJ_IF_NOT_EXISTS( m_root->toElement(), "QuickFindBar" );
    JSONElement quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    ADD_ARR_IF_NOT_EXISTS( quickFindBar, "SearchHistory" );
    return quickFindBar.namedObject("SearchHistory").toArrayString();
}
