#ifndef BUILDER_H
#define BUILDER_H

#include "wx/string.h"
#include "smart_ptr.h"
#include "wx/event.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

/**
 * \ingroup SDK
 * this class defines the interface of a build system
 *
 * \version 1.0
 * first version
 *
 * \date 05-19-2007
 *
 * \author Eran
 */
class WXDLLIMPEXP_LE_SDK Builder {
protected:
	wxString m_name;
	wxString m_buildTool;
	wxString m_buildToolOptions;
	wxString m_buildToolJobs;

	wxString GetBuildToolFromConfig() const;
	wxString GetBuildToolOptionsFromConfig() const;
	wxString GetBuildToolJobsFromConfig() const;

public:
	Builder(const wxString &name, const wxString &buildTool, const wxString &buildToolOptions) ;
	virtual ~Builder();

	void SetBuildTool(const wxString &buildTool) { m_buildTool = buildTool; }
	void SetBuildToolOptions(const wxString &buildToolOptions) { m_buildToolOptions = buildToolOptions; }
	void SetBuildToolJobs(const wxString &buildToolJobs) { m_buildToolJobs = buildToolJobs; }

	/**
	 * Normalize the configuration name, this is done by removing any trailing and leading 
	 * spaces from the string, and replacing any space character with underscore.
	 */
	static wxString NormalizeConfigName(const wxString &confgName);

	/**
	 * \return the builder name 
	 */
	const wxString &GetName() const { return m_name; }
	
	/**
	 * \return the build tool assoicated with this builder
	 */
	wxString GetBuildToolCommand(bool isCommandlineCommand) const;
	
	/**
	 * return the build tool name 
	 */
	wxString GetBuildToolName() const;

	/**
	 * return the build tool options
	 */
	wxString GetBuildToolOptions() const;
	
	/**
	 * return the build tool jobs
	 */ 
	wxString GetBuildToolJobs() const;

	/**
	 * Export the build system specific file (e.g. GNU makefile, Ant file etc) 
	 * to allow users to invoke them manualy from the command line
	 * \param project project to export. 
	 * \param errMsg output
	 * \return true on success, false otherwise.
	 */
	virtual bool Export(const wxString &project, bool isProjectOnly, wxString &errMsg) = 0;

	/** 
	 * Return the command that should be executed for performing the clean
	 * task
	 */
	virtual wxString GetCleanCommand(const wxString &project, bool &isCustom) = 0;

	/** 
	 * Return the command that should be executed for performing the build
	 * task for a given project
	 */
	virtual wxString GetBuildCommand(const wxString &project, bool &isCustom) = 0;

	//-----------------------------------------------------------------
	// Project Only API
	//-----------------------------------------------------------------
	/** 
	 * Return the command that should be executed for performing the clean
	 * task - for the project only (excluding dependencies)
	 */
	virtual wxString GetPOCleanCommand(const wxString &project, bool &isCustom) = 0;

	/** 
	 * Return the command that should be executed for performing the build
	 * task for a given project - for the project only (excluding dependencies)
	 */
	virtual wxString GetPOBuildCommand(const wxString &project, bool &isCustom) = 0;
	
	/**
	 * \brief create a command to execute for compiling single source file
	 * \param project
	 * \param fileName
	 * \param errMsg [output]
	 * \return the command
	 */
	virtual wxString GetSingleFileCmd(const wxString &project, const wxString &fileName, bool &isCustom, wxString &errMsg) = 0;
};

typedef SmartPtr<Builder> BuilderPtr;

#endif // BUILDER_H
