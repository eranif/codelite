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
    // destory them in reverse order of the creation
    wxDELETE(m_zip);
    wxDELETE(m_file);
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

void clZipReader::ExtractAll(const wxString& directory)
{
    wxZipEntry* entry(NULL);
    wxString basedir = directory;
    if(basedir.IsEmpty()) { basedir = "."; }

    entry = m_zip->GetNextEntry();
    while(entry) {
        // Prepend the basedir to the entry name
        wxString fullpath;
        fullpath << basedir << "/" << entry->GetName();

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
        wxDELETE(entry);
        entry = m_zip->GetNextEntry();
    }
}
