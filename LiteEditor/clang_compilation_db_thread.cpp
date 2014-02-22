#include "clang_compilation_db_thread.h"
#include "compilation_database.h"

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
    CompilationDatabase cdb(m_dbfile);
    cdb.Initialize();
    return NULL;
}
