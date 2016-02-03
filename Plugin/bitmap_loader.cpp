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
#include <wx/tokenzr.h>
#include <wx/stdpaths.h>
#include <wx/ffile.h>
#include "globals.h"
#include "editor_config.h"
#include "optionsconfig.h"
#include "cl_standard_paths.h"
#include <algorithm>
#include "clZipReader.h"
#include <wx/dir.h>
#include "file_logger.h"

std::map<wxString, wxBitmap> BitmapLoader::m_toolbarsBitmaps;
std::map<wxString, wxString> BitmapLoader::m_manifest;
BitmapLoader::BitmapMap_t BitmapLoader::m_userBitmaps;

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
    std::map<wxString, wxBitmap>::const_iterator iter = m_toolbarsBitmaps.find(newName);
    if(iter != m_toolbarsBitmaps.end()) {
        const wxBitmap& b = iter->second;
        CL_DEBUG("Loaded HiRes image: %s", newName);
        CL_DEBUG("Image Size: (%d,%d)", b.GetWidth(), b.GetHeight());
        return b;
    }

    iter = m_toolbarsBitmaps.find(name);
    if(iter != m_toolbarsBitmaps.end()) {
        return iter->second;
    }

    return wxNullBitmap;
}

void BitmapLoader::doLoadManifest()
{
    wxString targetFile;
    if(ExtractFileFromZip(
           m_zipPath.GetFullPath(), wxT("manifest.ini"), clStandardPaths::Get().GetUserDataDir(), targetFile)) {
        // we got the file extracted, read it
        wxFileName manifest(targetFile);
        wxFFile fp(manifest.GetFullPath(), wxT("r"));
        if(fp.IsOpened()) {

            wxString content;
            fp.ReadAll(&content);

            m_manifest.clear();
            wxArrayString entries = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
            for(size_t i = 0; i < entries.size(); i++) {
                wxString entry = entries[i];
                entry.Trim().Trim(false);

                // empty?
                if(entry.empty()) continue;

                // comment?
                if(entry.StartsWith(wxT(";"))) continue;

                wxString key = entry.BeforeFirst(wxT('='));
                wxString val = entry.AfterFirst(wxT('='));
                key.Trim().Trim(false);
                val.Trim().Trim(false);

                wxString key16, key24;
                key16 = key;
                key24 = key;

                key16.Replace(wxT("<size>"), wxT("16"));
                key24.Replace(wxT("<size>"), wxT("24"));

                key16.Replace(wxT("."), wxT("/"));
                key24.Replace(wxT("."), wxT("/"));

                m_manifest[key16] = val;
                m_manifest[key24] = val;
            }

            fp.Close();
            wxRemoveFile(manifest.GetFullPath());
        }
        wxRemoveFile(targetFile);
    }
}

wxBitmap BitmapLoader::doLoadBitmap(const wxString& filepath)
{
    wxString bitmapFile;
    if(ExtractFileFromZip(m_zipPath.GetFullPath(), filepath, clStandardPaths::Get().GetUserDataDir(), bitmapFile)) {
        wxBitmap bmp;
        if(bmp.LoadFile(bitmapFile, wxBITMAP_TYPE_PNG)) {
            wxRemoveFile(bitmapFile);
            return bmp;
        }
        wxRemoveFile(bitmapFile);
    }
    return wxNullBitmap;
}

void BitmapLoader::doLoadBitmaps()
{
    std::map<wxString, wxString>::iterator iter = m_manifest.begin();
    for(; iter != m_manifest.end(); iter++) {
        wxString key = iter->first;
        key = key.BeforeLast(wxT('/'));
        m_toolbarsBitmaps[iter->first] =
            doLoadBitmap(wxString::Format(wxT("%s/%s"), key.c_str(), iter->second.c_str()));
    }
}

int BitmapLoader::GetMimeImageId(FileExtManager::FileType type)
{
    FileExtManager::Init();
    if(m_fileIndexMap.empty()) {
        // Allocate image list so we will populate the m_fileIndexMap
        wxImageList* il = MakeStandardMimeImageList();
        wxDELETE(il);
    }
    std::map<FileExtManager::FileType, int>::const_iterator iter = m_fileIndexMap.find(type);
    if(iter == m_fileIndexMap.end()) return wxNOT_FOUND;
    return iter->second;
}

int BitmapLoader::GetMimeImageId(const wxString& filename)
{
    FileExtManager::Init();
    if(m_fileIndexMap.empty()) {
        // Allocate image list so we will populate the m_fileIndexMap
        wxImageList* il = MakeStandardMimeImageList();
        wxDELETE(il);
    }

    FileExtManager::FileType type = FileExtManager::GetType(filename);
    std::map<FileExtManager::FileType, int>::const_iterator iter = m_fileIndexMap.find(type);
    if(iter == m_fileIndexMap.end()) {
        return wxNOT_FOUND;
    }
    return iter->second;
}

