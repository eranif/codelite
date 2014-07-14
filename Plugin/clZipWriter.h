#ifndef CLZIPWRITER_H
#define CLZIPWRITER_H

#include "codelite_exports.h"
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <wx/stream.h>
#include <wx/filename.h>

class WXDLLIMPEXP_SDK clZipWriter
{
    wxFileName m_filename;
    wxZipOutputStream* m_zip;
    wxFileOutputStream* m_file;

public:
    clZipWriter(const wxFileName& zipfile);
    ~clZipWriter();

    /**
     * @brief add file to the zip
     */
    void Add(const wxFileName& file);
    
    /**
     * @brief add a directory to the archive
     * @param path directory path
     * @param pattern files to add mask
     */
    void AddDirectory(const wxString& path, const wxString &pattern);
    
    /**
     * @brief return the zip filename
     */
    const wxFileName& GetFilename() const { return m_filename; }
    
    /**
     * @brief close the zip
     */
    void Close();
};

#endif // CLZIPWRITER_H
