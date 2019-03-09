//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_settings_config.h
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
#ifndef BUILD_CONFIG_SETTINGS_H
#define BUILD_CONFIG_SETTINGS_H

#include <vector>
#include "wx/string.h"
#include "builder.h"
#include "codelite_exports.h"
#include "singleton.h"
#include "compiler.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"
#include "build_system.h"
#include "JSON.h"
#include "cl_config.h"
#include <map>
#include <wxStringHash.h>
#include <macros.h>

// Cookie class for the editor to provide reentrance operations
// on various methods (such as iteration)
class WXDLLIMPEXP_SDK BuildSettingsConfigCookie
{
public:
    wxXmlNode* child;
    wxXmlNode* parent;

public:
    BuildSettingsConfigCookie()
        : child(NULL)
        , parent(NULL)
    {
    }
    ~BuildSettingsConfigCookie() {}
};

/**
 * @class BuildSettingsConfig the build system configuration
 * this class represents the content of the build_settings.xml configuration
 * file
 */
class WXDLLIMPEXP_SDK BuildSettingsConfig
{
    wxXmlDocument* m_doc;
    wxFileName m_fileName;
    wxString m_version;
    std::unordered_map<wxString, CompilerPtr> m_compilers;

protected:
    wxXmlNode* GetCompilerNode(const wxString& name) const;
    void DoUpdateCompilers();
    bool SaveXmlFile();

public:
    BuildSettingsConfig();
    virtual ~BuildSettingsConfig();

    /**
     * @brief return a map for the available compilers and their global include paths
     */
    std::unordered_map<wxString, wxArrayString> GetCompilersGlobalPaths() const;

    /**
     * Load the configuration file
     * @param version XML version which to be loaded, any version different from this one, will cause
     * codelite to override the user version
     */
    bool Load(const wxString& version, const wxString& xmlFilePath = "");

    /**
     * @brief delete all compilers
     */
    void DeleteAllCompilers(bool notify = true);

    /**
     * @brief return list of all compiler names
     */
    wxArrayString GetAllCompilersNames() const;

    /**
     * @brief return vector with all compilers defined
     * @param family the compiler family. Leave empty to get list of
     * all compilers regardless their family
     */
    CompilerPtrVec_t GetAllCompilers(const wxString& family = "") const;

    /**
     * @brief replace the current compilers list with a new one
     */
    void SetCompilers(const std::vector<CompilerPtr>& compilers);

    /**
     * Set or update a given compiler using its name as the index
     */
    void SetCompiler(CompilerPtr cmp);

    /**
     * @brief return the default compiler for a given family
     */
    CompilerPtr GetDefaultCompiler(const wxString& compilerFamilty) const;
    /**
     * Find and return compiler by name
     */
    CompilerPtr GetCompiler(const wxString& name) const;

    /**
     * Returns the first compiler found.
     * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is
     * necessary for the library to make these functions reentrant
     * (i.e. allow more than one enumeration on one and the same object simultaneously).
     */
    CompilerPtr GetFirstCompiler(BuildSettingsConfigCookie& cookie);

    /**
     * Returns the next compiler.
     * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is
     * necessary for the library to make these functions reentrant
     * (i.e. allow more than one enumeration on one and the same object simultaneously).
     */
    CompilerPtr GetNextCompiler(BuildSettingsConfigCookie& cookie);

    /**
     * check whether a compiler with a given name already exist
     */
    bool IsCompilerExist(const wxString& name) const;

    /**
     * delete compiler
     */
    void DeleteCompiler(const wxString& name);

    /**
     * Add build system
     */
    void SetBuildSystem(BuilderConfigPtr bs);

    /**
     * get build system from configuration by name
     */
    BuilderConfigPtr GetBuilderConfig(const wxString& name);

    /**
     * @brief save builder configurtation to the XML file
     * @param builder
     */
    void SaveBuilderConfig(BuilderPtr builder);

    /*
     * get name of current selected build system from configuration
     */
    wxString GetSelectedBuildSystem();

    /**
     * @brief discard the current settings and reload the default build
     * settings
     */
    void RestoreDefaults();
};

class WXDLLIMPEXP_SDK BuildSettingsConfigST
{
public:
    static BuildSettingsConfig* Get();
    static void Free();
};

#endif // BUILD_CONFIG_SETTINGS_H
