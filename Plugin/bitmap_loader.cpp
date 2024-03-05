//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : bitmap_loader.cpp
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
#include "bitmap_loader.h"

#include "clBitmap.h"
#include "clFilesCollector.h"
#include "clSystemSettings.h"
#include "clZipReader.h"
#include "cl_standard_paths.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "optionsconfig.h"

#include <algorithm>
#include <array>
#include <unordered_map>
#include <wx/app.h>
#include <wx/dcscreen.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

namespace
{
std::unordered_map<wxString, wxBitmapBundle> DARK_THEME_BMPBUNLES;
std::unordered_map<wxString, wxBitmapBundle> LIGHT_THEME_BMPBUNLES;
}; // namespace

BitmapLoader::~BitmapLoader() {}

BitmapLoader::BitmapLoader(bool darkTheme) { Initialize(darkTheme); }

std::unordered_map<wxString, wxBitmapBundle>* BitmapLoader::GetBundles(bool darkTheme) const
{
    return darkTheme ? &DARK_THEME_BMPBUNLES : &LIGHT_THEME_BMPBUNLES;
}

const wxBitmap& BitmapLoader::LoadBitmap(const wxString& name, int requestedSize)
{
    // try to load a new bitmap first
    wxUnusedVar(requestedSize);
    wxString newName = name.AfterLast('/');

    if (m_toolbarsBitmaps.count(newName) == 0) {
        LOG_IF_WARN { clWARNING() << "requested image:" << newName << "does not exist" << endl; }
        return wxNullBitmap;
    }
    return m_toolbarsBitmaps.find(newName)->second;
}

int BitmapLoader::GetMimeImageId(int type, bool disabled) { return GetMimeBitmaps().GetIndex(type, disabled); }

int BitmapLoader::GetMimeImageId(const wxString& filename, bool disabled)
{
    return GetMimeBitmaps().GetIndex(filename, disabled);
}

wxIcon BitmapLoader::GetIcon(const wxBitmap& bmp) const
{
    wxIcon icn;
    icn.CopyFromBitmap(bmp);
    return icn;
}

void BitmapLoader::AddBitmapInternal(const wxBitmapBundle& bundle, const wxString& base_name)
{
    wxBitmap bmp = bundle.GetBitmapFor(wxTheApp->GetTopWindow());
    if (bmp.IsOk()) {
        m_toolbarsBitmaps.insert({ base_name, bmp });
    }
}

void BitmapLoader::LoadSVGFiles(bool darkTheme)
{
    // Load the bitmaps based on the current theme background colour
    wxFileName svg_path{ clStandardPaths::Get().GetDataDir(), wxEmptyString };
    svg_path.AppendDir("svgs");
    svg_path.AppendDir(darkTheme ? "dark-theme" : "light-theme");

    if (!svg_path.DirExists()) {
        clWARNING() << "Unable to load SVG images. Broken installation" << endl;
        return;
    }
    auto bitmap_bundle_cache = GetBundles(darkTheme);

    // load the cache
    if (bitmap_bundle_cache->empty()) {
        clFilesScanner scanner;
        clDEBUG() << "Loading SVG files from:" << svg_path.GetPath() << endl;
        scanner.ScanWithCallbacks(svg_path.GetPath(), nullptr, [&](const wxArrayString& files) -> bool {
            for (const wxString& filepath : files) {
                auto bmpbundle = wxBitmapBundle::FromSVGFile(filepath, wxSize(16, 16));
                if (bmpbundle.IsOk()) {
                    bitmap_bundle_cache->insert({ wxFileName(filepath).GetName(), bmpbundle });
                }
            }
            return true;
        });
    }
}

