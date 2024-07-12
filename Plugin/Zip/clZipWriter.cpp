#include "clZipWriter.h"

#include <wx/dir.h>

clZipWriter::clZipWriter(const wxFileName& zipfile)
    : m_filename(zipfile)
{
    m_file = new wxFileOutputStream(zipfile.GetFullPath());
    m_zip = new wxZipOutputStream(*m_file);
}

clZipWriter::~clZipWriter() { Close(); }

void clZipWriter::Add(const wxFileName& file)
{
    if(!file.FileExists()) {
        return;
    }
    wxZipEntry* entry = new wxZipEntry(file.GetFullName());
    m_zip->PutNextEntry(entry);

    wxFileInputStream fis(file.GetFullPath());
    wxBufferedOutputStream bfs(*m_zip);
    fis.Read(bfs);
    bfs.Sync();
}

void clZipWriter::AddDirectory(const wxString& path, const wxString& pattern)
{
    wxArrayString files;
    wxDir::GetAllFiles(path, &files, pattern);

    for(size_t i = 0; i < files.GetCount(); ++i) {
        Add(wxFileName(files.Item(i)));
    }
}

void clZipWriter::Close()
{
    if(m_zip) {
        m_zip->Close();
    }
    wxDELETE(m_zip);
    wxDELETE(m_file);
}
