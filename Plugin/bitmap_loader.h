#ifndef BITMAP_LOADER_H
#define BITMAP_LOADER_H

#include <wx/filename.h>
#include <wx/bitmap.h>

class BitmapLoader {
	wxFileName m_zipPath;

public:
	BitmapLoader(const wxString &zipName);
	~BitmapLoader();

protected:
	void doLoadManifest();
	const wxBitmap& LoadBitmap(const wxString &name);

};

#endif // BITMAP_LOADER_H
