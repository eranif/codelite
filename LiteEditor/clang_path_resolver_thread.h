#ifndef CLANGPATHRESOLVERTHREAD_H
#define CLANGPATHRESOLVERTHREAD_H

#if HAS_LIBCLANG

#include "precompiled_header.h"
#include <wx/thread.h>
#include <set>

extern const wxEventType wxEVT_COMMAND_CLANG_PATH_RESOLVED;

class ClangPathResolverThread : public wxThread 
{
public:
	typedef std::set<wxString> Set_t;
	
protected:
	Set_t    m_workspaceFolders;
	wxString m_compilerOutput;
	Set_t*   m_compilerSearchPaths;
	Set_t*   m_compilerMacros;
	
public:
	ClangPathResolverThread(const Set_t& files, const wxString &compilerOutput);
	virtual ~ClangPathResolverThread();

public:
	virtual void* Entry();
	void Start();
};

#endif // #if HAS_LIBCLANG
#endif // CLANGPATHRESOLVERTHREAD_H
