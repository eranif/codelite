#include "clang_cleaner_thread.h"
#include "file_logger.h"
#include <wx/dir.h>
#include <wx/filefn.h>
#if HAS_LIBCLANG

ClangCleanerThread::ClangCleanerThread()
    : wxThread(wxTHREAD_JOINABLE)
{
}

ClangCleanerThread::~ClangCleanerThread()
{
}

void* ClangCleanerThread::Entry()
{
    wxLogNull nl;
    while ( !TestDestroy() ) {
        
        // Clear all temporary files created for clang parsing
        // DeleteTemporaryFiles();

        // Clear stale .pch files from %TMP%
        // libclang does not clear properly any stale files under %TMP% (this is at least under Windows)
        DeleteStalePCHFiles();

        wxThread::This()->Sleep( 5000 );
    }
    
    // Before we exit, try to perform one last cleanup
    DeleteTemporaryFiles();
    DeleteStalePCHFiles();
    return NULL;
}

void ClangCleanerThread::AddFileName(const wxString& filename)
{
    wxCriticalSectionLocker locker( m_cs );
    if ( m_files.count(filename) == 0 ) {
        m_files.insert( filename );
    }
}

void ClangCleanerThread::DeleteTemporaryFiles()
{
    wxStringSet_t files;
    {
        wxCriticalSectionLocker locker( m_cs );
        files.swap( m_files );
        m_files.clear();
    }

    wxStringSet_t failedSet;
    wxStringSet_t::const_iterator iter = files.begin();
    while ( iter != files.end() ) {
        if ( wxFileName::Exists(*iter) && !::wxRemoveFile( *iter )  ) {
            failedSet.insert( *iter );
            CL_DEBUG("Failed to delete file %s", *iter);

        } else {
            CL_DEBUG("Successfully deleted file %s", *iter);
        }
        ++iter;
    }

    if ( !failedSet.empty() ) {
        wxCriticalSectionLocker locker( m_cs );

        // add the "failed" files to the set, we will try again later
        m_files.insert( failedSet.begin(), failedSet.end() );
    }
}

void ClangCleanerThread::DeleteStalePCHFiles()
{
    wxString tmpdir;
    if ( ::wxGetEnv("TMP", &tmpdir) ) {
        wxArrayString files;
        if ( wxDir::GetAllFiles(tmpdir, &files, "*.pch", wxDIR_FILES) ) {
            for(size_t i=0; i<files.GetCount(); ++i) {
                wxFileName fn( files.Item(i) );
                if ( fn.GetFullName().StartsWith("preamble") ) {
                    if( ::wxRemoveFile( files.Item(i) ) ) {
                        CL_DEBUG("Deleting stale file: %s", files.Item(i) );
                    }
                }
            }
        }
    }
}

#endif // HAS_LIBCLANG
