#include "bitmap_loader.h"
#include <wx/tokenzr.h>
#include <wx/stdpaths.h>
#include <wx/ffile.h>
#include "globals.h"

BitmapLoader::~BitmapLoader()
{
}

BitmapLoader::BitmapLoader(const wxString& zipName)
{
	m_zipPath = wxFileName(wxStandardPaths::Get().GetDataDir(), zipName);
	if(m_zipPath.FileExists()) {
		doLoadManifest();
		doLoadBitmaps();
	}
}

const wxBitmap& BitmapLoader::LoadBitmap(const wxString& name)
{
	std::map<wxString, wxBitmap>::const_iterator iter = m_toolbarsBitmaps.find(name);
	if(iter != m_toolbarsBitmaps.end())
		return iter->second;

	return wxNullBitmap;
}

void BitmapLoader::doLoadManifest()
{
	wxString targetFile;
	if(ExtractFileFromZip(m_zipPath.GetFullPath(), wxT("manifest.ini"), wxStandardPaths::Get().GetTempDir(), targetFile)) {
		// we got the file extracted, read it
		wxFileName manifest(targetFile);
		wxFFile fp(manifest.GetFullPath(), wxT("r"));
		if(fp.IsOpened())  {

			wxString content;
			fp.ReadAll(&content);

			m_manifest.clear();
			wxArrayString entries = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
			for(size_t i=0; i<entries.size(); i++) {
				wxString entry = entries[i];
				entry.Trim().Trim(false);
				if(entry.empty())
					continue;

				wxString key = entry.BeforeFirst(wxT('='));
				wxString val = entry.AfterFirst (wxT('='));
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
			wxRemoveFile( manifest.GetFullPath() );
		}
	}
}

wxBitmap BitmapLoader::doLoadBitmap(const wxString& filepath)
{
	wxString bitmapFile;
	if(ExtractFileFromZip(m_zipPath.GetFullPath(), filepath, wxStandardPaths::Get().GetTempDir(), bitmapFile)) {
		wxBitmap bmp;
		if(bmp.LoadFile(bitmapFile, wxBITMAP_TYPE_PNG))
			return bmp;
	}
	return wxNullBitmap;
}

void BitmapLoader::doLoadBitmaps()
{
	std::map<wxString, wxString>::iterator iter = m_manifest.begin();
	for(; iter != m_manifest.end(); iter++) {
		wxString key = iter->first;
		key = key.BeforeLast(wxT('/'));
		m_toolbarsBitmaps[iter->first] = doLoadBitmap( wxString::Format(wxT("%s/%s"), key.c_str(), iter->second.c_str()) );
	}
}