wxImageList* BitmapLoader::MakeStandardMimeImageList()
{
    wxImageList* imageList = new wxImageList(16, 16);

    m_fileIndexMap.clear();
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/exe"))), FileExtManager::TypeExe);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/html"))), FileExtManager::TypeHtml);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/zip"))), FileExtManager::TypeArchive);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/php"))), FileExtManager::TypePhp);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/dll"))), FileExtManager::TypeDll);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/wxfb"))), FileExtManager::TypeFormbuilder);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/cd"))), FileExtManager::TypeCodedesigner);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/bmp"))), FileExtManager::TypeBmp);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/makefile"))), FileExtManager::TypeMakefile);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/c"))), FileExtManager::TypeSourceC);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/cpp"))), FileExtManager::TypeSourceCpp);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/h"))), FileExtManager::TypeHeader);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/text"))), FileExtManager::TypeText);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/script"))), FileExtManager::TypeScript);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/xml"))), FileExtManager::TypeXml);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/erd"))), FileExtManager::TypeErd);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/python"))), FileExtManager::TypePython);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/css"))), FileExtManager::TypeCSS);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/javascript"))), FileExtManager::TypeJS);
    AddImage(imageList->Add(LoadBitmap(wxT("cxx-workspace"))), FileExtManager::TypeWorkspace);
    AddImage(imageList->Add(LoadBitmap(wxT("php-workspace"))), FileExtManager::TypeWorkspacePHP);
    AddImage(imageList->Add(LoadBitmap(wxT("nodejs-workspace"))), FileExtManager::TypeWorkspaceNodeJS);
    AddImage(imageList->Add(LoadBitmap(wxT("project"))), FileExtManager::TypeProject);
    AddImage(imageList->Add(LoadBitmap(wxT("blocks"))), FileExtManager::TypeWxCrafter);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/xml"))), FileExtManager::TypeXRC);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/res"))), FileExtManager::TypeResource);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/sql"))), FileExtManager::TypeSQL);
    AddImage(imageList->Add(LoadBitmap(wxT("folder-yellow"))), FileExtManager::TypeFolder);
    AddImage(imageList->Add(LoadBitmap(wxT("folder-yellow-opened"))), FileExtManager::TypeFolderExpanded);
    AddImage(imageList->Add(LoadBitmap(wxT("mime/16/asm"))), FileExtManager::TypeAsm);
    AddImage(imageList->Add(LoadBitmap(wxT("cmake"))), FileExtManager::TypeCMake);
    AddImage(imageList->Add(LoadBitmap(wxT("qt"))), FileExtManager::TypeQMake);

    std::for_each(
        m_userBitmaps.begin(), m_userBitmaps.end(), [&](const std::pair<FileExtManager::FileType, wxBitmap>& p) {
            AddImage(imageList->Add(GetIcon(p.second)), p.first);
        });
    return imageList;
}

void BitmapLoader::AddImage(int index, FileExtManager::FileType type)
{
    std::map<FileExtManager::FileType, int>::iterator iter = m_fileIndexMap.find(type);
    if(iter != m_fileIndexMap.end()) m_fileIndexMap.erase(iter);
    m_fileIndexMap.insert(std::make_pair(type, index));
}

BitmapLoader::BitmapMap_t BitmapLoader::MakeStandardMimeMap()
{
    BitmapLoader::BitmapMap_t images;
    images[FileExtManager::TypeExe] = LoadBitmap(wxT("mime/16/exe"));
    images[FileExtManager::TypeHtml] = LoadBitmap(wxT("mime/16/html"));
    images[FileExtManager::TypeArchive] = LoadBitmap(wxT("mime/16/zip"));
    images[FileExtManager::TypePhp] = LoadBitmap(wxT("mime/16/php"));
    images[FileExtManager::TypeDll] = LoadBitmap(wxT("mime/16/dll"));
    images[FileExtManager::TypeFormbuilder] = LoadBitmap(wxT("mime/16/wxfb"));
    images[FileExtManager::TypeCodedesigner] = LoadBitmap(wxT("mime/16/cd"));
    images[FileExtManager::TypeBmp] = LoadBitmap(wxT("mime/16/bmp"));
    images[FileExtManager::TypeMakefile] = LoadBitmap(wxT("mime/16/makefile"));
    images[FileExtManager::TypeSourceC] = LoadBitmap(wxT("mime/16/c"));
    images[FileExtManager::TypeSourceCpp] = LoadBitmap(wxT("mime/16/cpp"));
    images[FileExtManager::TypeHeader] = LoadBitmap(wxT("mime/16/h"));
    images[FileExtManager::TypeText] = LoadBitmap(wxT("mime/16/text"));
    images[FileExtManager::TypeScript] = LoadBitmap(wxT("mime/16/script"));
    images[FileExtManager::TypeXml] = LoadBitmap(wxT("mime/16/xml"));
    images[FileExtManager::TypeErd] = LoadBitmap(wxT("mime/16/erd"));
    images[FileExtManager::TypePython] = LoadBitmap(wxT("mime/16/python"));
    images[FileExtManager::TypeCSS] = LoadBitmap(wxT("mime/16/css"));
    images[FileExtManager::TypeJS] = LoadBitmap(wxT("mime/16/javascript"));
    images[FileExtManager::TypeWorkspace] = LoadBitmap("cxx-workspace");
    images[FileExtManager::TypeWorkspacePHP] = LoadBitmap("php-workspace");
    images[FileExtManager::TypeWorkspaceNodeJS] = LoadBitmap("nodejs-workspace");
    images[FileExtManager::TypeProject] = LoadBitmap(wxT("project"));
    images[FileExtManager::TypeWxCrafter] = LoadBitmap(wxT("mime/16/wxcp"));
    images[FileExtManager::TypeXRC] = LoadBitmap(wxT("mime/16/xml"));
    images[FileExtManager::TypeResource] = LoadBitmap(wxT("mime/16/res"));
    images[FileExtManager::TypeSQL] = LoadBitmap(wxT("mime/16/sql"));
    images[FileExtManager::TypeFolder] = LoadBitmap("folder-yellow");
    images[FileExtManager::TypeFolderExpanded] = LoadBitmap("folder-yellow-opened");
    images[FileExtManager::TypeProjectActive] = LoadBitmap(wxT("project"));
    images[FileExtManager::TypeAsm] = LoadBitmap(wxT("mime/16/asm"));
    images[FileExtManager::TypeCMake] = LoadBitmap(wxT("mime/16/cmake"));
    images[FileExtManager::TypeQMake] = LoadBitmap(wxT("mime/16/qmake"));

    BitmapLoader::BitmapMap_t merged;
    merged.insert(m_userBitmaps.begin(), m_userBitmaps.end());
    merged.insert(images.begin(), images.end());
    return merged;
}

