#include "clZip.h"
#include "macros.h"

clZip::clZip()
    : m_zip(NULL)
    , m_file(NULL)
{
}

clZip::~clZip()
{
    wxDELETE(m_zip);
    wxDELETE(m_file);
}

void clZip::Add(const wxFileName& file)
{
    CHECK_PTR_RET(m_zip);
    CHECK_PTR_RET(m_file);

    wxZipEntry* entry = new wxZipEntry(file.GetFullName());
    m_zip->PutNextEntry(entry);

    wxFileInputStream fis(file.GetFullPath());
    wxBufferedOutputStream bfs(*m_zip);
    fis.Read(bfs);
    bfs.Sync();
}

void clZip::Close()
{
    CHECK_PTR_RET(m_zip);
    CHECK_PTR_RET(m_file);
}

void clZip::Extract(const wxString& filename, const wxString& directory)
{
    CHECK_PTR_RET(m_zip);
    CHECK_PTR_RET(m_file);
}