void BitmapLoader::Initialize(bool darkTheme)
{
    LoadSVGFiles(darkTheme);
    auto bitmap_bundle_cache = GetBundles(darkTheme);

    m_toolbarsBitmaps.clear();
    m_toolbarsBitmaps.reserve(bitmap_bundle_cache->size());

    for (const auto& vt : *bitmap_bundle_cache) {
        AddBitmapInternal(vt.second, vt.first);
    }

    // Create the mime-list
    CreateMimeList();
}

void BitmapLoader::CreateMimeList()
{
    if (m_mimeBitmaps.IsEmpty()) {
        m_mimeBitmaps.AddBitmap(LoadBitmap("executable", 16), FileExtManager::TypeExe);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-html", 16), FileExtManager::TypeHtml);
        m_mimeBitmaps.AddBitmap(LoadBitmap("archive", 16), FileExtManager::TypeArchive);
        m_mimeBitmaps.AddBitmap(LoadBitmap("zip", 16), FileExtManager::TypeZip);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-php", 16), FileExtManager::TypePhp);
        m_mimeBitmaps.AddBitmap(LoadBitmap("dll", 16), FileExtManager::TypeDll);
        m_mimeBitmaps.AddBitmap(LoadBitmap("blocks", 16), FileExtManager::TypeFormbuilder);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-txt", 16), FileExtManager::TypeCodedesigner);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-bmp", 16), FileExtManager::TypeBmp);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mk", 16), FileExtManager::TypeMakefile);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-c", 16), FileExtManager::TypeSourceC);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-cpp", 16), FileExtManager::TypeSourceCpp);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-h", 16), FileExtManager::TypeHeader);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-md", 16), FileExtManager::TypeMarkdown);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-txt", 16), FileExtManager::TypeText);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-sh", 16), FileExtManager::TypeShellScript);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-xml", 16), FileExtManager::TypeXml);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-txt", 16), FileExtManager::TypeErd);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-python", 16), FileExtManager::TypePython);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-css", 16), FileExtManager::TypeCSS);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-js", 16), FileExtManager::TypeJS);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cxx-workspace", 16), FileExtManager::TypeWorkspace);
        m_mimeBitmaps.AddBitmap(LoadBitmap("php-workspace", 16), FileExtManager::TypeWorkspacePHP);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow", 16), FileExtManager::TypeWorkspaceFileSystem);
        m_mimeBitmaps.AddBitmap(LoadBitmap("docker", 16), FileExtManager::TypeWorkspaceDocker);
        m_mimeBitmaps.AddBitmap(LoadBitmap("nodejs-workspace", 16), FileExtManager::TypeWorkspaceNodeJS);
        m_mimeBitmaps.AddBitmap(LoadBitmap("project", 16), FileExtManager::TypeProject);
        m_mimeBitmaps.AddBitmap(LoadBitmap("project-opened", 16), FileExtManager::TypeProjectExpanded);
        m_mimeBitmaps.AddBitmap(LoadBitmap("blocks", 16), FileExtManager::TypeWxCrafter);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-xml", 16), FileExtManager::TypeXRC);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-xml", 16), FileExtManager::TypeResource);
        m_mimeBitmaps.AddBitmap(LoadBitmap("sql", 16), FileExtManager::TypeSQL);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow", 16), FileExtManager::TypeFolder);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow-opened", 16), FileExtManager::TypeFolderExpanded);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-as", 16), FileExtManager::TypeAsm);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cmake", 16), FileExtManager::TypeCMake);
        m_mimeBitmaps.AddBitmap(LoadBitmap("qt", 16), FileExtManager::TypeQMake);
        m_mimeBitmaps.AddBitmap(LoadBitmap("docker", 16), FileExtManager::TypeDockerfile);
        m_mimeBitmaps.AddBitmap(LoadBitmap("yml", 16), FileExtManager::TypeYAML);
        m_mimeBitmaps.AddBitmap(LoadBitmap("database", 16), FileExtManager::TypeDatabase);
        m_mimeBitmaps.AddBitmap(LoadBitmap("sqlite", 16), FileExtManager::TypeSLite);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-svg", 16), FileExtManager::TypeSvg);
        m_mimeBitmaps.AddBitmap(LoadBitmap("workspace-folder-yellow-opened", 16),
                                FileExtManager::TypeWorkspaceFolderExpanded);
        m_mimeBitmaps.AddBitmap(LoadBitmap("workspace-folder-yellow", 16), FileExtManager::TypeWorkspaceFolder);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow-opened-symlink", 16),
                                FileExtManager::TypeFolderSymlinkExpanded);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow-symlink", 16), FileExtManager::TypeFolderSymlink);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-txt-symlink", 16), FileExtManager::TypeFileSymlink);
        m_mimeBitmaps.AddBitmap(LoadBitmap("rust", 16), FileExtManager::TypeRust);
        m_mimeBitmaps.AddBitmap(LoadBitmap("diff", 16), FileExtManager::TypeDiff);
        m_mimeBitmaps.AddBitmap(LoadBitmap("patch", 16), FileExtManager::TypePatch);
        m_mimeBitmaps.AddBitmap(LoadBitmap("ruby", 16), FileExtManager::TypeRuby);
        m_mimeBitmaps.AddBitmap(LoadBitmap("json", 16), FileExtManager::TypeJSON);
        m_mimeBitmaps.AddBitmap(LoadBitmap("phar", 16), FileExtManager::TypePhar);
        m_mimeBitmaps.AddBitmap(LoadBitmap("dart", 16), FileExtManager::TypeDart);

        // Non mime bitmaps
        m_mimeBitmaps.AddBitmap(LoadBitmap("file_save", 16), kSave);
        m_mimeBitmaps.AddBitmap(LoadBitmap("file_save_all", 16), kSaveAll);
        m_mimeBitmaps.AddBitmap(LoadBitmap("file_close", 16), kClose);
        m_mimeBitmaps.AddBitmap(LoadBitmap("db-table", 16), kTable);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cscope", 16), kFind);
        m_mimeBitmaps.AddBitmap(LoadBitmap("database", 16), kDatabase);
        m_mimeBitmaps.AddBitmap(LoadBitmap("db-column", 16), kColumn);
        m_mimeBitmaps.AddBitmap(LoadBitmap("globals", 16), kAngleBrackets);
        m_mimeBitmaps.AddBitmap(LoadBitmap("namespace", 16), kNamespace);
        m_mimeBitmaps.AddBitmap(LoadBitmap("class", 16), kClass);
        m_mimeBitmaps.AddBitmap(LoadBitmap("struct", 16), kStruct);
        m_mimeBitmaps.AddBitmap(LoadBitmap("function_public", 16), kFunctionPublic);
        m_mimeBitmaps.AddBitmap(LoadBitmap("function_protected", 16), kFunctionProtected);
        m_mimeBitmaps.AddBitmap(LoadBitmap("function_private", 16), kFunctionPrivate);
        m_mimeBitmaps.AddBitmap(LoadBitmap("member_public", 16), kMemberPublic);
        m_mimeBitmaps.AddBitmap(LoadBitmap("member_protected", 16), kMemberProtected);
        m_mimeBitmaps.AddBitmap(LoadBitmap("member_private", 16), kMemberPrivate);
        m_mimeBitmaps.AddBitmap(LoadBitmap("typedef", 16), kTypedef);
        m_mimeBitmaps.AddBitmap(LoadBitmap("macro", 16), kMacro);
        m_mimeBitmaps.AddBitmap(LoadBitmap("enum", 16), kEnum);
        m_mimeBitmaps.AddBitmap(LoadBitmap("enum", 16), kCEnum);
        m_mimeBitmaps.AddBitmap(LoadBitmap("macro", 16), kConstant);
        m_mimeBitmaps.AddBitmap(LoadBitmap("enumerator", 16), kEnumerator);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cpp_keyword", 16), kCxxKeyword);
        m_mimeBitmaps.AddBitmap(LoadBitmap("sort", 16), kSort);
        m_mimeBitmaps.Finalise(); // combine the disabled and non disabled bitmaps
    }
}

