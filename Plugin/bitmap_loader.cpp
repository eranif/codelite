#include "bitmap_loader.h"

BitmapLoader::~BitmapLoader()
{
}

BitmapLoader::BitmapLoader(const wxString& zipName)
{
	m_zipPath = wxFileName(wxStandardPaths::Get().GetDataDir(), zipName);
	if(zipPath.FileExists()) {
		doLoadManifest();
	}
}

const wxBitmap& BitmapLoader::LoadBitmap(const wxString& name)
{
}

void BitmapLoader::doLoadManifest()
{
	if(ExtractFileFromZip(zipPath.GetFullPath(), wxT("manifest.ini"), wxStandardPaths::Get().GetTempDir())) {
		// we got the file extracted, read it
		wxFileName manifest(wxStandardPaths::Get().GetTempDir(), wxT("manifest.ini"));
		wxFFile fp(manifest.GetFullPath(), wxT("r"));
		if(fp.IsOpened()) {
			wxString content;
			fp.ReadAll(&content);
		}
	}
}
