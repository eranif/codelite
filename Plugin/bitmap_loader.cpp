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
#include "clZipReader.h"
#include "cl_standard_paths.h"
#include "editor_config.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "optionsconfig.h"
#include <algorithm>
#include <wx/dcscreen.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include "clSystemSettings.h"
#include <wx/msgdlg.h>
#include "clFilesCollector.h"

std::unordered_map<wxString, wxBitmap> BitmapLoader::m_toolbarsBitmaps;
std::unordered_map<wxString, wxString> BitmapLoader::m_manifest;

BitmapLoader::~BitmapLoader() {}

BitmapLoader::BitmapLoader()
    : m_bMapPopulated(false)
{
    initialize();
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

int BitmapLoader::GetMimeImageId(int type) { return GetMimeBitmaps().GetIndex(type); }

int BitmapLoader::GetMimeImageId(const wxString& filename) { return GetMimeBitmaps().GetIndex(filename); }

wxIcon BitmapLoader::GetIcon(const wxBitmap& bmp) const
{
    wxIcon icn;
    icn.CopyFromBitmap(bmp);
    return icn;
}

#define DARK_ICONS _("Dark Theme Icons Set")
#define LIGHT_ICONS _("Light Theme Icons Set")

void BitmapLoader::initialize()
{
    wxString zipname;
    wxFileName fn;

// Under linux, take into account the --prefix
#ifdef __WXGTK__
    wxString bitmapPath = wxString(INSTALL_DIR, wxConvUTF8);
    fn = wxFileName(bitmapPath, zipname);
#else
    fn = wxFileName(clStandardPaths::Get().GetDataDir(), zipname);
#endif

    // Load the bitmaps based on the current theme background colour
    wxFileName fnNewZip(clStandardPaths::Get().GetDataDir(), "codelite-bitmaps-light.zip");
    if(DrawingUtils::IsDark(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE))) {
        fnNewZip.SetFullName("codelite-bitmaps-dark.zip");
    }
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
            if(fn.GetFullName().Contains("@2x")) { continue; }
            wxString name = fn.GetName();
            wxBitmap bmp;
            if(bmp.LoadFile(fn.GetFullPath(), wxBITMAP_TYPE_PNG)) {
                m_toolbarsBitmaps.erase(name);
                m_toolbarsBitmaps.insert({name, bmp});
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
            if(!entry.first.EndsWith(".png")) { continue; }

            wxString name = wxFileName(entry.first).GetName();
            clZipReader::Entry d = entry.second;
            if(d.len && d.buffer) {
                wxMemoryInputStream is(d.buffer, d.len);
                clBitmap bmp;
                if(bmp.LoadPNGFromMemory(name, is, fnGetHiResVersion)) {
                    clDEBUG1() << "Adding new image:" << name;
                    m_toolbarsBitmaps.erase(name);
                    m_toolbarsBitmaps.insert({ name, bmp });
                }
            }
        }

        // Free the memory
        for(const auto& entry : buffers) {
            if(entry.second.buffer && entry.second.len) { free(entry.second.buffer); }
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
        m_mimeBitmaps.AddBitmap(LoadBitmap("execute", bitmap_size), FileExtManager::TypeScript);
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
    }
}

const wxBitmap& BitmapLoader::GetBitmapForFile(const wxString& filename) const
{
    FileExtManager::FileType ft = FileExtManager::GetType(filename, FileExtManager::TypeText);
    return m_mimeBitmaps.GetBitmap(ft);
}

//===---------------------------
// clMimeBitmaps
//===---------------------------

clMimeBitmaps::clMimeBitmaps() {}

clMimeBitmaps::~clMimeBitmaps() {}

int clMimeBitmaps::GetIndex(int type) const
{
    if(m_fileIndexMap.count(type) == 0) { return wxNOT_FOUND; }
    return m_fileIndexMap.at(type);
}

int clMimeBitmaps::GetIndex(const wxString& filename) const
{
    FileExtManager::Init();
    int ft = FileExtManager::GetType(filename, FileExtManager::TypeText);
    return GetIndex(ft);
}

void clMimeBitmaps::AddBitmap(const wxBitmap& bitmap, int type)
{
    m_bitmaps.push_back(bitmap);
    int index = (m_bitmaps.size() - 1);
    m_fileIndexMap.insert({ type, index });
}

void clMimeBitmaps::Clear()
{
    m_bitmaps.clear();
    m_fileIndexMap.clear();
}

const wxBitmap& clMimeBitmaps::GetBitmap(int type) const
{
    int index = GetIndex(type);
    if(index == wxNOT_FOUND) {
        static wxBitmap emptyBitmap;
        return emptyBitmap;
    }
    return m_bitmaps.at(index);
}