const wxBitmap& BitmapLoader::GetBitmapForFile(const wxString& filename, bool disabled) const
{
    FileExtManager::FileType ft = FileExtManager::GetType(filename, FileExtManager::TypeText);
    return m_mimeBitmaps.GetBitmap(ft, disabled);
}

//===---------------------------
// clMimeBitmaps
//===---------------------------

clMimeBitmaps::clMimeBitmaps() {}

clMimeBitmaps::~clMimeBitmaps() {}

int clMimeBitmaps::GetIndex(int type, bool disabled) const
{
    if (m_fileIndexMap.count(type) == 0) {
        return wxNOT_FOUND;
    }
    int offset = 0;
    if (disabled) {
        offset += m_disabled_bitmaps.size();
    }

    int index = m_fileIndexMap.at(type);
    index += offset;
    if (index >= m_bitmaps.size()) {
        index -= offset;
    }
    return index;
}

int clMimeBitmaps::GetIndex(const wxString& filename, bool disabled) const
{
    FileExtManager::Init();
    int ft = FileExtManager::GetType(filename, FileExtManager::TypeText);
    return GetIndex(ft, disabled);
}

void clMimeBitmaps::AddBitmap(const wxBitmap& bitmap, int type)
{
    m_bitmaps.push_back(bitmap);

    // add a disable item as well
    m_disabled_bitmaps.push_back(DrawingUtils::CreateDisabledBitmap(bitmap));

    int index = (m_bitmaps.size() - 1);
    m_fileIndexMap.insert({ type, index });
}

