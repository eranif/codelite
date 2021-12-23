//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : builder.h
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
#ifndef BUILDER_H
#define BUILDER_H

#include "codelite_exports.h"
#include "smart_ptr.h"
#include "wx/event.h"
#include "wx/string.h"

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
class WXDLLIMPEXP_SDK Builder
{
protected:
    wxString m_name;
    bool m_isActive;

public:
    Builder(const wxString& name);
    virtual ~Builder();

    /**
     * Normalize the configuration name, this is done by removing any trailing and leading
     * spaces from the string, and replacing any space character with underscore.
     */
    static wxString NormalizeConfigName(const wxString& confgName);

    /**
     * \return the builder name
     */
    const wxString& GetName() const { return m_name; }

    // ================ API ==========================
    // The below API as default implementation, but can be
    // overridden in the derived class
    // ================ API ==========================

    /**
     * @brief set this builder as the active builder. It also makes sure that all other
     * builders are set as non-active
     */
    virtual void SetActive();

    /**
     * @brief return true if this builder is the active one
     * @return
     */
    virtual bool IsActive() const { return m_isActive; }

    // ================ API ==========================
    // The below API must be implemented by the
    // derived class
    // ================ API ==========================

    /**
     * Export the build system specific file (e.g. GNU makefile, Ant file etc)
     * to allow users to invoke them manualy from the command line
     * \param project project to export.
     * \param errMsg output
     * \return true on success, false otherwise.
     */
    virtual bool Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                        bool isProjectOnly, bool force, wxString& errMsg) = 0;

    /**
     * Return the command that should be executed for performing the clean
     * task
     */
    virtual wxString GetCleanCommand(const wxString& project, const wxString& confToBuild,
                                     const wxString& arguments) = 0;

    /**
     * Return the command that should be executed for performing the build
     * task for a given project
     */
    virtual wxString GetBuildCommand(const wxString& project, const wxString& confToBuild,
                                     const wxString& arguments) = 0;

    //-----------------------------------------------------------------
    // Project Only API
    //-----------------------------------------------------------------
    /**
     * Return the command that should be executed for performing the clean
     * task - for the project only (excluding dependencies)
     */
    virtual wxString GetPOCleanCommand(const wxString& project, const wxString& confToBuild,
                                       const wxString& arguments) = 0;

    /**
     * Return the command that should be executed for performing the build
     * task for a given project - for the project only (excluding dependencies)
     */
    virtual wxString GetPOBuildCommand(const wxString& project, const wxString& confToBuild,
                                       const wxString& arguments) = 0;

    /**
     * \brief create a command to execute for compiling single source file
     * \param project
     * \param fileName
     * \param errMsg [output]
     * \return the command
     */
    virtual wxString GetSingleFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                                      const wxString& fileName) = 0;

    /**
     * \brief create a command to execute for preprocessing single source file
     * \param project
     * \param fileName
     * \param errMsg [output]
     * \return the command
     */
    virtual wxString GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
                                          const wxString& arguments, const wxString& fileName, wxString& errMsg) = 0;

    /**
     * @brief return the 'rebuild' command
     * @param project
     * @param confToBuild
     * @return
     */
    virtual wxString GetPORebuildCommand(const wxString& project, const wxString& confToBuild,
                                         const wxString& arguments) = 0;

    /**
     * @brief the optimal build config for use with this builder
     */
    struct OptimalBuildConfig {
        wxString intermediateDirectory; ///< intermediate directory
        wxString outputFile;            ///< output file name e.g. lib$(ProjectName).a
        wxString command;               ///< program execution command
        wxString workingDirectory;      ///< program execution working directory
    };

    /**
     * @brief return the optimal build config for use with this builder
     * Use CodeLite macros to make it 'portable' and configuration aware
     * @param projectType executable, static library or dynamic library
     */
    virtual OptimalBuildConfig GetOptimalBuildConfig(const wxString& projectType) const;

    /**
     * @brief return the best file extension (depends on platform) for the project type
     * @param projectType executable, static library or dynamic library
     * @return .exe / .dll (Windows), .so (Unix), .dylib (macOS), .a / .lib (static library) or empty
     */
    wxString GetOutputFileSuffix(const wxString& projectType) const;

    /**
     * @brief return static library file extension for the targeted compiler
     * @return usually .a for GCC, .lib for MSVC
     */
    virtual wxString GetStaticLibSuffix() const { return ".a"; }
};

typedef SmartPtr<Builder> BuilderPtr;

#endif // BUILDER_H
