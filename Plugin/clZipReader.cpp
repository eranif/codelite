#include "clZipReader.h"
#include "macros.h"

clZipReader::clZipReader(const wxFileName& zipfile)
    : m_file(new wxFileInputStream(zipfile.GetFullPath()))
    , m_zip(new wxZipInputStream(*m_file))
{
}

clZipReader::~clZipReader() { Close(); }

void clZipReader::Close()
{
    wxDELETE(m_file);
    wxDELETE(m_zip);
}

void clZipReader::Extract(const wxString& filename, const wxString& directory)
{
    wxZipEntry* entry(NULL);
    entry = m_zip->GetNextEntry();
    while(entry) {
        if(::wxMatchWild(filename, entry->GetName())) {
            // Incase the entry name has a directory prefix, remove it
            wxString fullpath;
            fullpath << directory << "/" << entry->GetName();
            
            // Change to posix style
            fullpath.Replace("\\", "/");
            // Remove any duplicate double slashes
            while(fullpath.Replace("//", "/")) {}
            
            if(entry->IsDir()) {
                // a folder
                wxFileName::Mkdir(fullpath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            } else {
                wxFileName outfile(fullpath);
                // ensure that the path to the file exists
                outfile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
                wxFFileOutputStream out(outfile.GetFullPath());
                if(out.IsOk()) {
                    m_zip->Read(out);
                    out.Close();
                }
            }
        }
        wxDELETE(entry);
        entry = m_zip->GetNextEntry();
    }
}