void clMimeBitmaps::Clear()
{
    m_bitmaps.clear();
    m_fileIndexMap.clear();
}

void clMimeBitmaps::Finalise()
{
    std::vector<wxBitmap> all;
    all.reserve(m_bitmaps.size() * 2);
    all.insert(all.end(), m_bitmaps.begin(), m_bitmaps.end());
    all.insert(all.end(), m_disabled_bitmaps.begin(), m_disabled_bitmaps.end());
    m_bitmaps.swap(all);
}

const wxBitmap& clMimeBitmaps::GetBitmap(int type, bool disabled) const
{
    int index = GetIndex(type);
    if (index == wxNOT_FOUND) {
        static wxBitmap emptyBitmap;
        return emptyBitmap;
    }
    if (disabled) {
        index += m_disabled_bitmaps.size();
        if (index >= m_bitmaps.size()) {
            // caller did not call "Finalise"
            index -= m_disabled_bitmaps.size();
        }
    }
    return m_bitmaps.at(index);
}

// ------------------------------------
// clBitmaps
// ------------------------------------
wxDEFINE_EVENT(wxEVT_BITMAPS_UPDATED, clCommandEvent);
clBitmaps::clBitmaps()
{
    Initialise();
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clBitmaps::OnSysColoursChanged, this);
}

clBitmaps::~clBitmaps()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clBitmaps::OnSysColoursChanged, this);
}

clBitmaps& clBitmaps::Get()
{
    static clBitmaps* pBitmaps = nullptr;
    if (!pBitmaps) {
        pBitmaps = new clBitmaps();
    }
    return *pBitmaps;
}

BitmapLoader* clBitmaps::GetLoader() { return m_activeBitmaps; }

void clBitmaps::Initialise()
{
    m_darkBitmaps = new BitmapLoader(true);
    m_lightBitmaps = new BitmapLoader(false);
    SysColoursChanged();
}

void clBitmaps::SysColoursChanged()
{
    auto old_ptr = m_activeBitmaps;
    bool isDark = clSystemSettings::IsDark();
    m_activeBitmaps = isDark ? m_darkBitmaps : m_lightBitmaps;

    if (old_ptr != m_activeBitmaps) {
        // change was made, fire an event
        clCommandEvent event(wxEVT_BITMAPS_UPDATED);
        EventNotifier::Get()->AddPendingEvent(event);
    }
}

