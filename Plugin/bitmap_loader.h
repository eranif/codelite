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

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "fileextmanager.h"
#include "wxStringHash.h"

#include <vector>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/imaglist.h>

using namespace std;

#ifndef __WXMSW__
namespace std
{
template <> struct hash<FileExtManager::FileType> {
    size_t operator()(const FileExtManager::FileType& t) const { return hash<int>{}((int)t); }
};
} // namespace std
#endif

class WXDLLIMPEXP_SDK clMimeBitmaps
{
    /// Maps between image-id : index in the list
    unordered_map<int, int> m_fileIndexMap;
    vector<wxBitmap> m_bitmaps;

public:
    clMimeBitmaps();
    ~clMimeBitmaps();

    /**
     * @brief return the bitmap index that matches a given file type
     */
    int GetIndex(int type) const;
    /**
     * @brief return the bitmap index that matches the given filename
     */
    int GetIndex(const wxString& filename) const;
    const wxBitmap& GetBitmap(int type) const;
    void AddBitmap(const wxBitmap& bitmap, int type);
    void Clear();
    bool IsEmpty() const { return m_bitmaps.empty(); }
    vector<wxBitmap>& GetBitmaps() { return m_bitmaps; }
    const vector<wxBitmap>& GetBitmaps() const { return m_bitmaps; }
};

class WXDLLIMPEXP_SDK clBitmaps;

class WXDLLIMPEXP_SDK clBitmapList : public wxEvtHandler
{
    size_t m_index = 0;
    struct BmpInfo {
        wxBitmap* bmp_ptr = nullptr;          // if this is set, it means that it is part of the BitmapLoader class
        wxBitmap bmp = wxNullBitmap;          // user provided bitmap
        wxBitmap bmp_disabled = wxNullBitmap; // this one is always a one that we provide
        wxString name;
        int ref_count = 1;
    };
    unordered_map<size_t, BmpInfo> m_bitmaps;
    unordered_map<wxString, size_t> m_nameToIndex;

protected:
    void OnBitmapsUpdated(clCommandEvent& event);
    size_t FindIdByName(const wxString& name) const;
    size_t DoAdd(const wxBitmap& bmp, const wxBitmap& bmpDisabled, const wxString& bmp_name, bool user_bmp);

public:
    clBitmapList();
    virtual ~clBitmapList();
    size_t Add(const wxBitmap& bmp, const wxString& name);
    size_t Add(const wxString& bmp_name, int size = wxNOT_FOUND);
    size_t size() const { return m_bitmaps.size(); }
    bool empty() const { return m_bitmaps.empty(); }
    const wxBitmap& Get(size_t index, bool disabledBmp);
    const wxBitmap& Get(const wxString& name, bool disabledBmp);
    void Delete(size_t index);
    void Delete(const wxString& name);
    void clear();
    /**
     * @brief return bitmap name for a given index
     */
    const wxString& GetBitmapName(size_t index) const;
};

class WXDLLIMPEXP_SDK BitmapLoader : public wxEvtHandler
{
    friend class clBitmaps;

public:
    typedef unordered_map<FileExtManager::FileType, wxBitmap> BitmapMap_t;
    typedef vector<wxBitmap> Vec_t;

    enum eBitmapId {
        kClass = 1000,
        kStruct,
        kNamespace,
        kTypedef,
        kMemberPrivate,
        kMemberProtected,
        kMemberPublic,
        kFunctionPrivate,
        kFunctionProtected,
        kFunctionPublic,
        kEnum,
        kCEnum,
        kEnumerator,
        kConstant,
        kMacro,
        kCxxKeyword,
        kClose,
        kSave,
        kSaveAll,
        kTable,
        kDatabase,
        kColumn,
        kFind,
        kAngleBrackets,
        kSort,
    };

protected:
    wxFileName m_zipPath;
    unordered_map<wxString, wxBitmap> m_toolbarsBitmaps;
    unordered_map<wxString, wxString> m_manifest;
    unordered_map<FileExtManager::FileType, int> m_fileIndexMap;
    bool m_bMapPopulated;
    size_t m_toolbarIconSize;
    clMimeBitmaps m_mimeBitmaps;

protected:
    wxIcon GetIcon(const wxBitmap& bmp) const;

private:
    BitmapLoader(bool darkTheme);
    virtual ~BitmapLoader();

public:
    clMimeBitmaps& GetMimeBitmaps() { return m_mimeBitmaps; }
    const clMimeBitmaps& GetMimeBitmaps() const { return m_mimeBitmaps; }

    /**
     * @brief prepare an image list allocated on the heap which is based on
     * the FileExtManager content. It is the CALLER responsibility for deleting the memory
     */
    BitmapLoader::Vec_t* GetStandardMimeBitmapListPtr() { return &GetMimeBitmaps().GetBitmaps(); }

    /**
     * @brief return the image associated with a filename
     */
    const wxBitmap& GetBitmapForFile(const wxFileName& filename) const
    {
        return GetBitmapForFile(filename.GetFullName());
    }
    const wxBitmap& GetBitmapForFile(const wxString& filename) const;

    /**
     * @brief return the image index in the image list prepared by GetStandardMimeBitmapListPtr()
     * @return wxNOT_FOUND if no match is found, the index otherwise
     */
    int GetMimeImageId(const wxString& filename);

    /**
     * @brief return the image index in the image list prepared by GetStandardMimeBitmapListPtr()
     * @return wxNOT_FOUND if no match is found, the index otherwise
     */
    int GetMimeImageId(int type);
    int GetImageIndex(int type) { return GetMimeImageId(type); }

protected:
    void CreateMimeList();

private:
    void Initialize(bool darkTheme);
    static void SetActiveBitmaps();
    void OnSysColoursChanged(clCommandEvent& event);

public:
    const wxBitmap& LoadBitmap(const wxString& name, int requestedSize = 16);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BITMAPS_UPDATED, clCommandEvent);
class WXDLLIMPEXP_SDK clBitmaps : public wxEvtHandler
{
    BitmapLoader* m_lightBitmaps = nullptr;
    BitmapLoader* m_darkBitmaps = nullptr;
    BitmapLoader* m_activeBitmaps = nullptr;

protected:
    void Initialise();
    void OnSysColoursChanged(clCommandEvent& event);

protected:
    clBitmaps();
    virtual ~clBitmaps();

public:
    static clBitmaps& Get();
    BitmapLoader* GetLoader();
    void SysColoursChanged();
};
#endif // BITMAP_LOADER_H
