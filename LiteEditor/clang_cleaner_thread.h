#ifndef CLANGCLEANERTHREAD_H
#define CLANGCLEANERTHREAD_H

#if HAS_LIBCLANG
#include <wx/thread.h> // Base class: wxThread
#include "macros.h"

class ClangCleanerThread : public wxThread
{
    wxCriticalSection m_cs;
    wxStringSet_t m_files;
    
protected:
    void DeleteTemporaryFiles();
    void DeleteStalePCHFiles();
    
public:
    ClangCleanerThread();
    virtual ~ClangCleanerThread();

public:
    virtual void* Entry();
    
    void Start() {
        Create();
        Run();
    }
    
    /**
     * @brief stop the working thread
     * this function shdould be called from the main thread only
     */
    void Stop() {
        if ( IsAlive() ) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);
            
        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
    
    /**
     * @brief add a file name to be cleared from the file system
     * @param filename
     */
    void AddFileName( const wxString &filename );
};

#endif // HAS_LIBCLANG
#endif // CLANGCLEANERTHREAD_H
