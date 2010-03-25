#include "cl_registry.h"
#include <wx/tokenzr.h>
#include <map>

wxString clRegistry::m_filename;

clRegistry::clRegistry()
{
	// m_filename is set in the static method SetFilename()
	m_fp.Open(m_filename.c_str(), wxT("rb"));
	if(m_fp.IsOpened()) {
		wxString fileContent;
		m_fp.ReadAll(&fileContent, wxConvUTF8);

		wxArrayString entries = wxStringTokenize(fileContent, wxT("\n\r"), wxTOKEN_STRTOK);
		for(size_t i=0; i<entries.GetCount(); i++) {
			entries.Item(i).Trim().Trim(false);

			// Empty line?
			if(entries.Item(i).IsEmpty())
				continue;

			// Comment line?
			if(entries.Item(i).StartsWith(wxT(";"))) {
				continue;
			}

			if(entries.Item(i).StartsWith(wxT("["))) {
				// Section
				continue;
			}

			wxString key   = entries.Item(i).BeforeFirst(wxT('='));
			wxString value = entries.Item(i).AfterFirst(wxT('='));

			m_entries[key] = value;
		}
	}
}

clRegistry::~clRegistry()
{
}

bool clRegistry::Read(const wxString& key, wxString& val)
{
	std::map<wxString, wxString>::iterator iter = m_entries.find(key);
	if(iter == m_entries.end()) {
		return false;
	}
	val = iter->second;
	return true;
}

void clRegistry::SetFilename(const wxString& filename)
{
	m_filename = filename.c_str();
}

