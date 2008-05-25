//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : dirtraverser.cpp              
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
 #include "dirtraverser.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"
#include "wx/log.h"

DirTraverser::DirTraverser(const wxString &filespec, bool includeExtLessFiles)
		: wxDirTraverser()
		, m_filespec(filespec)
		, m_extlessFiles(includeExtLessFiles)
{
	if (m_filespec.Trim() == wxT("*.*") || m_filespec.Trim() == wxT("*")) {
		m_specMap.clear();
	} else {
		wxStringTokenizer tok(m_filespec, wxT(";"));
		while ( tok.HasMoreTokens() ) {
			std::pair<wxString, bool> val;
			val.first = tok.GetNextToken().AfterLast(wxT('*'));
			val.first = val.first.AfterLast(wxT('.')).MakeLower();
			val.second = true;
			m_specMap.insert( val );
		}
	}
}

wxDirTraverseResult DirTraverser::OnFile(const wxString& filename)
{
	// add the file to our array
	wxFileName fn(filename);

	if ( m_specMap.empty() ) {
		m_files.Add(filename);
	} else if (fn.GetExt().IsEmpty() & m_extlessFiles) {
		m_files.Add(filename);
	} else if (m_specMap.find(fn.GetExt().MakeLower()) != m_specMap.end()) {
		m_files.Add(filename);
	}
	return wxDIR_CONTINUE;
}

wxDirTraverseResult DirTraverser::OnDir(const wxString &dirname)
{
	for (size_t i=0; i<m_excludeDirs.GetCount(); i++) {
		wxString tmpDir(dirname);
		tmpDir.Replace(wxT("\\"), wxT("/"));
		wxArrayString toks = wxStringTokenize(tmpDir, wxT("/"), wxTOKEN_STRTOK);
		wxString dir = m_excludeDirs.Item(i);
		wxString onlyDirName;
		
		if (toks.GetCount() > 0) {
			onlyDirName = toks.Last();
		}
		
		if (onlyDirName == dir) {
			return wxDIR_IGNORE;
		}
	}
	return wxDIR_CONTINUE;
}
