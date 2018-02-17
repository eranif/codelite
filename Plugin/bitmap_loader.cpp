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
        return b;
    }

    iter = m_toolbarsBitmaps.find(name);
    if(iter != m_toolbarsBitmaps.end()) { return iter->second; }

    return wxNullBitmap;
}

void BitmapLoader::doLoadManifest()
{
    wxString targetFile;
    if(ExtractFileFromZip(m_zipPath.GetFullPath(), wxT("manifest.ini"), clStandardPaths::Get().GetUserDataDir(),
                          targetFile)) {
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
            clRemoveFile(manifest.GetFullPath());
        }
        clRemoveFile(targetFile);
    }
}

wxBitmap BitmapLoader::doLoadBitmap(const wxString& filepath)
{
    wxString bitmapFile;
    if(ExtractFileFromZip(m_zipPath.GetFullPath(), filepath, clStandardPaths::Get().GetUserDataDir(), bitmapFile)) {
        clBitmap bmp;
        if(bmp.LoadFile(bitmapFile, wxBITMAP_TYPE_PNG)) {
            clRemoveFile(bitmapFile);
            return bmp;
        }
        clRemoveFile(bitmapFile);
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
    if(iter == m_fileIndexMap.end()) { return wxNOT_FOUND; }
    return iter->second;
}

wxImageList* BitmapLoader::MakeStandardMimeImageList()
{
#if defined(__WXMSW__) || defined(__WXGTK__)
    wxImageList* imageList = new wxImageList(clGetScaledSize(16), clGetScaledSize(16));
#else
    wxImageList* imageList = new wxImageList(16, 16);
#endif
    m_fileIndexMap.clear();
    AddImage(imageList->Add(LoadBitmap(wxT("console"))), FileExtManager::TypeExe);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-html"))), FileExtManager::TypeHtml);
    AddImage(imageList->Add(LoadBitmap(wxT("archive"))), FileExtManager::TypeArchive);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-php"))), FileExtManager::TypePhp);
    AddImage(imageList->Add(LoadBitmap(wxT("dll"))), FileExtManager::TypeDll);
    AddImage(imageList->Add(LoadBitmap(wxT("blocks"))), FileExtManager::TypeFormbuilder);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-txt"))), FileExtManager::TypeCodedesigner);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-bmp"))), FileExtManager::TypeBmp);
    AddImage(imageList->Add(LoadBitmap(wxT("cog"))), FileExtManager::TypeMakefile);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-c"))), FileExtManager::TypeSourceC);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-cpp"))), FileExtManager::TypeSourceCpp);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-h"))), FileExtManager::TypeHeader);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-txt"))), FileExtManager::TypeText);
    AddImage(imageList->Add(LoadBitmap(wxT("execute"))), FileExtManager::TypeScript);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-xml"))), FileExtManager::TypeXml);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-txt"))), FileExtManager::TypeErd);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-python"))), FileExtManager::TypePython);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-css"))), FileExtManager::TypeCSS);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-js"))), FileExtManager::TypeJS);
    AddImage(imageList->Add(LoadBitmap(wxT("cxx-workspace"))), FileExtManager::TypeWorkspace);
    AddImage(imageList->Add(LoadBitmap(wxT("php-workspace"))), FileExtManager::TypeWorkspacePHP);
    AddImage(imageList->Add(LoadBitmap(wxT("nodejs-workspace"))), FileExtManager::TypeWorkspaceNodeJS);
    AddImage(imageList->Add(LoadBitmap(wxT("project"))), FileExtManager::TypeProject);
    AddImage(imageList->Add(LoadBitmap(wxT("blocks"))), FileExtManager::TypeWxCrafter);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-xml"))), FileExtManager::TypeXRC);
    AddImage(imageList->Add(LoadBitmap(wxT("tools"))), FileExtManager::TypeResource);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-sql"))), FileExtManager::TypeSQL);
    AddImage(imageList->Add(LoadBitmap(wxT("folder-yellow"))), FileExtManager::TypeFolder);
    AddImage(imageList->Add(LoadBitmap(wxT("folder-yellow-opened"))), FileExtManager::TypeFolderExpanded);
    AddImage(imageList->Add(LoadBitmap(wxT("mime-as"))), FileExtManager::TypeAsm);
    AddImage(imageList->Add(LoadBitmap(wxT("cmake"))), FileExtManager::TypeCMake);
    AddImage(imageList->Add(LoadBitmap(wxT("qt"))), FileExtManager::TypeQMake);

    std::for_each(m_userBitmaps.begin(), m_userBitmaps.end(),
                  [&](const std::pair<FileExtManager::FileType, wxBitmap>& p) {
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
    images[FileExtManager::TypeExe] = LoadBitmap(wxT("console"));
    images[FileExtManager::TypeHtml] = LoadBitmap(wxT("mime-html"));
    images[FileExtManager::TypeArchive] = LoadBitmap(wxT("archive"));
    images[FileExtManager::TypePhp] = LoadBitmap(wxT("mime-php"));
    images[FileExtManager::TypeDll] = LoadBitmap(wxT("dll"));
    images[FileExtManager::TypeFormbuilder] = LoadBitmap(wxT("blocks"));
    images[FileExtManager::TypeCodedesigner] = LoadBitmap(wxT("mime-txt"));
    images[FileExtManager::TypeBmp] = LoadBitmap(wxT("mime-bmp"));
    images[FileExtManager::TypeMakefile] = LoadBitmap(wxT("cog"));
    images[FileExtManager::TypeSourceC] = LoadBitmap(wxT("mime-c"));
    images[FileExtManager::TypeSourceCpp] = LoadBitmap(wxT("mime-cpp"));
    images[FileExtManager::TypeHeader] = LoadBitmap(wxT("mime-h"));
    images[FileExtManager::TypeText] = LoadBitmap(wxT("mime-txt"));
    images[FileExtManager::TypeScript] = LoadBitmap(wxT("execute"));
    images[FileExtManager::TypeXml] = LoadBitmap(wxT("mime-xml"));
    images[FileExtManager::TypeErd] = LoadBitmap(wxT("mime-txt"));
    images[FileExtManager::TypePython] = LoadBitmap(wxT("mime-python"));
    images[FileExtManager::TypeCSS] = LoadBitmap(wxT("mime-css"));
    images[FileExtManager::TypeJS] = LoadBitmap(wxT("mime-js"));
    images[FileExtManager::TypeWorkspace] = LoadBitmap("cxx-workspace");
    images[FileExtManager::TypeWorkspacePHP] = LoadBitmap("php-workspace");
    images[FileExtManager::TypeWorkspaceNodeJS] = LoadBitmap("nodejs-workspace");
    images[FileExtManager::TypeProject] = LoadBitmap(wxT("project"));
    images[FileExtManager::TypeWxCrafter] = LoadBitmap(wxT("blocks"));
    images[FileExtManager::TypeXRC] = LoadBitmap(wxT("mime-xml"));
    images[FileExtManager::TypeResource] = LoadBitmap(wxT("tools"));
    images[FileExtManager::TypeSQL] = LoadBitmap(wxT("mime-sql"));
    images[FileExtManager::TypeFolder] = LoadBitmap("folder-yellow");
    images[FileExtManager::TypeFolderExpanded] = LoadBitmap("folder-yellow-opened");
    images[FileExtManager::TypeProjectActive] = LoadBitmap(wxT("project"));
    images[FileExtManager::TypeAsm] = LoadBitmap(wxT("mime-as"));
    images[FileExtManager::TypeCMake] = LoadBitmap(wxT("cmake"));
    images[FileExtManager::TypeQMake] = LoadBitmap(wxT("qt"));

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
    if(iter != m_userBitmaps.end()) { m_userBitmaps.erase(iter); }
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
    fn = wxFileName(clStandardPaths::Get().GetDataDir(), zipname);
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
        if(tmpFolder.DirExists()) { tmpFolder.Rmdir(wxPATH_RMDIR_RECURSIVE); }

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
            clBitmap bmp;
            if(bmp.LoadFile(pngFile.GetFullPath(), wxBITMAP_TYPE_PNG)) {
                clDEBUG1() << "Adding new image:" << pngFile.GetName() << clEndl;
                m_toolbarsBitmaps.insert(std::make_pair(pngFile.GetName(), bmp));
            }
        }

        // if(DrawingUtils::IsDark(DrawingUtils::GetMenuBarBgColour())) {
        //     std::map<wxString, wxBitmap> greyBitmaps;
        //     std::for_each(m_toolbarsBitmaps.begin(),
        //                   m_toolbarsBitmaps.end(),
        //                   [&](const std::map<wxString, wxBitmap>::value_type& vt) {
        //         wxBitmap bmp = DrawingUtils::CreateGrayBitmap(vt.second);
        //         wxString name = vt.first;
        //         greyBitmaps[name] = bmp;
        //     });
        //     m_toolbarsBitmaps.swap(greyBitmaps);
        // }
    }
}
