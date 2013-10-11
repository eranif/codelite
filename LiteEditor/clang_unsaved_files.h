//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clang_unsaved_files.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
