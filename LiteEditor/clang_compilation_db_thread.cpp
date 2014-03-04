#include "clang_compilation_db_thread.h"
#include "compilation_database.h"
#include "file_logger.h"

ClangCompilationDbThread::ClangCompilationDbThread()
    : wxThread(wxTHREAD_JOINABLE)
{
}

ClangCompilationDbThread::~ClangCompilationDbThread()
{
}

void* ClangCompilationDbThread::Entry()
{
    CL_DEBUG("ClangCompilationDbThread: Started");
    wxString filename;
    while ( !TestDestroy() ) {
        if ( m_queue.ReceiveTimeout( 50, filename ) == wxMSGQUEUE_NO_ERROR ) {

            // success
            // Process the file
            CL_DEBUG("ClangCompilationDbThread: Processing file " + filename);
            CompilationDatabase cdb(filename);
            cdb.Initialize();
            CL_DEBUG("ClangCompilationDbThread: Processing file " + filename + "... done");
            
        }
    }
    CL_DEBUG("ClangCompilationDbThread: Going down");
    return NULL;
}

void ClangCompilationDbThread::AddFile(const wxString& filename)
{
    m_queue.Post( filename );
}
