//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "FontUtils.hpp"
#include "cl_defs.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fileutils.h"
#include "wx/filename.h"

#include <algorithm>
#include <wx/filefn.h>
#include <wx/log.h>
#include <wx/stdpaths.h>

#define ADD_OBJ_IF_NOT_EXISTS(parent, objName)          \
    if(!parent.hasNamedObject(objName)) {               \
        JSONItem obj = JSONItem::createObject(objName); \
        parent.append(obj);                             \
    }

#define ADD_ARR_IF_NOT_EXISTS(parent, arrName)         \
    if(!parent.hasNamedObject(arrName)) {              \
        JSONItem arr = JSONItem::createArray(arrName); \
        parent.append(arr);                            \
    }

namespace
{
void truncate_array(wxArrayString& arr, size_t maxSize)
{
    while(arr.GetCount() > maxSize && arr.GetCount() > 0) {
        arr.RemoveAt(arr.GetCount() - 1);
    }
}
} // namespace

clConfig::clConfig(const wxString& filename)
{
    if(wxFileName(filename).IsAbsolute()) {
        m_filename = filename;
    } else {
        m_filename = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
                     wxFileName::GetPathSeparator() + filename;
    }

    if(m_filename.FileExists()) {
        m_root = new JSON(m_filename);

    } else {
        if(!m_filename.DirExists()) {
            m_filename.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        m_root = new JSON(cJSON_Object);
    }

    // Load the "Recent Items" cache
    {
        wxArrayString recentFiles;
        JSONItem e = m_root->toElement();
        if(e.hasNamedObject("RecentWorkspaces")) {
            recentFiles = e.namedObject("RecentWorkspaces").toArrayString();
            m_cacheRecentItems.insert(std::make_pair("RecentWorkspaces", recentFiles));
        }
    }

    {
        wxArrayString recentFiles;
        JSONItem e = m_root->toElement();
        if(e.hasNamedObject("RecentFiles")) {
            recentFiles = e.namedObject("RecentFiles").toArrayString();
            m_cacheRecentItems.insert(std::make_pair("RecentFiles", recentFiles));
        }
    }
}

clConfig::~clConfig() { wxDELETE(m_root); }

clConfig& clConfig::Get()
{
    static clConfig config;
    return config;
}

bool clConfig::GetOutputTabOrder(wxArrayString& tabs, int& selected)
{
    if(m_root->toElement().hasNamedObject("outputTabOrder")) {
        JSONItem element = m_root->toElement().namedObject("outputTabOrder");
        tabs = element.namedObject("tabs").toArrayString();
        selected = element.namedObject("selected").toInt();
        return true;
    }
    return false;
}

void clConfig::SetOutputTabOrder(const wxArrayString& tabs, int selected)
{
    DoDeleteProperty("outputTabOrder");

    // first time
    JSONItem e = JSONItem::createObject("outputTabOrder");
    e.addProperty("tabs", tabs);
    e.addProperty("selected", selected);
    m_root->toElement().append(e);
    m_root->save(m_filename);
}

bool clConfig::GetWorkspaceTabOrder(wxArrayString& tabs, int& selected)
{
    if(m_root->toElement().hasNamedObject("workspaceTabOrder")) {
        JSONItem element = m_root->toElement().namedObject("workspaceTabOrder");
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
    JSONItem e = JSONItem::createObject("workspaceTabOrder");
    e.addProperty("tabs", tabs);
    e.addProperty("selected", selected);
    m_root->toElement().append(e);

    m_root->save(m_filename);
}

void clConfig::DoDeleteProperty(const wxString& property)
{
    if(m_root->toElement().hasNamedObject(property)) {
        m_root->toElement().removeProperty(property);
    }
}

bool clConfig::ReadItem(clConfigItem* item, const wxString& differentName)
{
    wxString nameToUse = differentName.IsEmpty() ? item->GetName() : differentName;
    if(m_root->toElement().hasNamedObject(nameToUse)) {
        item->FromJSON(m_root->toElement().namedObject(nameToUse));
        return true;
    }
    return false;
}

bool clConfig::Write(const wxString& name, std::function<JSONItem()> serialiser_func, const wxFileName& configFile)
{
    auto item = serialiser_func();
    if(configFile.IsOk()) {
        // write to a separate file
        configFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        return FileUtils::WriteFileContent(configFile, item.format());
    } else {
        // add it to the global configuration file
        DoDeleteProperty(name);
        item.setName(name);
        m_root->toElement().append(item);
        return true;
    }
}

void clConfig::Read(const wxString& name, std::function<void(const JSONItem& item)> deserialiser_func,
                    const wxFileName& configFile)
{
    if(configFile.IsOk() && configFile.FileExists()) {
        JSON root(configFile);
        if(!root.isOk()) {
            return;
        }
        deserialiser_func(root.toElement());
    } else {
        auto root = m_root->toElement();
        if(root.hasNamedObject(name)) {
            deserialiser_func(root[name]);
        }
    }
}

void clConfig::WriteItem(const clConfigItem* item, const wxString& differentName)
{
    wxString nameToUse = differentName.IsEmpty() ? item->GetName() : differentName;
    DoDeleteProperty(nameToUse);
    m_root->toElement().append(item->ToJSON());
    m_root->save(m_filename);
}

void clConfig::Reload()
{
    if(m_filename.FileExists() == false)
        return;

    delete m_root;
    m_root = new JSON(m_filename);
}

wxArrayString clConfig::MergeArrays(const wxArrayString& arr1, const wxArrayString& arr2) const
{
    // we use set to keep the records sorted
    std::set<wxString> visited;

    for(const wxString& element : arr1) {
        if(visited.count(element))
            continue;
        visited.insert(element);
    }

    wxArrayString merged_array;
    merged_array.reserve(visited.size());
    for(const wxString& element : visited) {
        merged_array.Add(element);
    }
    return merged_array;
}

wxStringMap_t clConfig::MergeStringMaps(const wxStringMap_t& map1, const wxStringMap_t& map2) const
{
    wxStringMap_t output;
    output.reserve(map1.size() + map2.size());

    output.insert(map1.begin(), map1.end());
    output.insert(map2.begin(), map2.end());
    return output;
}

void clConfig::Save()
{
    if(m_root) {
        LOG_IF_TRACE { clDEBUG1() << "Config file:" << m_filename << "saved!" << endl; }
        m_root->save(m_filename);
    }
}

void clConfig::Save(const wxFileName& fn)
{
    if(m_root)
        m_root->save(fn);
}

JSONItem clConfig::GetGeneralSetting()
{
    if(!m_root->toElement().hasNamedObject("General")) {
        JSONItem general = JSONItem::createObject("General");
        m_root->toElement().append(general);
    }
    return m_root->toElement().namedObject("General");
}

void clConfig::Write(const wxString& name, bool value)
{
    JSONItem general = GetGeneralSetting();
    if(general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

bool clConfig::Read(const wxString& name, bool defaultValue)
{
#if CL_USE_NATIVEBOOK
    if(name == "UseCustomBaseColour")
        return false;
#endif
    JSONItem general = GetGeneralSetting();
    if(general.namedObject(name).isBool()) {
        return general.namedObject(name).toBool();
    }
    return defaultValue;
}

void clConfig::Write(const wxString& name, int value)
{
    JSONItem general = GetGeneralSetting();
    if(general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

int clConfig::Read(const wxString& name, int defaultValue)
{
    JSONItem general = GetGeneralSetting();
    return general.namedObject(name).toInt(defaultValue);
}

void clConfig::Write(const wxString& name, const wxString& value)
{
    JSONItem general = GetGeneralSetting();
    if(general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

wxString clConfig::Read(const wxString& name, const wxString& defaultValue)
{
    JSONItem general = GetGeneralSetting();
    if(general.namedObject(name).isString()) {
        return general.namedObject(name).toString();
    }

    return defaultValue;
}

int clConfig::GetAnnoyingDlgAnswer(const wxString& name, int defaultValue)
{
    if(m_root->toElement().hasNamedObject("AnnoyingDialogsAnswers")) {

        JSONItem element = m_root->toElement().namedObject("AnnoyingDialogsAnswers");
        if(element.hasNamedObject(name)) {
            return element.namedObject(name).toInt(defaultValue);
        }
    }
    return defaultValue;
}

void clConfig::SetAnnoyingDlgAnswer(const wxString& name, int value)
{
    if(!m_root->toElement().hasNamedObject("AnnoyingDialogsAnswers")) {
        JSONItem element = JSONItem::createObject("AnnoyingDialogsAnswers");
        m_root->toElement().append(element);
    }

    JSONItem element = m_root->toElement().namedObject("AnnoyingDialogsAnswers");
    if(element.hasNamedObject(name)) {
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

void clConfig::SetQuickFindSearchItems(const wxArrayString& items)
{
    ADD_OBJ_IF_NOT_EXISTS(m_root->toElement(), "QuickFindBar");
    JSONItem quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    if(quickFindBar.hasNamedObject("SearchHistory")) {
        quickFindBar.removeProperty("SearchHistory");
    }

    wxArrayString items_to_save = items;
    truncate_array(items_to_save, 20);

    quickFindBar.addProperty("SearchHistory", items_to_save);
    Save();
}

void clConfig::SetQuickFindReplaceItems(const wxArrayString& items)
{
    ADD_OBJ_IF_NOT_EXISTS(m_root->toElement(), "QuickFindBar");
    JSONItem quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    if(quickFindBar.hasNamedObject("ReplaceHistory")) {
        quickFindBar.removeProperty("ReplaceHistory");
    }

    wxArrayString items_to_save = items;
    truncate_array(items_to_save, 20);

    quickFindBar.addProperty("ReplaceHistory", items_to_save);
    Save();
}

void clConfig::AddQuickFindReplaceItem(const wxString& str)
{
    ADD_OBJ_IF_NOT_EXISTS(m_root->toElement(), "QuickFindBar");

    JSONItem quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    ADD_ARR_IF_NOT_EXISTS(quickFindBar, "ReplaceHistory");

    JSONItem arr = quickFindBar.namedObject("ReplaceHistory");
    wxArrayString items = arr.toArrayString();

    // Update the array
    int where = items.Index(str);
    if(where != wxNOT_FOUND) {
        items.RemoveAt(where);
        items.Insert(str, 0);

    } else {
        // remove overflow items if needed
        if(items.GetCount() > 20) {
            // remove last item
            items.RemoveAt(items.GetCount() - 1);
        }
        items.Insert(str, 0);
    }

    quickFindBar.removeProperty("ReplaceHistory");
    quickFindBar.addProperty("ReplaceHistory", items);
    Save();
}

void clConfig::AddQuickFindSearchItem(const wxString& str)
{
    ADD_OBJ_IF_NOT_EXISTS(m_root->toElement(), "QuickFindBar");

    JSONItem quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    ADD_ARR_IF_NOT_EXISTS(quickFindBar, "SearchHistory");

    JSONItem arr = quickFindBar.namedObject("SearchHistory");
    wxArrayString items = arr.toArrayString();

    // Update the array
    int where = items.Index(str);
    if(where != wxNOT_FOUND) {
        items.RemoveAt(where);
    }
    items.Insert(str, 0);

    // Reudce to size to max of 20
    while(items.size() > 20) {
        items.RemoveAt(items.size() - 1);
    }

    // Update the array
    quickFindBar.removeProperty("SearchHistory");
    quickFindBar.addProperty("SearchHistory", items);
    Save();
}

wxArrayString clConfig::GetQuickFindReplaceItems() const
{
    ADD_OBJ_IF_NOT_EXISTS(m_root->toElement(), "QuickFindBar");
    JSONItem quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    ADD_ARR_IF_NOT_EXISTS(quickFindBar, "ReplaceHistory");
    return quickFindBar.namedObject("ReplaceHistory").toArrayString();
}

wxArrayString clConfig::GetQuickFindSearchItems() const
{
    ADD_OBJ_IF_NOT_EXISTS(m_root->toElement(), "QuickFindBar");
    JSONItem quickFindBar = m_root->toElement().namedObject("QuickFindBar");
    ADD_ARR_IF_NOT_EXISTS(quickFindBar, "SearchHistory");
    return quickFindBar.namedObject("SearchHistory").toArrayString();
}

wxArrayString clConfig::Read(const wxString& name, const wxArrayString& defaultValue)
{
    JSONItem general = GetGeneralSetting();
    if(general.hasNamedObject(name)) {
        return general.namedObject(name).toArrayString();
    }
    return defaultValue;
}

void clConfig::Write(const wxString& name, const wxArrayString& value)
{
    JSONItem general = GetGeneralSetting();
    if(general.hasNamedObject(name)) {
        general.removeProperty(name);
    }

    general.addProperty(name, value);
    Save();
}

void clConfig::DoAddRecentItem(const wxString& propName, const wxString& filename)
{
    wxArrayString recentItems = DoGetRecentItems(propName);

    // Prepend the item
    if(recentItems.Index(filename) != wxNOT_FOUND) {
        recentItems.Remove(filename);
    }

    if(!wxFileName(filename).FileExists()) {
        // Don't add non existing file
        return;
    }

    recentItems.Insert(filename, 0);

    // Make sure the list does not go over 15 items
    while(recentItems.size() >= 15) {
        recentItems.RemoveAt(recentItems.size() - 1);
    }

    // Remove non existing items
    wxArrayString existingFiles;
    for(size_t i = 0; i < recentItems.size(); ++i) {
        if(wxFileName(recentItems.Item(i)).FileExists()) {
            existingFiles.Add(recentItems.Item(i));
        }
    }
    recentItems.swap(existingFiles);

    // Remove old node if exists
    JSONItem e = m_root->toElement();
    if(e.hasNamedObject(propName)) {
        e.removeProperty(propName);
    }

    // append new property
    e.addProperty(propName, recentItems);

    // update the cache
    if(m_cacheRecentItems.count(propName)) {
        m_cacheRecentItems.erase(propName);
    }

    m_cacheRecentItems.insert(std::make_pair(propName, recentItems));
    m_root->save(m_filename);
}

void clConfig::DoClearRecentItems(const wxString& propName)
{
    JSONItem e = m_root->toElement();
    if(e.hasNamedObject(propName)) {
        e.removeProperty(propName);
    }
    m_root->save(m_filename);
    // update the cache
    if(m_cacheRecentItems.count(propName)) {
        m_cacheRecentItems.erase(propName);
    }
}

wxArrayString clConfig::DoGetRecentItems(const wxString& propName) const
{
    wxArrayString recentItems;

    // Try the cache first
    if(m_cacheRecentItems.count(propName)) {
        recentItems = m_cacheRecentItems.find(propName)->second;

    } else {
        JSONItem e = m_root->toElement();
        if(e.hasNamedObject(propName)) {
            recentItems = e.namedObject(propName).toArrayString();
        }
    }
    return recentItems;
}
#if wxUSE_GUI
wxFont clConfig::Read(const wxString& name, const wxFont& defaultValue)
{
    JSONItem general = GetGeneralSetting();
    if(!general.hasNamedObject(name))
        return defaultValue;

    // Unserialize the font
    wxFont font;
    JSONItem f = general.namedObject(name);
    if(!f.hasNamedObject("fontDesc"))
        return defaultValue;

    font.SetNativeFontInfo(FontUtils::GetFontInfo(f.namedObject("fontDesc").toString()));
    return font;
}

void clConfig::Write(const wxString& name, const wxFont& value)
{
    JSONItem font = JSONItem::createObject(name);
    font.addProperty("fontDesc", FontUtils::GetFontInfo(value));

    JSONItem general = GetGeneralSetting();
    if(general.hasNamedObject(name)) {
        general.removeProperty(name);
    }
    general.append(font);
    Save();
}

wxColour clConfig::Read(const wxString& name, const wxColour& defaultValue)
{
    wxString strValue;
    strValue = Read(name, wxString());
    if(strValue.IsEmpty()) {
        return defaultValue;
    }
    wxColour col(strValue);
    return col;
}

void clConfig::Write(const wxString& name, const wxColour& value)
{
    wxString strValue = value.GetAsString(wxC2S_HTML_SYNTAX);
    Write(name, strValue);
    Save();
}

#endif
