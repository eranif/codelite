#ifndef BUILDER_GNUMAKE_H
#define BUILDER_GNUMAKE_H

#include "builder.h"
#include "workspace.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

class WXDLLIMPEXP_LE_SDK BuilderGnuMake : public Builder 
{
public:
	BuilderGnuMake();
	virtual ~BuilderGnuMake();

	/**
	 * Export the build system specific file (e.g. GNU makefile, Ant file etc) 
	 * to allow users to invoke them manualy from the command line
	 * \param project project to export. 
	 * \param errMsg output
	 * \return true on success, false otherwise.
	 */
	virtual bool Export(const wxString &project, bool isProjectOnly, wxString &errMsg);
	virtual wxString GetBuildCommand(const wxString &project, bool &isCustom);
	virtual wxString GetCleanCommand(const wxString &project, bool &isCustom);
	virtual wxString GetPOBuildCommand(const wxString &project, bool &isCustom);
	virtual wxString GetPOCleanCommand(const wxString &project, bool &isCustom);
	virtual wxString GetSingleFileCmd(const wxString &project, const wxString &fileName, wxString &errMsg);

private:
	void GenerateMakefile(ProjectPtr proj);
	void CreateConfigsVariables(BuildConfigPtr bldConf, wxString &text);
	void CreateMakeDirsTarget(BuildConfigPtr bldConf, const wxString &targetName, wxString &text);
	void CreateFileTargets(ProjectPtr proj, wxString &text);
	void CreateObjectList(ProjectPtr proj, wxString &text);
	void CreateTargets(const wxString &type, BuildConfigPtr bldConf, wxString &text);
	void CreatePreBuildEvents(BuildConfigPtr bldConf, wxString &text);
	void CreatePostBuildEvents(BuildConfigPtr bldConf, wxString &text);
	
	wxString GetCdCmd(const wxFileName &path1, const wxFileName &path2);

	wxString ParseIncludePath(const wxString &paths);
	wxString ParseLibPath(const wxString &paths);
	wxString ParseLibs(const wxString &libs);
	wxString ParsePreprocessor(const wxString &prep);
	bool HasPrebuildCommands(BuildConfigPtr bldConf) const;

};
#endif // BUILDER_GNUMAKE_H
