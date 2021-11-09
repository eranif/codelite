#ifndef CMAKEBUILDER_H
#define CMAKEBUILDER_H

#include "builder.h" // Base class: Builder

class CMakeBuilder : public Builder
{
public:
    virtual OptimalBuildConfig GetOptimalBuildConfig(const wxString& projectType) const;
    static wxString GetWorkspaceBuildFolder(bool wrapWithQuotes);
    static wxString GetProjectBuildFolder(const wxString& project, bool wrapWithQuotes);

private:
    wxString GetBuildToolCommand(const wxString& project, const wxString& confToBuild) const;

public:
    CMakeBuilder();
    virtual ~CMakeBuilder();
    /**
     * Export the build system specific file (e.g. GNU makefile, Ant file etc)
     * to allow users to invoke them manualy from the command line
     * \param project project to export.
     * \param errMsg output
     * \return true on success, false otherwise.
     */
    virtual bool Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
        bool isProjectOnly, bool force, wxString& errMsg);

    /**
     * Return the command that should be executed for performing the clean
     * task
     */
    virtual wxString GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);

    /**
     * Return the command that should be executed for performing the build
     * task for a given project
     */
    virtual wxString GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);

    //-----------------------------------------------------------------
    // Project Only API
    //-----------------------------------------------------------------
    /**
     * Return the command that should be executed for performing the clean
     * task - for the project only (excluding dependencies)
     */
    virtual wxString GetPOCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);

    /**
     * Return the command that should be executed for performing the build
     * task for a given project - for the project only (excluding dependencies)
     */
    virtual wxString GetPOBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);

    /**
     * \brief create a command to execute for compiling single source file
     * \param project
     * \param fileName
     * \param errMsg [output]
     * \return the command
     */
    virtual wxString GetSingleFileCmd(
        const wxString& project, const wxString& confToBuild, const wxString& arguments, const wxString& fileName);

    /**
     * \brief create a command to execute for preprocessing single source file
     * \param project
     * \param fileName
     * \param errMsg [output]
     * \return the command
     */
    virtual wxString GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
        const wxString& arguments, const wxString& fileName, wxString& errMsg);

    /**
     * @brief return the 'rebuild' command
     * @param project
     * @param confToBuild
     * @return
     */
    virtual wxString GetPORebuildCommand(
        const wxString& project, const wxString& confToBuild, const wxString& arguments);
};

#endif // CMAKEBUILDER_H
