#ifndef CLANGUNSAVEDFILES_H
#define CLANGUNSAVEDFILES_H

#if HAS_LIBCLANG

#include <clang-c/Index.h>
#include "clang_pch_maker_thread.h"

class ClangUnsavedFiles
{
	ClangThreadRequest::List_t m_files;
	CXUnsavedFile*             m_unsavedFiles;
	
public:
	ClangUnsavedFiles(const ClangThreadRequest::List_t& files);
	~ClangUnsavedFiles();
	
	CXUnsavedFile* GetUnsavedFiles() const {
		return m_unsavedFiles;
	}
	
	int GetCount() const {
		return m_files.size();
	}
};

#endif // #if HAS_LIBCLANG
#endif // CLANGUNSAVEDFILES_H
