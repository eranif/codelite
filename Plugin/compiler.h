//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : compiler.h
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
#ifndef COMPILER_H
#define COMPILER_H

#include "configuration_object.h"
#include "smart_ptr.h"
#include "codelite_exports.h"
#include <map>
#include <list>
#include <wx/arrstr.h>
#include <vector>

/**
 * \ingroup LiteEditor
 * This class represents a compiler entry in the configuration file
 *
 * \version 1.0
 * first version
 *
 * \date 05-25-2007
 *
 * \author Eran
 */
class WXDLLIMPEXP_SDK Compiler : public ConfObject
{
public:
    enum { eErrorPattern, eWarningPattern };

    enum CmpFileKind { CmpFileKindSource, CmpFileKindResource };

    struct CmpFileTypeInfo {
        wxString extension;
        wxString compilation_line;
        CmpFileKind kind;
    };

    struct CmpCmdLineOption {
        wxString name;
        wxString help;
    };
    typedef std::map<wxString, CmpCmdLineOption> CmpCmdLineOptions;

    struct CmpInfoPattern {
        wxString pattern;
        wxString lineNumberIndex;
        wxString fileNameIndex;
        wxString columnIndex;
    };

    enum eRegexType { kRegexVC = 0, kRegexGNU };
    typedef std::list<CmpInfoPattern> CmpListInfoPattern;

private:
    void
    AddPattern(int type, const wxString& pattern, int fileNameIndex, int lineNumberIndex, int colIndex = wxNOT_FOUND);
    void AddDefaultGnuComplierOptions();
    void AddDefaultGnuLinkerOptions();

protected:
    wxString m_name;
    std::map<wxString, wxString> m_switches;
    std::map<wxString, Compiler::CmpFileTypeInfo> m_fileTypes;
    CmpCmdLineOptions m_compilerOptions;
    CmpCmdLineOptions m_linkerOptions;
    wxString m_objectSuffix;
    wxString m_dependSuffix;
    wxString m_preprocessSuffix;
    CmpListInfoPattern m_errorPatterns;
    CmpListInfoPattern m_warningPatterns;
    std::map<wxString, wxString> m_tools;
    wxString m_globalIncludePath;
    wxString m_globalLibPath;
    wxString m_pathVariable;
    bool m_generateDependeciesFile;
    bool m_readObjectFilesFromList;
    bool m_objectNameIdenticalToFileName;
    wxString m_compilerFamily;
    bool m_isDefault;
    wxString m_installationPath;
    wxArrayString m_compilerBuiltinDefinitions;

private:
    wxString GetGCCVersion() const;
    wxString GetIncludePath(const wxString& pathSuffix) const;
    wxArrayString POSIXGetIncludePaths() const;

public:
    typedef std::map<wxString, wxString>::const_iterator ConstIterator;

    Compiler(wxXmlNode* node, Compiler::eRegexType regexType = Compiler::kRegexGNU);
    virtual ~Compiler();

    /**
     * @brief return the compiler default include paths
     */
    wxArrayString GetDefaultIncludePaths();

    /**
     * @brief return true if this compiler is compatible with GNU compilers
     */
    bool IsGnuCompatibleCompiler() const;

    wxXmlNode* ToXml() const;
    void SetTool(const wxString& toolname, const wxString& cmd);
    void SetSwitch(const wxString& switchName, const wxString& switchValue);

    void SetInstallationPath(const wxString& installationPath) { this->m_installationPath = installationPath; }
    const wxString& GetInstallationPath() const { return m_installationPath; }
    void SetCompilerFamily(const wxString& compilerFamily) { this->m_compilerFamily = compilerFamily; }
    void SetIsDefault(bool isDefault) { this->m_isDefault = isDefault; }
    const wxString& GetCompilerFamily() const { return m_compilerFamily; }
    bool IsDefault() const { return m_isDefault; }
    // iteration over switches
    Compiler::ConstIterator SwitchesBegin() const { return m_switches.begin(); }
    Compiler::ConstIterator SwitchesEnd() const { return m_switches.end(); }

