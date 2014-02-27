#include "clang_compilation_db_thread.h"
#include "compilation_database.h"

wxCriticalSection ClangCompilationDbThread::m_cs;

ClangCompilationDbThread::ClangCompilationDbThread(const wxString &filename)
    : wxThread(wxTHREAD_DETACHED)
    , m_dbfile(filename.c_str())
{
}

ClangCompilationDbThread::~ClangCompilationDbThread()
{
}

void* ClangCompilationDbThread::Entry()
{
    // only one thread can work at a time
    wxCriticalSectionLocker locker( m_cs );
    
    CompilationDatabase cdb(m_dbfile);
    cdb.Initialize();
    return NULL;
}
