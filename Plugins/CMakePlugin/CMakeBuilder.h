#ifndef CMAKEBUILDER_H
#define CMAKEBUILDER_H

#include "builder/builder.h" // Base class: Builder

class CMakeBuilder : public Builder
{
public:
    OptimalBuildConfig GetOptimalBuildConfig(const wxString& projectType) const override;
    static wxString GetWorkspaceBuildFolder(bool wrapWithQuotes);
    static wxString GetProjectBuildFolder(const wxString& project, bool wrapWithQuotes);

private:
    wxString GetBuildToolCommand(const wxString& project, const wxString& confToBuild) const;

public:
    CMakeBuilder();
    ~CMakeBuilder() override = default;
    /**
     * Export the build system specific file (e.g. GNU makefile, Ant file etc)
     * to allow users to invoke them manually from the command line
     * \param project project to export.
     * \param errMsg output
     * \return true on success, false otherwise.
     */
    bool Export(const wxString& project,
                        const wxString& confToBuild,
                        const wxString& arguments,
                        bool isProjectOnly,
                        bool force,
                        wxString& errMsg) override;

    /**
     * Return the command that should be executed for performing the clean
     * task
     */
    wxString GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) override;

    /**
     * Return the command that should be executed for performing the build
     * task for a given project
     */
    wxString GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) override;

    //-----------------------------------------------------------------
    // Project Only API
    //-----------------------------------------------------------------
    /**
     * Return the command that should be executed for performing the clean
     * task - for the project only (excluding dependencies)
     */
    wxString GetPOCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) override;

    /**
     * Return the command that should be executed for performing the build
     * task for a given project - for the project only (excluding dependencies)
     */
    wxString GetPOBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) override;

    /**
     * \brief create a command to execute for compiling single source file
     * \param project
     * \param fileName
     * \param errMsg [output]
     * \return the command
     */
    wxString GetSingleFileCmd(const wxString& project,
                                      const wxString& confToBuild,
                                      const wxString& arguments,
                                      const wxString& fileName) override;

    /**
     * \brief create a command to execute for preprocessing single source file
     * \param project
     * \param fileName
     * \param errMsg [output]
     * \return the command
     */
    wxString GetPreprocessFileCmd(const wxString& project,
                                          const wxString& confToBuild,
                                          const wxString& arguments,
                                          const wxString& fileName,
                                          wxString& errMsg) override;

    /**
     * @brief return the 'rebuild' command
     * @param project
     * @param confToBuild
     * @return
     */
    wxString
    GetPORebuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) override;
};

#endif // CMAKEBUILDER_H