    void AddCompilerOption(const wxString& name, const wxString& desc);
    void AddLinkerOption(const wxString& name, const wxString& desc);

    /**
     * @brief return list of builtin macros for this compiler instance
     * @return
     */
    const wxArrayString& GetBuiltinMacros();

    //---------------------------------------------------
    // setters/getters
    //---------------------------------------------------
    wxString GetTool(const wxString& name) const;
    wxString GetSwitch(const wxString& name) const;

    const wxString& GetObjectSuffix() const { return m_objectSuffix; }
    void SetObjectSuffix(const wxString& suffix) { m_objectSuffix = suffix; }
    const wxString& GetDependSuffix() const { return m_dependSuffix; }
    void SetDependSuffix(const wxString& suffix) { m_dependSuffix = suffix; }
    const wxString& GetPreprocessSuffix() const { return m_preprocessSuffix; }
    void SetPreprocessSuffix(const wxString& suffix) { m_preprocessSuffix = suffix; }

    void SetName(const wxString& name) { m_name = name; }
    const wxString& GetName() const { return m_name; }
    const CmpListInfoPattern& GetErrPatterns() const { return m_errorPatterns; }
    const CmpListInfoPattern& GetWarnPatterns() const { return m_warningPatterns; }

    void SetErrPatterns(const CmpListInfoPattern& p) { m_errorPatterns = p; }
    void SetWarnPatterns(const CmpListInfoPattern& p) { m_warningPatterns = p; }

    void SetGlobalIncludePath(const wxString& globalIncludePath) { this->m_globalIncludePath = globalIncludePath; }
    void SetGlobalLibPath(const wxString& globalLibPath) { this->m_globalLibPath = globalLibPath; }
    const wxString& GetGlobalIncludePath() const { return m_globalIncludePath; }
    const wxString& GetGlobalLibPath() const { return m_globalLibPath; }

    void SetPathVariable(const wxString& pathVariable) { this->m_pathVariable = pathVariable; }
    const wxString& GetPathVariable() const { return m_pathVariable; }

    void SetFileTypes(const std::map<wxString, Compiler::CmpFileTypeInfo>& fileTypes)
    {
        m_fileTypes.clear();
        this->m_fileTypes = fileTypes;
    }

    const std::map<wxString, Compiler::CmpFileTypeInfo>& GetFileTypes() const { return m_fileTypes; }

    const CmpCmdLineOptions& GetCompilerOptions() const { return m_compilerOptions; }

    void SetCompilerOptions(const CmpCmdLineOptions& cmpOptions) { m_compilerOptions = cmpOptions; }

    const CmpCmdLineOptions& GetLinkerOptions() const { return m_linkerOptions; }

    void SetLinkerOptions(const CmpCmdLineOptions& cmpOptions) { m_linkerOptions = cmpOptions; }

    void SetGenerateDependeciesFile(const bool& generateDependeciesFile)
    {
        this->m_generateDependeciesFile = generateDependeciesFile;
    }
    const bool& GetGenerateDependeciesFile() const { return m_generateDependeciesFile; }
    void SetReadObjectFilesFromList(bool readObjectFilesFromList)
    {
        this->m_readObjectFilesFromList = readObjectFilesFromList;
    }
    bool GetReadObjectFilesFromList() const { return m_readObjectFilesFromList; }
    void AddCmpFileType(const wxString& extension, CmpFileKind type, const wxString& compile_line);
    bool GetCmpFileType(const wxString& extension, Compiler::CmpFileTypeInfo& ft);
    void SetObjectNameIdenticalToFileName(bool objectNameIdenticalToFileName)
    {
        this->m_objectNameIdenticalToFileName = objectNameIdenticalToFileName;
    }
    bool GetObjectNameIdenticalToFileName() const { return m_objectNameIdenticalToFileName; }
};

typedef SmartPtr<Compiler> CompilerPtr;
typedef std::vector<CompilerPtr> CompilerPtrVec_t;

#endif // COMPILER_H
