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

std::unordered_map<wxString, wxBitmap> BitmapLoader::m_toolbarsBitmaps;
std::unordered_map<wxString, wxString> BitmapLoader::m_manifest;
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
    std::unordered_map<wxString, wxBitmap>::const_iterator iter = m_toolbarsBitmaps.find(newName);
    if(iter != m_toolbarsBitmaps.end()) {
        const wxBitmap& b = iter->second;
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
    std::unordered_map<wxString, wxString>::iterator iter = m_manifest.begin();
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
    std::unordered_map<FileExtManager::FileType, int>::const_iterator iter = m_fileIndexMap.find(type);
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
    std::unordered_map<FileExtManager::FileType, int>::const_iterator iter = m_fileIndexMap.find(type);
    if(iter == m_fileIndexMap.end()) {
        return wxNOT_FOUND;
    }
    return iter->second;
}

#define ADD_BITMAP_TO_VEC(vect, bmp_name, file_type)    \
    vect.push_back(LoadBitmap(bmp_name));               \
    AddImage(vect.size() - 1, file_type);

BitmapLoader::Vec_t BitmapLoader::MakeStandardMimeBitmapList()
{
    BitmapLoader::Vec_t V;
    m_fileIndexMap.clear();
    ADD_BITMAP_TO_VEC(V, "console", FileExtManager::TypeExe);
    ADD_BITMAP_TO_VEC(V, "mime-html", FileExtManager::TypeHtml);
    ADD_BITMAP_TO_VEC(V, "archive", FileExtManager::TypeArchive);
    ADD_BITMAP_TO_VEC(V, "mime-php", FileExtManager::TypePhp);
    ADD_BITMAP_TO_VEC(V, "dll", FileExtManager::TypeDll);
    ADD_BITMAP_TO_VEC(V, "blocks", FileExtManager::TypeFormbuilder);
    ADD_BITMAP_TO_VEC(V, "mime-txt", FileExtManager::TypeCodedesigner);
    ADD_BITMAP_TO_VEC(V, "mime-bmp", FileExtManager::TypeBmp);
    ADD_BITMAP_TO_VEC(V, "cog", FileExtManager::TypeMakefile);
    ADD_BITMAP_TO_VEC(V, "mime-c", FileExtManager::TypeSourceC);
    ADD_BITMAP_TO_VEC(V, "mime-cpp", FileExtManager::TypeSourceCpp);
    ADD_BITMAP_TO_VEC(V, "mime-h", FileExtManager::TypeHeader);
    ADD_BITMAP_TO_VEC(V, "mime-txt", FileExtManager::TypeText);
    ADD_BITMAP_TO_VEC(V, "execute", FileExtManager::TypeScript);
    ADD_BITMAP_TO_VEC(V, "mime-xml", FileExtManager::TypeXml);
    ADD_BITMAP_TO_VEC(V, "mime-txt", FileExtManager::TypeErd);
    ADD_BITMAP_TO_VEC(V, "mime-python", FileExtManager::TypePython);
    ADD_BITMAP_TO_VEC(V, "mime-css", FileExtManager::TypeCSS);
    ADD_BITMAP_TO_VEC(V, "mime-js", FileExtManager::TypeJS);
    ADD_BITMAP_TO_VEC(V, "cxx-workspace", FileExtManager::TypeWorkspace);
    ADD_BITMAP_TO_VEC(V, "php-workspace", FileExtManager::TypeWorkspacePHP);
    ADD_BITMAP_TO_VEC(V, "docker", FileExtManager::TypeWorkspaceDocker);
    ADD_BITMAP_TO_VEC(V, "nodejs-workspace", FileExtManager::TypeWorkspaceNodeJS);
    ADD_BITMAP_TO_VEC(V, "project", FileExtManager::TypeProject);
    ADD_BITMAP_TO_VEC(V, "project-opened", FileExtManager::TypeProjectExpanded);
    ADD_BITMAP_TO_VEC(V, "blocks", FileExtManager::TypeWxCrafter);
    ADD_BITMAP_TO_VEC(V, "mime-xml", FileExtManager::TypeXRC);
    ADD_BITMAP_TO_VEC(V, "tools", FileExtManager::TypeResource);
    ADD_BITMAP_TO_VEC(V, "mime-sql", FileExtManager::TypeSQL);
    ADD_BITMAP_TO_VEC(V, "folder-yellow", FileExtManager::TypeFolder);
    ADD_BITMAP_TO_VEC(V, "folder-yellow-opened", FileExtManager::TypeFolderExpanded);
    ADD_BITMAP_TO_VEC(V, "mime-as", FileExtManager::TypeAsm);
    ADD_BITMAP_TO_VEC(V, "cmake", FileExtManager::TypeCMake);
    ADD_BITMAP_TO_VEC(V, "qt", FileExtManager::TypeQMake);
    ADD_BITMAP_TO_VEC(V, "docker", FileExtManager::TypeDockerfile);
    ADD_BITMAP_TO_VEC(V, "yml", FileExtManager::TypeYAML);
    ADD_BITMAP_TO_VEC(V, "database", FileExtManager::TypeDatabase);
    ADD_BITMAP_TO_VEC(V, "mime-svg", FileExtManager::TypeSvg);
    
    std::for_each(m_userBitmaps.begin(), m_userBitmaps.end(),
                  [&](const std::pair<FileExtManager::FileType, wxBitmap>& p) {
                      V.push_back(p.second);
                      AddImage(V.size() - 1, p.first);
                  });
    return V;
}

