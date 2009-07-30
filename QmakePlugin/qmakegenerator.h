#ifndef __qmakegenerator__
#define __qmakegenerator__

#include <wx/string.h>
#include "qmakeplugindata.h"
#include "project_settings.h"
#include "build_config.h"
#include "project.h"

class IManager;

class QMakeProFileGenerator {
	IManager*     m_manager;
	wxString      m_project;
	wxString      m_configuration;
	wxString      m_makefilePath;

	void     SetVariables   ( wxString &pro_file, BuildConfigPtr bldConf, ProjectSettingsPtr settings);
	void     SetFiles       ( wxString &pro_file, ProjectPtr proj);
	wxString prepareVariable( const wxString &variable );

public:
	QMakeProFileGenerator(IManager *manager, const wxString &project, const wxString &configuration);
	virtual ~QMakeProFileGenerator();

	/**
	 * @brief generate .pro file to build the project with its dependencies
	 * @param project project name
	 * @param configuration the selected configuration
	 * @return true on success
	 */
	bool Generate();

	/**
	 * @brief return the name of the generated pro file
	 */
	wxString GetProFileName();
};
#endif // __qmakegenerator__
