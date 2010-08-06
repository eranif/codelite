#ifndef BITMAP_LOADER_H
#define BITMAP_LOADER_H

#include <wx/filename.h>
#include <wx/bitmap.h>
#include <map>

class BitmapLoader {
	wxFileName m_zipPath;
	std::map<wxString, wxBitmap> m_toolbarsBitmaps;
	std::map<wxString, wxString> m_manifest;

public:
	BitmapLoader(const wxString &zipName);
	~BitmapLoader();

protected:
	void            doLoadManifest();
	void            doLoadBitmaps();
	wxBitmap        doLoadBitmap(const wxString &filepath);

public:
	const wxBitmap& LoadBitmap(const wxString &name);

};

#endif // BITMAP_LOADER_H
