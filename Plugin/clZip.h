#ifndef CLZIP_H
#define CLZIP_H

#include "codelite_exports.h"
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <wx/stream.h>
#include <wx/filename.h>

class WXDLLIMPEXP_SDK clZip
{
    wxZipOutputStream *m_zip;
    wxFileOutputStream *m_file;
public:
    clZip();
    ~clZip();
    
    /**
     * @brief add file to the zip archive
     * @param file
     */
    void Add(const wxFileName& file);
    
    /**
     * @brief extract filename into 'path' (directory)
     * @param filename file name to extract. Wildcards ('*'/'?') can be used here
     * @param directory the target directory
     */
    void Extract(const wxString &filename, const wxString &directory);
    
    /**
     * @brief close the zip archive
     */
    void Close();
};

#endif // CLZIP_H
