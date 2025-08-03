//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : qmakegenerator.h
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
    virtual ~QMakeProFileGenerator() = default;

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