wxIcon BitmapLoader::GetIcon(const wxBitmap& bmp) const
{
    wxIcon icn;
    icn.CopyFromBitmap(bmp);
    return icn;
}

void BitmapLoader::RegisterImage(FileExtManager::FileType type, const wxBitmap& bmp)
{
    BitmapMap_t::iterator iter = m_userBitmaps.find(type);
    if(iter != m_userBitmaps.end()) {
        m_userBitmaps.erase(iter);
    }
    m_userBitmaps.insert(std::make_pair(type, bmp));
}

void BitmapLoader::initialize()
{
    wxString zipname;
    wxFileName fn;
    zipname = "codelite-icons.zip";

    if(EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_IconSet_FreshFarm) {
        zipname = wxT("codelite-icons-fresh-farm.zip");

    } else if(EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_IconSet_Classic_Dark) {
        zipname = wxT("codelite-icons-dark.zip");

    } else if(EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_IconSet_Classic) {
        zipname = wxT("codelite-icons.zip");
    }

// Under linux, take into account the --prefix
#ifdef __WXGTK__
    wxString bitmapPath = wxString(INSTALL_DIR, wxConvUTF8);
    fn = wxFileName(bitmapPath, zipname);
#else
#ifdef USE_POSIX_LAYOUT
    wxString bitmapPath(clStandardPaths::Get().GetDataDir() + wxT(INSTALL_DIR));
    fn = wxFileName(bitmapPath, zipname);
#else
    fn = wxFileName(clStandardPaths::Get().GetDataDir(), zipname);
#endif
#endif

    if(m_manifest.empty() || m_toolbarsBitmaps.empty()) {
        m_zipPath = fn;
        if(m_zipPath.FileExists()) {
            doLoadManifest();
            doLoadBitmaps();
        }
    }

    wxFileName fnNewZip(clStandardPaths::Get().GetDataDir(), "codelite-bitmaps.zip");
    if(fnNewZip.FileExists()) {
        clZipReader zip(fnNewZip);
        wxFileName tmpFolder(clStandardPaths::Get().GetTempDir(), "");

        // Make sure we append the user name to the temporary user folder
        // this way, multiple CodeLite instances from different users can extract the
        // bitmaps to /tmp
        wxString bitmapFolder = "codelite-bitmaps";
        bitmapFolder << "." << clGetUserName();

        tmpFolder.AppendDir(bitmapFolder);
        if(tmpFolder.DirExists()) {
            tmpFolder.Rmdir(wxPATH_RMDIR_RECURSIVE);
        }

        tmpFolder.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        // Extract all images into this folder
        zip.Extract("*", tmpFolder.GetPath());

        // Load all the files into wxBitmap
        wxArrayString files;
        wxDir::GetAllFiles(tmpFolder.GetPath(), &files, "*.png");
        for(size_t i = 0; i < files.size(); ++i) {
            wxFileName pngFile(files.Item(i));
            if(pngFile.GetFullName().Contains("@2x")) {
                // No need to load the hi-res images manually,
                // this is done by wxWidgets
                continue;
            }
            wxBitmap bmp;
            if(bmp.LoadFile(pngFile.GetFullPath(), wxBITMAP_TYPE_PNG)) {
                CL_DEBUG("Adding new image: %s", pngFile.GetName());
                m_toolbarsBitmaps.insert(std::make_pair(pngFile.GetName(), bmp));
            }
        }
    }
}
