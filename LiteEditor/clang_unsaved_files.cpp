#include "clang_unsaved_files.h"

#if HAS_LIBCLANG

ClangUnsavedFiles::ClangUnsavedFiles(const ClangThreadRequest::List_t& files)
	: m_files(files)
{
	m_unsavedFiles = new CXUnsavedFile[files.size()];
	ClangThreadRequest::List_t::const_iterator iter = files.begin();
	for(size_t i=0; iter != files.end(); ++iter, i++ ) {
		m_unsavedFiles[i].Contents = strdup( iter->second.mb_str(wxConvUTF8).data() );
		m_unsavedFiles[i].Filename = strdup( iter->first.mb_str(wxConvUTF8).data() );
		m_unsavedFiles[i].Length   = strlen(m_unsavedFiles[i].Contents);
	}
}

ClangUnsavedFiles::~ClangUnsavedFiles()
{
	for(size_t i=0; i<m_files.size(); i++) {
		free((void*)m_unsavedFiles[i].Contents);
		free((void*)m_unsavedFiles[i].Filename);
	}
	delete [] m_unsavedFiles;
}

#endif // #if HAS_LIBCLANG
