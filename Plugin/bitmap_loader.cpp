#include "bitmap_loader.h"
#include <wx/tokenzr.h>
#include <wx/stdpaths.h>
#include <wx/ffile.h>
#include "globals.h"
#include "editor_config.h"
#include "optionsconfig.h"

std::map<wxString, wxBitmap> BitmapLoader::m_toolbarsBitmaps;
std::map<wxString, wxString> BitmapLoader::m_manifest;

BitmapLoader::~BitmapLoader()
{
}

BitmapLoader::BitmapLoader()
	: m_bMapPopulated(false)
{
	wxString zipname;
	
	if(EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_IconSet_FreshFarm)
		zipname = wxT("codelite-icons-fresh-farm.zip");
	else
		zipname = wxT("codelite-icons.zip");
		
	if(m_manifest.empty() || m_toolbarsBitmaps.empty()) {
		m_zipPath = wxFileName(wxStandardPaths::Get().GetDataDir(), zipname);
		if(m_zipPath.FileExists()) {
			doLoadManifest();
			doLoadBitmaps();
		}
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
	if(ExtractFileFromZip(m_zipPath.GetFullPath(), wxT("manifest.ini"), wxStandardPaths::Get().GetUserDataDir(), targetFile)) {
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

				// empty?
				if(entry.empty())
					continue;

				// comment?
				if(entry.StartsWith(wxT(";")))
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
		wxRemoveFile( targetFile );
	}
}

wxBitmap BitmapLoader::doLoadBitmap(const wxString& filepath)
{
	wxString bitmapFile;
	if(ExtractFileFromZip(m_zipPath.GetFullPath(), filepath, wxStandardPaths::Get().GetUserDataDir(), bitmapFile)) {
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
		m_toolbarsBitmaps[iter->first] = doLoadBitmap( wxString::Format(wxT("%s/%s"), key.c_str(), iter->second.c_str()) );
	}
}

int BitmapLoader::GetMimeImageId(FileExtManager::FileType type) const
{
	FileExtManager::Init();
	std::map<FileExtManager::FileType , int>::const_iterator iter = m_fileIndexMap.find(type);
	if(iter == m_fileIndexMap.end())
		return wxNOT_FOUND;
	return iter->second;
}

int BitmapLoader::GetMimeImageId(const wxString& filename) const
{
	FileExtManager::Init();
	
	FileExtManager::FileType type = FileExtManager::GetType(filename);
	std::map<FileExtManager::FileType , int>::const_iterator iter = m_fileIndexMap.find(type);
	if(iter == m_fileIndexMap.end())
		return wxNOT_FOUND;

	return iter->second;
}

wxImageList* BitmapLoader::MakeStandardMimeImageList()
{
	wxImageList *imageList = new wxImageList(16, 16, true);

	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/exe" ) )),          FileExtManager::TypeExe);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/html" ) )),         FileExtManager::TypeHtml);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/zip" ) )),          FileExtManager::TypeArchive);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/php" ) ) ),         FileExtManager::TypePhp);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/dll") )),           FileExtManager::TypeDll);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/wxfb" ))),          FileExtManager::TypeFormbuilder);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/cd" ) )),           FileExtManager::TypeCodedesigner);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/bmp" ) ) ),         FileExtManager::TypeBmp);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/makefile"))),       FileExtManager::TypeMakefile);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/c"))),              FileExtManager::TypeSourceC);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/cpp"))),            FileExtManager::TypeSourceCpp);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/h"))),              FileExtManager::TypeHeader);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/text"))),           FileExtManager::TypeText);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/script"))),         FileExtManager::TypeScript);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/xml"))),            FileExtManager::TypeXml);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/erd"))),            FileExtManager::TypeErd);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/python"))),         FileExtManager::TypePython);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/css"))),            FileExtManager::TypeCSS);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/javascript"))),     FileExtManager::TypeJS);
	AddImage( imageList->Add( LoadBitmap(wxT("workspace/16/workspace"))), FileExtManager::TypeWorkspace);
	AddImage( imageList->Add( LoadBitmap(wxT("workspace/16/project"))),   FileExtManager::TypeProject);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/wxcp"))),           FileExtManager::TypeWxCrafter);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/xml"))),            FileExtManager::TypeXRC);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/res"))),            FileExtManager::TypeResource);
	AddImage( imageList->Add( LoadBitmap(wxT("mime/16/sql"))),            FileExtManager::TypeSQL);
	
	m_bMapPopulated = true;
	return imageList;
}

void BitmapLoader::AddImage(int index, FileExtManager::FileType type)
{
	if(!m_bMapPopulated) {
		m_fileIndexMap.insert(std::make_pair(type, index));
	}
}

