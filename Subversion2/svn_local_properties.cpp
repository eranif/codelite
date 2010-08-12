#include "svn_local_properties.h"
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include <wx/filename.h>

wxString SubversionLocalProperties::BUG_TRACKER_URL     = wxT("bug_tracker_url");
wxString SubversionLocalProperties::BUG_TRACKER_MESSAGE = wxT("bug_tracker_message");

SubversionLocalProperties::SubversionLocalProperties(const wxString& url)
	: m_url(url)
{
}

SubversionLocalProperties::~SubversionLocalProperties()
{
}

wxString SubversionLocalProperties::ReadProperty(const wxString& propName)
{
	ReadProperties();

	// find the relevant group
	GroupTable::const_iterator iter = m_values.find(m_url);
	if(iter == m_values.end())
		return wxT("");

	SimpleTable::const_iterator it = iter->second.find(propName);
	if(it == iter->second.end())
		return wxT("");

	return it->second;
}

void SubversionLocalProperties::WriteProperty(const wxString& name, const wxString& val)
{
	ReadProperties();

	GroupTable::iterator iter = m_values.find(m_url);
	if(iter == m_values.end()) {
		SimpleTable tb;
		tb[name] = val;
		m_values[m_url] = tb;
	} else {
		m_values[m_url][name] = val;
	}

	// Update the properties
	WriteProperties();
}

wxString SubversionLocalProperties::GetConfigFile()
{
	// disable logging
	wxString configDir(wxStandardPaths::Get().GetUserDataDir());
	configDir << wxFileName::GetPathSeparator() << wxT("subversion");

	wxMkdir(configDir);
	configDir << wxFileName::GetPathSeparator() << wxT("codelite-properties.ini");
	return configDir;
}

void SubversionLocalProperties::ReadProperties()
{
	m_values.clear();
	wxString group;
	wxFFile  fp(GetConfigFile(), wxT("rb"));
	if(fp.IsOpened()) {
		wxString content;
		fp.ReadAll(&content);

		wxArrayString lines = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
		for(size_t i=0; i<lines.size(); i++) {
			wxString entry = lines[i];

			// remove the comment part
			entry = entry.BeforeFirst(wxT(';'));

			// trim the string
			entry.Trim().Trim(false);

			if(entry.IsEmpty())
				continue;

			if(entry.StartsWith(wxT("["))) {
				// found new group
				entry = entry.AfterFirst(wxT('['));
				group = entry.BeforeFirst(wxT(']'));
				group.Trim().Trim(false);
				continue;
			}

			wxString key   = entry.BeforeFirst(wxT('='));
			wxString value = entry.AfterFirst(wxT('='));

			key.Trim().Trim(false);
			value.Trim().Trim(false);

			if(group.IsEmpty()) {
				// we dont have group yet - discard this entry
				continue;
			}

			GroupTable::iterator iter = m_values.find(group);
			if(iter == m_values.end()){
				// create new table and the value
				SimpleTable tb;
				tb[key] = value;
				m_values[group] = tb;
			} else {
				m_values[group][key] = value;
			}
		}
	}
}

void SubversionLocalProperties::WriteProperties()
{
	wxFFile  fp(GetConfigFile(), wxT("wb"));
	if(fp.IsOpened()) {
		GroupTable::const_iterator iter = m_values.begin();
		for(; iter != m_values.end(); iter++) {
			SimpleTable tb          = iter->second;
			wxString    sectionName = iter->first;

			SimpleTable::const_iterator it = tb.begin();
			fp.Write(wxString::Format(wxT("[%s]\n"), sectionName.c_str()));

			for(; it != tb.end(); it++) {
				fp.Write(wxString::Format(wxT("%s=%s\n"), it->first.c_str(), it->second.c_str()));
			}
		}
	}
}
