#include "clZipReader.h"
#include "macros.h"
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include "file_logger.h"
#include <cstdlib>
#include <wx/file.h>

clZipReader::clZipReader(const wxFileName& zipfile)
{
    // Read the entire content into memory
    wxFile fp(zipfile.GetFullPath(), wxFile::read);
    if(!fp.IsOpened()) {
        clERROR() << "Failed to open file:" << zipfile;
        return;
    }
    wxFileOffset size = fp.Length();

    // increase the buffer size to match the file size
    m_mb.SetBufSize(size);
    fp.Read(m_mb.GetWriteBuf(size), size);
    // update the data length
    m_mb.SetDataLen(size);
    fp.Close();
    m_file = new wxMemoryInputStream(m_mb.GetData(), m_mb.GetDataLen());
    m_zip = new wxZipInputStream(*m_file);
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
    if(!m_zip) { return; }
    wxZipEntry* entry(NULL);
    entry = m_zip->GetNextEntry();
    while(entry) {
        if(::wxMatchWild(filename, entry->GetName())) { DoExtractEntry(entry, directory); }
        wxDELETE(entry);
        entry = m_zip->GetNextEntry();
    }
}

void clZipReader::ExtractAll(const wxString& directory)
{
    if(!m_zip) { return; }

    wxZipEntry* entry(NULL);
    wxString basedir = directory;
    if(basedir.IsEmpty()) { basedir = "."; }

    entry = m_zip->GetNextEntry();
    while(entry) {
        DoExtractEntry(entry, directory);
        wxDELETE(entry);
        entry = m_zip->GetNextEntry();
    }
}

void clZipReader::DoExtractEntry(wxZipEntry* entry, const wxString& directory)
{
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
        wxFileOutputStream out(outfile.GetFullPath());
        if(out.IsOk()) {
            m_zip->Read(out);
            out.Close();
        }
    }
}

void clZipReader::ExtractAll(std::unordered_map<wxString, Entry>& buffers)
{
    // Incase the entry name has a directory prefix, remove it
    if(!m_zip) { return; }
    wxZipEntry* entry(NULL);

    entry = m_zip->GetNextEntry();
    while(entry) {
        if(!entry->IsDir()) {
            wxMemoryOutputStream out;
            if(out.IsOk()) {
                m_zip->Read(out);
                Entry e;
                e.len = out.GetLength();
                e.buffer = malloc(e.len);
                out.CopyTo(e.buffer, e.len);
                buffers.insert({ entry->GetName(), e });
            }
        }
        wxDELETE(entry);
        entry = m_zip->GetNextEntry();
    }
}
