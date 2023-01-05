#include "file_logger.h"

#include <wx/settings.h>
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
#include "optionsconfig.h"

#include <algorithm>
#include <wx/dcscreen.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

BitmapLoader::~BitmapLoader() {}

BitmapLoader::BitmapLoader(bool darkTheme)
    : m_bMapPopulated(false)
{
    Initialize(darkTheme);
}

const wxBitmap& BitmapLoader::LoadBitmap(const wxString& name, int requestedSize)
{
    // try to load a new bitmap first
    wxString newName;
    newName << requestedSize << "-" << name.AfterLast('/');

    std::unordered_map<wxString, wxBitmap>::const_iterator iter = m_toolbarsBitmaps.find(newName);
    if(iter != m_toolbarsBitmaps.end()) {
        const wxBitmap& b = iter->second;
        return b;
    }
    return wxNullBitmap;
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

#define DARK_ICONS _("Dark Theme Icons Set")
#define LIGHT_ICONS _("Light Theme Icons Set")

void BitmapLoader::Initialize(bool darkTheme)
{
    wxString zipname;
    // Load the bitmaps based on the current theme background colour
    wxFileName fnLight(clStandardPaths::Get().GetDataDir(), "codelite-bitmaps-light.zip");
    wxFileName fnDark(clStandardPaths::Get().GetDataDir(), "codelite-bitmaps-dark.zip");
    wxFileName fnNewZip = darkTheme ? fnDark : fnLight;

    clDEBUG() << "Loading bitmap resources:" << fnLight << endl;
    clDEBUG() << "Loading bitmap resources:" << fnDark << endl;

#ifdef __WXOSX__
    if(fnNewZip.FileExists()) {
        clZipReader zip(fnNewZip);
        wxFileName tmpdir("/tmp", "");
        tmpdir.AppendDir("codelite-bitmaps");
        tmpdir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        zip.ExtractAll(tmpdir.GetPath());
        clFilesScanner scanner;
        std::vector<wxFileName> V;
        scanner.Scan(tmpdir.GetPath(), V, "*.png", wxEmptyString, wxEmptyString);
        for(const wxFileName& fn : V) {
            // the @2x are loaded on demand
            if(fn.GetFullName().Contains("@2x")) {
                continue;
            }
            wxString name = fn.GetName();
            wxBitmap bmp;
            if(bmp.LoadFile(fn.GetFullPath(), wxBITMAP_TYPE_PNG)) {
                m_toolbarsBitmaps.erase(name);
                m_toolbarsBitmaps.insert({ name, bmp });
            }
        }
        tmpdir.Rmdir(wxPATH_RMDIR_FULL);
    }
#else
    if(fnNewZip.FileExists()) {
        clZipReader zip(fnNewZip);
        // Extract all images into this memory
        std::unordered_map<wxString, clZipReader::Entry> buffers;
        zip.ExtractAll(buffers);

        std::function<bool(const wxString&, void**, size_t&)> fnGetHiResVersion = [&](const wxString& name,
                                                                                      void** ppData, size_t& nLen) {
            wxString key;
            key << name << ".png";
            if(buffers.count(key)) {
                *ppData = buffers[key].buffer;
                nLen = buffers[key].len;
                return true;
            }
            return false;
        };

        for(const auto& entry : buffers) {
            if(!entry.first.EndsWith(".png")) {
                continue;
            }

            wxString name = wxFileName(entry.first).GetName();
            clZipReader::Entry d = entry.second;
            if(d.len && d.buffer) {
                wxMemoryInputStream is(d.buffer, d.len);
                clBitmap bmp;
                if(bmp.LoadPNGFromMemory(name, is, fnGetHiResVersion)) {
                    LOG_IF_TRACE { clDEBUG1() << "Adding new image:" << name << endl; }
                    m_toolbarsBitmaps.erase(name);
                    m_toolbarsBitmaps.insert({ name, bmp });
                }
            }
        }

        // Free the memory
        for(const auto& entry : buffers) {
            if(entry.second.buffer && entry.second.len) {
                free(entry.second.buffer);
            }
        }
        buffers.clear();
    }
#endif
    // Create the mime-list
    CreateMimeList();
}

void BitmapLoader::CreateMimeList()
{
    const int bitmap_size = 16;
    if(m_mimeBitmaps.IsEmpty()) {
        m_mimeBitmaps.AddBitmap(LoadBitmap("console", bitmap_size), FileExtManager::TypeExe);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-html", bitmap_size), FileExtManager::TypeHtml);
        m_mimeBitmaps.AddBitmap(LoadBitmap("archive", bitmap_size), FileExtManager::TypeArchive);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-php", bitmap_size), FileExtManager::TypePhp);
        m_mimeBitmaps.AddBitmap(LoadBitmap("dll", bitmap_size), FileExtManager::TypeDll);
        m_mimeBitmaps.AddBitmap(LoadBitmap("blocks", bitmap_size), FileExtManager::TypeFormbuilder);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-txt", bitmap_size), FileExtManager::TypeCodedesigner);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-bmp", bitmap_size), FileExtManager::TypeBmp);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cog", bitmap_size), FileExtManager::TypeMakefile);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-c", bitmap_size), FileExtManager::TypeSourceC);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-cpp", bitmap_size), FileExtManager::TypeSourceCpp);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-h", bitmap_size), FileExtManager::TypeHeader);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-txt", bitmap_size), FileExtManager::TypeText);
        m_mimeBitmaps.AddBitmap(LoadBitmap("execute", bitmap_size), FileExtManager::TypeShellScript);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-xml", bitmap_size), FileExtManager::TypeXml);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-txt", bitmap_size), FileExtManager::TypeErd);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-python", bitmap_size), FileExtManager::TypePython);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-css", bitmap_size), FileExtManager::TypeCSS);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-js", bitmap_size), FileExtManager::TypeJS);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cxx-workspace", bitmap_size), FileExtManager::TypeWorkspace);
        m_mimeBitmaps.AddBitmap(LoadBitmap("php-workspace", bitmap_size), FileExtManager::TypeWorkspacePHP);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow", bitmap_size), FileExtManager::TypeWorkspaceFileSystem);
        m_mimeBitmaps.AddBitmap(LoadBitmap("docker", bitmap_size), FileExtManager::TypeWorkspaceDocker);
        m_mimeBitmaps.AddBitmap(LoadBitmap("nodejs-workspace", bitmap_size), FileExtManager::TypeWorkspaceNodeJS);
        m_mimeBitmaps.AddBitmap(LoadBitmap("project", bitmap_size), FileExtManager::TypeProject);
        m_mimeBitmaps.AddBitmap(LoadBitmap("project-opened", bitmap_size), FileExtManager::TypeProjectExpanded);
        m_mimeBitmaps.AddBitmap(LoadBitmap("blocks", bitmap_size), FileExtManager::TypeWxCrafter);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-xml", bitmap_size), FileExtManager::TypeXRC);
        m_mimeBitmaps.AddBitmap(LoadBitmap("tools", bitmap_size), FileExtManager::TypeResource);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-sql", bitmap_size), FileExtManager::TypeSQL);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow", bitmap_size), FileExtManager::TypeFolder);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow-opened", bitmap_size), FileExtManager::TypeFolderExpanded);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-as", bitmap_size), FileExtManager::TypeAsm);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cmake", bitmap_size), FileExtManager::TypeCMake);
        m_mimeBitmaps.AddBitmap(LoadBitmap("qt", bitmap_size), FileExtManager::TypeQMake);
        m_mimeBitmaps.AddBitmap(LoadBitmap("docker", bitmap_size), FileExtManager::TypeDockerfile);
        m_mimeBitmaps.AddBitmap(LoadBitmap("yml", bitmap_size), FileExtManager::TypeYAML);
        m_mimeBitmaps.AddBitmap(LoadBitmap("database", bitmap_size), FileExtManager::TypeDatabase);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-svg", bitmap_size), FileExtManager::TypeSvg);
        m_mimeBitmaps.AddBitmap(LoadBitmap("workspace-folder-yellow-opened", bitmap_size),
                                FileExtManager::TypeWorkspaceFolderExpanded);
        m_mimeBitmaps.AddBitmap(LoadBitmap("workspace-folder-yellow", bitmap_size),
                                FileExtManager::TypeWorkspaceFolder);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow-opened-symlink", bitmap_size),
                                FileExtManager::TypeFolderSymlinkExpanded);
        m_mimeBitmaps.AddBitmap(LoadBitmap("folder-yellow-symlink", bitmap_size), FileExtManager::TypeFolderSymlink);
        m_mimeBitmaps.AddBitmap(LoadBitmap("mime-txt-symlink", bitmap_size), FileExtManager::TypeFileSymlink);
        m_mimeBitmaps.AddBitmap(LoadBitmap("rust", bitmap_size), FileExtManager::TypeRust);
        m_mimeBitmaps.AddBitmap(LoadBitmap("diff", bitmap_size), FileExtManager::TypeDiff);
        m_mimeBitmaps.AddBitmap(LoadBitmap("patch", bitmap_size), FileExtManager::TypePatch);
        m_mimeBitmaps.AddBitmap(LoadBitmap("ruby", bitmap_size), FileExtManager::TypeRuby);
        m_mimeBitmaps.AddBitmap(LoadBitmap("json", bitmap_size), FileExtManager::TypeJSON);

        // Non mime bitmaps
        m_mimeBitmaps.AddBitmap(LoadBitmap("file_save", bitmap_size), kSave);
        m_mimeBitmaps.AddBitmap(LoadBitmap("file_save_all", bitmap_size), kSaveAll);
        m_mimeBitmaps.AddBitmap(LoadBitmap("file_close", bitmap_size), kClose);
        m_mimeBitmaps.AddBitmap(LoadBitmap("db-table", bitmap_size), kTable);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cscope", bitmap_size), kFind);
        m_mimeBitmaps.AddBitmap(LoadBitmap("database", bitmap_size), kDatabase);
        m_mimeBitmaps.AddBitmap(LoadBitmap("db-column", bitmap_size), kColumn);
        m_mimeBitmaps.AddBitmap(LoadBitmap("globals", bitmap_size), kAngleBrackets);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/namespace", bitmap_size), kNamespace);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/class", bitmap_size), kClass);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/struct", bitmap_size), kStruct);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/function_public", bitmap_size), kFunctionPublic);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/function_protected", bitmap_size), kFunctionProtected);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/function_private", bitmap_size), kFunctionPrivate);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/member_public", bitmap_size), kMemberPublic);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/member_protected", bitmap_size), kMemberProtected);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/member_private", bitmap_size), kMemberPrivate);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/typedef", bitmap_size), kTypedef);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/macro", bitmap_size), kMacro);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/enum", bitmap_size), kEnum);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/enum", bitmap_size), kCEnum);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/macro", bitmap_size), kConstant);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/enumerator", bitmap_size), kEnumerator);
        m_mimeBitmaps.AddBitmap(LoadBitmap("cc/16/cpp_keyword", bitmap_size), kCxxKeyword);
        m_mimeBitmaps.AddBitmap(LoadBitmap("sort", bitmap_size), kSort);
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
    if(m_fileIndexMap.count(type) == 0) {
        return wxNOT_FOUND;
    }
    int offset = 0;
    if(disabled) {
        offset += m_disabled_bitmaps.size();
    }

    int index = m_fileIndexMap.at(type);
    index += offset;
    if(index >= m_bitmaps.size()) {
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
    if(index == wxNOT_FOUND) {
        static wxBitmap emptyBitmap;
        return emptyBitmap;
    }
    if(disabled) {
        index += m_disabled_bitmaps.size();
        if(index >= m_bitmaps.size()) {
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
    if(!pBitmaps) {
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
    bool isDark = DrawingUtils::IsDark(clSystemSettings::GetDefaultPanelColour());
    m_activeBitmaps = isDark ? m_darkBitmaps : m_lightBitmaps;

    if(old_ptr != m_activeBitmaps) {
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
    if(m_bitmaps.empty()) {
        return;
    }

    decltype(m_bitmaps) M;
    M.reserve(m_bitmaps.size());
    m_nameToIndex.clear();

    for(const auto& b : m_bitmaps) {
        size_t index = b.first; // the key
        const auto& old_bmp_info = b.second;

        BmpInfo new_bmp_info;
        if(old_bmp_info.bmp_ptr) {
            // replace this entry
            new_bmp_info.name = old_bmp_info.name;
            new_bmp_info.bmp_ptr = const_cast<wxBitmap*>(&clBitmaps::Get().GetLoader()->LoadBitmap(old_bmp_info.name));
            if(new_bmp_info.bmp_ptr && new_bmp_info.bmp_ptr->IsOk()) {
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
    if(iter == m_bitmaps.end()) {
        return wxNullBitmap;
    }

    if(disabledBmp) {
        // create the disabled bitmap upon request
        if(!iter->second.bmp_disabled.IsOk()) {
            iter->second.bmp_disabled = DrawingUtils::CreateDisabledBitmap(iter->second.bmp);
        }
        return iter->second.bmp_disabled;
    } else {
        if(iter->second.bmp_ptr) {
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
    if(iter == m_bitmaps.end()) {
        return;
    }

    iter->second.ref_count--;
    if(iter->second.ref_count <= 0) {
        // remove the entry from the name:index map
        auto iter2 = m_nameToIndex.find(iter->second.name);
        if(iter2 != m_nameToIndex.end()) {
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
    if(iter == m_nameToIndex.end()) {
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
    if(index != wxString::npos) {
        m_bitmaps[index].ref_count++;
        return index;
    }

    // new entry
    BmpInfo bi; // ref_count = 1
    bi.bmp_disabled = bmpDisabled;
    if(!user_bmp) {
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
    if(where == m_bitmaps.end()) {
        static wxString emptyString;
        return emptyString;
    }
    return where->second.name;
}
