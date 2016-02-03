//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : bitmap_loader.h
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

#ifndef BITMAP_LOADER_H
#define BITMAP_LOADER_H

#include <wx/filename.h>
#include <wx/bitmap.h>
#include <wx/imaglist.h>
#include <map>
#include "fileextmanager.h"
#include "codelite_exports.h"
#include <vector>

class WXDLLIMPEXP_SDK BitmapLoader
{
public:
    typedef std::map<FileExtManager::FileType, wxBitmap> BitmapMap_t;

protected:
    wxFileName m_zipPath;
    static std::map<wxString, wxBitmap> m_toolbarsBitmaps;
    static std::map<wxString, wxString> m_manifest;
    std::map<FileExtManager::FileType, int> m_fileIndexMap;
    bool m_bMapPopulated;
    static BitmapMap_t m_userBitmaps;
    size_t m_toolbarIconSize;
    
protected:
    void AddImage(int index, FileExtManager::FileType type);
    wxIcon GetIcon(const wxBitmap& bmp) const;

private:
    BitmapLoader();
    ~BitmapLoader();
    
public:
    static BitmapLoader* Create() { return new BitmapLoader(); }
    
    /**
     * @brief register a user defined image to a given file type
     */
    static void RegisterImage(FileExtManager::FileType type, const wxBitmap& bmp);

    /**
     * @brief prepare an image list allocated on the heap which is based on
     * the FileExtManager content. It is the CALLER responsibility for deleting the memory
     */
    wxImageList* MakeStandardMimeImageList();
    BitmapMap_t MakeStandardMimeMap();

    /**
     * @brief return the image index in the image list prepared by MakeStandardMimeImageList()
     * @return wxNOT_FOUND if no match is found, the index otherwise
     */
    int GetMimeImageId(const wxString& filename);

    /**
     * @brief return the image index in the image list prepared by MakeStandardMimeImageList()
     * @return wxNOT_FOUND if no match is found, the index otherwise
     */
    int GetMimeImageId(FileExtManager::FileType type);

protected:
    void doLoadManifest();
    void doLoadBitmaps();
    wxBitmap doLoadBitmap(const wxString& filepath);
    
private:
    void initialize();
    
public:
    const wxBitmap& LoadBitmap(const wxString& name, int requestedSize = 16);
};

#endif // BITMAP_LOADER_H
