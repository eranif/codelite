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
        if ( ::wxMatchWild(filename, entry->GetName()) ) {
            wxFileName outfile(directory, entry->GetName());
            wxFFileOutputStream out(outfile.GetFullPath());
            m_zip->Read(out);
            out.Close();
        }
        wxDELETE(entry);
        entry = m_zip->GetNextEntry();
    }
}
