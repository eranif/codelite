#ifndef VCIMPORTER_H
#define VCIMPORTER_H

#include "wx/string.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include "map"
#include "project_settings.h"
#include "project.h"

struct VcProjectData
{
	wxString name;
	wxString id;
	wxString filepath;
	wxArrayString deps;
};

class VcImporter {
	wxString m_fileName;
	bool m_isOk;
	wxFileInputStream *m_is;
	wxTextInputStream *m_tis;
	std::map<wxString, VcProjectData> m_projects;
	
public:
	VcImporter(const wxString &fileName);
	virtual ~VcImporter();
	bool Import(wxString &errMsg);

private:
	//read line, skip empty lines
	bool ReadLine(wxString &line);
	bool OnProject(const wxString &firstLine, wxString &errMsg);
	void RemoveGershaim(wxString &str);
	void CreateWorkspace();
	void CreateProjects();
	bool ConvertProject(VcProjectData &data);
	void AddConfiguration(ProjectSettingsPtr settings, wxXmlNode *config);
	void CreateFiles(wxXmlNode *parent, wxString vdPath, ProjectPtr proj);
	wxArrayString SplitString(const wxString &s);
};

#endif //VCIMPORTER_H