void clBitmaps::OnSysColoursChanged(clCommandEvent& event)
{
    event.Skip();
    SysColoursChanged();
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// clBitmapList
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void clBitmapList::OnBitmapsUpdated(clCommandEvent& event)
{
    event.Skip();
    if (m_bitmaps.empty()) {
        return;
    }

    decltype(m_bitmaps) M;
    M.reserve(m_bitmaps.size());
    m_nameToIndex.clear();

    for (const auto& b : m_bitmaps) {
        size_t index = b.first; // the key
        const auto& old_bmp_info = b.second;

        BmpInfo new_bmp_info;
        if (old_bmp_info.bmp_ptr) {
            // replace this entry
            new_bmp_info.name = old_bmp_info.name;
            new_bmp_info.bmp_ptr = const_cast<wxBitmap*>(&clBitmaps::Get().GetLoader()->LoadBitmap(old_bmp_info.name));
            if (new_bmp_info.bmp_ptr && new_bmp_info.bmp_ptr->IsOk()) {
                new_bmp_info.bmp_disabled = DrawingUtils::CreateDisabledBitmap(*new_bmp_info.bmp_ptr);
            }
        } else {
            new_bmp_info = old_bmp_info;
        }
        M.insert({ index, new_bmp_info });
        m_nameToIndex.insert({ new_bmp_info.name, index });
    }
    m_bitmaps.swap(M);
}

clBitmapList::clBitmapList()
{
    clear();
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, &clBitmapList::OnBitmapsUpdated, this);
}

clBitmapList::~clBitmapList()
{
    EventNotifier::Get()->Unbind(wxEVT_BITMAPS_UPDATED, &clBitmapList::OnBitmapsUpdated, this);
}

const wxBitmap& clBitmapList::Get(size_t index, bool disabledBmp)
{
    auto iter = m_bitmaps.find(index);
    if (iter == m_bitmaps.end()) {
        return wxNullBitmap;
    }

    if (disabledBmp) {
        // create the disabled bitmap upon request
        if (!iter->second.bmp_disabled.IsOk()) {
            iter->second.bmp_disabled = DrawingUtils::CreateDisabledBitmap(iter->second.bmp);
        }
        return iter->second.bmp_disabled;
    } else {
        if (iter->second.bmp_ptr) {
            return *(iter->second.bmp_ptr);
        } else {
            return iter->second.bmp;
        }
    }
}

void clBitmapList::clear()
{
    m_bitmaps.clear();
    m_nameToIndex.clear();
    m_index = 0;
}

void clBitmapList::Delete(size_t index)
{
    auto iter = m_bitmaps.find(index);
    if (iter == m_bitmaps.end()) {
        return;
    }

    iter->second.ref_count--;
    if (iter->second.ref_count <= 0) {
        // remove the entry from the name:index map
        auto iter2 = m_nameToIndex.find(iter->second.name);
        if (iter2 != m_nameToIndex.end()) {
            m_nameToIndex.erase(iter2);
        }
        m_bitmaps.erase(iter);
    }
}

void clBitmapList::Delete(const wxString& name) { Delete(FindIdByName(name)); }

const wxBitmap& clBitmapList::Get(const wxString& name, bool disabledBmp)
{
    return Get(FindIdByName(name), disabledBmp);
}

size_t clBitmapList::FindIdByName(const wxString& name) const
{
    auto iter = m_nameToIndex.find(name);
    if (iter == m_nameToIndex.end()) {
        return wxString::npos;
    }
    return iter->second;
}

size_t clBitmapList::Add(const wxString& bmp_name, int size)
{
    wxUnusedVar(size);
    const wxBitmap& bmp = clBitmaps::Get().GetLoader()->LoadBitmap(bmp_name);
    return DoAdd(bmp, wxNullBitmap, bmp_name, false);
}