#define ADD_BITMAP_TO_LIST(L, bmp_name, file_type)  \
    AddImage(imageList->Add(LoadBitmap(bmp_name)), file_type);
    
wxImageList* BitmapLoader::MakeStandardMimeImageList()
{
#if defined(__WXMSW__) || defined(__WXGTK__)
    wxImageList* imageList = new wxImageList(clGetScaledSize(16), clGetScaledSize(16));
#else
    wxImageList* imageList = new wxImageList(16, 16);
#endif
    m_fileIndexMap.clear();
    ADD_BITMAP_TO_LIST(imageList, "console", FileExtManager::TypeExe);
    ADD_BITMAP_TO_LIST(imageList, "mime-html", FileExtManager::TypeHtml);
    ADD_BITMAP_TO_LIST(imageList, "archive", FileExtManager::TypeArchive);
    ADD_BITMAP_TO_LIST(imageList, "mime-php", FileExtManager::TypePhp);
    ADD_BITMAP_TO_LIST(imageList, "dll", FileExtManager::TypeDll);
    ADD_BITMAP_TO_LIST(imageList, "blocks", FileExtManager::TypeFormbuilder);
    ADD_BITMAP_TO_LIST(imageList, "mime-txt", FileExtManager::TypeCodedesigner);
    ADD_BITMAP_TO_LIST(imageList, "mime-bmp", FileExtManager::TypeBmp);
    ADD_BITMAP_TO_LIST(imageList, "cog", FileExtManager::TypeMakefile);
    ADD_BITMAP_TO_LIST(imageList, "mime-c", FileExtManager::TypeSourceC);
    ADD_BITMAP_TO_LIST(imageList, "mime-cpp", FileExtManager::TypeSourceCpp);
    ADD_BITMAP_TO_LIST(imageList, "mime-h", FileExtManager::TypeHeader);
    ADD_BITMAP_TO_LIST(imageList, "mime-txt", FileExtManager::TypeText);
    ADD_BITMAP_TO_LIST(imageList, "execute", FileExtManager::TypeScript);
    ADD_BITMAP_TO_LIST(imageList, "mime-xml", FileExtManager::TypeXml);
    ADD_BITMAP_TO_LIST(imageList, "mime-txt", FileExtManager::TypeErd);
    ADD_BITMAP_TO_LIST(imageList, "mime-python", FileExtManager::TypePython);
    ADD_BITMAP_TO_LIST(imageList, "mime-css", FileExtManager::TypeCSS);
    ADD_BITMAP_TO_LIST(imageList, "mime-js", FileExtManager::TypeJS);
    ADD_BITMAP_TO_LIST(imageList, "cxx-workspace", FileExtManager::TypeWorkspace);
    ADD_BITMAP_TO_LIST(imageList, "php-workspace", FileExtManager::TypeWorkspacePHP);
    ADD_BITMAP_TO_LIST(imageList, "docker", FileExtManager::TypeWorkspaceDocker);
    ADD_BITMAP_TO_LIST(imageList, "nodejs-workspace", FileExtManager::TypeWorkspaceNodeJS);
    ADD_BITMAP_TO_LIST(imageList, "project", FileExtManager::TypeProject);
    ADD_BITMAP_TO_LIST(imageList, "project-opened", FileExtManager::TypeProjectExpanded);
    ADD_BITMAP_TO_LIST(imageList, "blocks", FileExtManager::TypeWxCrafter);
    ADD_BITMAP_TO_LIST(imageList, "mime-xml", FileExtManager::TypeXRC);
    ADD_BITMAP_TO_LIST(imageList, "tools", FileExtManager::TypeResource);
    ADD_BITMAP_TO_LIST(imageList, "mime-sql", FileExtManager::TypeSQL);
    ADD_BITMAP_TO_LIST(imageList, "folder-yellow", FileExtManager::TypeFolder);
    ADD_BITMAP_TO_LIST(imageList, "folder-yellow-opened", FileExtManager::TypeFolderExpanded);
    ADD_BITMAP_TO_LIST(imageList, "mime-as", FileExtManager::TypeAsm);
    ADD_BITMAP_TO_LIST(imageList, "cmake", FileExtManager::TypeCMake);
    ADD_BITMAP_TO_LIST(imageList, "qt", FileExtManager::TypeQMake);
    ADD_BITMAP_TO_LIST(imageList, "docker", FileExtManager::TypeDockerfile);
    ADD_BITMAP_TO_LIST(imageList, "yml", FileExtManager::TypeYAML);
    ADD_BITMAP_TO_LIST(imageList, "database", FileExtManager::TypeDatabase);
    ADD_BITMAP_TO_LIST(imageList, "mime-svg", FileExtManager::TypeSvg);

    std::for_each(m_userBitmaps.begin(), m_userBitmaps.end(),
                  [&](const std::pair<FileExtManager::FileType, wxBitmap>& p) {
                      AddImage(imageList->Add(GetIcon(p.second)), p.first);
                  });
    return imageList;
}

