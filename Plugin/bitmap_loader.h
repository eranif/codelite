#ifndef BITMAP_LOADER_H
#define BITMAP_LOADER_H

#include <wx/filename.h>
#include <wx/bitmap.h>
#include <wx/imaglist.h>
#include <map>
#include "fileextmanager.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK BitmapLoader 
{
	wxFileName                              m_zipPath;
	static std::map<wxString, wxBitmap>     m_toolbarsBitmaps;
	static std::map<wxString, wxString>     m_manifest;
	std::map<FileExtManager::FileType, int> m_fileIndexMap;
	bool                                    m_bMapPopulated;
	
protected:
	void AddImage(int index, FileExtManager::FileType type);
	
public:
	BitmapLoader();
	~BitmapLoader();
	
	/**
	 * @brief prepare an image list allocated on the heap which is based on 
	 * the FileExtManager content. It is the CALLER responsibility for deleting the memory
	 */
	wxImageList* MakeStandardMimeImageList();
	/**
	 * @brief return the image index in the image list prepared by MakeStandardMimeImageList()
	 * @return wxNOT_FOUND if no match is found, the index otherwise
	 */
	int GetMimeImageId(const wxString &filename) const;
	
	/**
	 * @brief return the image index in the image list prepared by MakeStandardMimeImageList()
	 * @return wxNOT_FOUND if no match is found, the index otherwise
	 */
	int GetMimeImageId(FileExtManager::FileType type) const;
	
protected:
	void            doLoadManifest();
	void            doLoadBitmaps();
	wxBitmap        doLoadBitmap(const wxString &filepath);

public:
	const wxBitmap& LoadBitmap(const wxString &name);

};

#endif // BITMAP_LOADER_H