size_t clBitmapList::Add(const wxBitmap& bmp, const wxString& name)
{
    // user bitmap
    return DoAdd(bmp, wxNullBitmap, name, true);
}

size_t clBitmapList::DoAdd(const wxBitmap& bmp, const wxBitmap& bmpDisabled, const wxString& bmp_name, bool user_bmp)
{
    size_t index = FindIdByName(bmp_name);
    if (index != wxString::npos) {
        m_bitmaps[index].ref_count++;
        return index;
    }

    // new entry
    BmpInfo bi; // ref_count = 1
    bi.bmp_disabled = bmpDisabled;
    if (!user_bmp) {
        // keep pointer
        bi.bmp_ptr = const_cast<wxBitmap*>(&bmp);
        bi.name = bmp_name;
    } else {
        // user provided bitmap
        bi.bmp = bmp;
        bi.bmp_ptr = nullptr;
        bi.name = bmp_name;
    }
    size_t new_index = m_index;
    m_bitmaps.insert({ new_index, bi });
    m_nameToIndex.insert({ bi.name, new_index });
    m_index++;
    return new_index;
}

const wxString& clBitmapList::GetBitmapName(size_t index) const
{
    auto where = m_bitmaps.find(index);
    if (where == m_bitmaps.end()) {
        static wxString emptyString;
        return emptyString;
    }
    return where->second.name;
}

bool BitmapLoader::GetIconBundle(const wxString& name, wxIconBundle* bundle)
{
    LoadSVGFiles(clSystemSettings::IsDark());
    auto bundles = GetBundles(clSystemSettings::IsDark());
    if (bundles->count(name) == 0) {
        return false;
    }

    const auto& bmp_bundle = bundles->find(name)->second;
    std::array<int, 5> sizes = { 24, 32, 64, 128, 256 };
    for (int size : sizes) {
        size = wxTheApp->GetTopWindow()->FromDIP(size);
        wxIcon icn = bmp_bundle.GetIcon(wxSize(size, size));
        bundle->AddIcon(icn);
    }
    return true;
}

namespace
{
thread_local static std::unordered_map<wxString, wxBitmap> dark_sidebar_bitmaps;
thread_local static std::unordered_map<wxString, wxBitmap> light_sidebar_bitmaps;
} // namespace

void clClearSidebarBitmapCache()
{
    dark_sidebar_bitmaps.clear();
    light_sidebar_bitmaps.clear();
}

wxBitmap clLoadSidebarBitmap(const wxString& name, wxWindow* win)
{
    wxUnusedVar(win);

    std::unordered_map<wxString, wxBitmap>& cache =
        clSystemSettings::IsDark() ? dark_sidebar_bitmaps : light_sidebar_bitmaps;
    if (cache.count(name)) {
        return cache.find(name)->second;
    }

    wxFileName svg_path{ clStandardPaths::Get().GetDataDir(), wxEmptyString };
    svg_path.AppendDir("svgs");
    svg_path.AppendDir(clSystemSettings::IsDark() ? "dark-theme" : "light-theme");
    svg_path.SetFullName(name + ".svg");
    if (!svg_path.DirExists()) {
        clWARNING() << "Unable to locate:" << svg_path << ". broken installation?" << endl;
        return wxNullBitmap;
    }

#if defined(__WXMSW__)
    wxSize button_size{ 24, 24 };
#else
    wxSize button_size{ 32, 32 };
#endif

    auto bmpbundle = wxBitmapBundle::FromSVGFile(svg_path.GetFullPath(), button_size);
    if (!bmpbundle.IsOk()) {
        return wxNullBitmap;
    }

    auto bmp = bmpbundle.GetBitmapFor(win);
    cache.insert({ name, bmp });
    return bmp;
}