void BitmapLoader::AddImage(int index, FileExtManager::FileType type)
{
    std::unordered_map<FileExtManager::FileType, int>::iterator iter = m_fileIndexMap.find(type);
    if(iter != m_fileIndexMap.end()) m_fileIndexMap.erase(iter);
    m_fileIndexMap.insert(std::make_pair(type, index));
}

#define ADD_BITMAP_TO_MAP(M, bmp_name, file_type)  \
    M[file_type] = LoadBitmap(bmp_name);
    
BitmapLoader::BitmapMap_t BitmapLoader::MakeStandardMimeMap()
{
    BitmapLoader::BitmapMap_t images;
    ADD_BITMAP_TO_MAP(images, "console", FileExtManager::TypeExe);
    ADD_BITMAP_TO_MAP(images, "mime-html", FileExtManager::TypeHtml);
    ADD_BITMAP_TO_MAP(images, "archive", FileExtManager::TypeArchive);
    ADD_BITMAP_TO_MAP(images, "mime-php", FileExtManager::TypePhp);
    ADD_BITMAP_TO_MAP(images, "dll", FileExtManager::TypeDll);
    ADD_BITMAP_TO_MAP(images, "blocks", FileExtManager::TypeFormbuilder);
    ADD_BITMAP_TO_MAP(images, "mime-txt", FileExtManager::TypeCodedesigner);
    ADD_BITMAP_TO_MAP(images, "mime-bmp", FileExtManager::TypeBmp);
    ADD_BITMAP_TO_MAP(images, "cog", FileExtManager::TypeMakefile);
    ADD_BITMAP_TO_MAP(images, "mime-c", FileExtManager::TypeSourceC);
    ADD_BITMAP_TO_MAP(images, "mime-cpp", FileExtManager::TypeSourceCpp);
    ADD_BITMAP_TO_MAP(images, "mime-h", FileExtManager::TypeHeader);
    ADD_BITMAP_TO_MAP(images, "mime-txt", FileExtManager::TypeText);
    ADD_BITMAP_TO_MAP(images, "execute", FileExtManager::TypeScript);
    ADD_BITMAP_TO_MAP(images, "mime-xml", FileExtManager::TypeXml);
    ADD_BITMAP_TO_MAP(images, "mime-txt", FileExtManager::TypeErd);
    ADD_BITMAP_TO_MAP(images, "mime-python", FileExtManager::TypePython);
    ADD_BITMAP_TO_MAP(images, "mime-css", FileExtManager::TypeCSS);
    ADD_BITMAP_TO_MAP(images, "mime-js", FileExtManager::TypeJS);
    ADD_BITMAP_TO_MAP(images, "cxx-workspace", FileExtManager::TypeWorkspace);
    ADD_BITMAP_TO_MAP(images, "php-workspace", FileExtManager::TypeWorkspacePHP);
    ADD_BITMAP_TO_MAP(images, "docker", FileExtManager::TypeWorkspaceDocker);
    ADD_BITMAP_TO_MAP(images, "nodejs-workspace", FileExtManager::TypeWorkspaceNodeJS);
    ADD_BITMAP_TO_MAP(images, "project", FileExtManager::TypeProject);
    ADD_BITMAP_TO_MAP(images, "project-opened", FileExtManager::TypeProjectExpanded);
    ADD_BITMAP_TO_MAP(images, "blocks", FileExtManager::TypeWxCrafter);
    ADD_BITMAP_TO_MAP(images, "mime-xml", FileExtManager::TypeXRC);
    ADD_BITMAP_TO_MAP(images, "tools", FileExtManager::TypeResource);
    ADD_BITMAP_TO_MAP(images, "mime-sql", FileExtManager::TypeSQL);
    ADD_BITMAP_TO_MAP(images, "folder-yellow", FileExtManager::TypeFolder);
    ADD_BITMAP_TO_MAP(images, "folder-yellow-opened", FileExtManager::TypeFolderExpanded);
    ADD_BITMAP_TO_MAP(images, "mime-as", FileExtManager::TypeAsm);
    ADD_BITMAP_TO_MAP(images, "cmake", FileExtManager::TypeCMake);
    ADD_BITMAP_TO_MAP(images, "qt", FileExtManager::TypeQMake);
    ADD_BITMAP_TO_MAP(images, "docker", FileExtManager::TypeDockerfile);
    ADD_BITMAP_TO_MAP(images, "yml", FileExtManager::TypeYAML);
    ADD_BITMAP_TO_MAP(images, "database", FileExtManager::TypeDatabase);
    ADD_BITMAP_TO_MAP(images, "mime-svg", FileExtManager::TypeSvg);
    
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
            clBitmap bmp;
            if(bmp.LoadFile(pngFile.GetFullPath(), wxBITMAP_TYPE_PNG)) {
                clDEBUG1() << "Adding new image:" << pngFile.GetName() << clEndl;
                m_toolbarsBitmaps.insert(std::make_pair(pngFile.GetName(), bmp));
            }
        }

        // if(DrawingUtils::IsDark(DrawingUtils::GetMenuBarBgColour())) {
        //     std::unordered_map<wxString, wxBitmap> greyBitmaps;
        //     std::for_each(m_toolbarsBitmaps.begin(),
        //                   m_toolbarsBitmaps.end(),
        //                   [&](const std::unordered_map<wxString, wxBitmap>::value_type& vt) {
        //         wxBitmap bmp = DrawingUtils::CreateGrayBitmap(vt.second);
        //         wxString name = vt.first;
        //         greyBitmaps[name] = bmp;
        //     });
        //     m_toolbarsBitmaps.swap(greyBitmaps);
        // }
    }
}
