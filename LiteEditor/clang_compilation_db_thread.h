#ifndef CLANGCOMPILATIONDBTHREAD_H
#define CLANGCOMPILATIONDBTHREAD_H

#if HAS_LIBCLANG

#include <wx/string.h>
#include <wx/thread.h>

class ClangCompilationDbThread : public wxThread
{
    wxString m_dbfile;
    
public:
    ClangCompilationDbThread(const wxString &filename);
    virtual ~ClangCompilationDbThread();

public:
    virtual void* Entry();
    void Start() {
        Create();
        Run();
    }
};
#endif // HAS_LIBCLANG

#endif // CLANGCOMPILATIONDBTHREAD_H
