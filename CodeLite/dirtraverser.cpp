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
	m_excludeDirs.Add(wxT(".svn"));
	m_excludeDirs.Add(wxT(".cvs"));
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
