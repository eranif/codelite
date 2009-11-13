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
#include <map>
#include <list>

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

/**
 * \ingroup LiteEditor
 * This class represenets a compiler entry in the configuration file
 *
 * \version 1.0
 * first version
 *
 * \date 05-25-2007
 *
 * \author Eran
 */
class WXDLLIMPEXP_LE_SDK Compiler : public ConfObject
{
public:
	enum CmpFileKind {
		CmpFileKindSource,
		CmpFileKindResource
	};

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
	};
	typedef std::list<CmpInfoPattern> CmpListInfoPattern;
	

private:
	void AddCmpFileType(const wxString &extension, CmpFileKind type, const wxString &compile_line);

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

public:
	typedef std::map<wxString, wxString>::const_iterator ConstIterator;

	Compiler(wxXmlNode *node);
	virtual ~Compiler();

	wxXmlNode *ToXml() const;

	//iteration over switches
	Compiler::ConstIterator SwitchesBegin() const {
		return m_switches.begin();
	}
	Compiler::ConstIterator SwitchesEnd() const {
		return m_switches.end();
	}

	//---------------------------------------------------
	//setters/getters
	//---------------------------------------------------
	wxString GetTool(const wxString &name) const;
	void SetTool(const wxString &name, const wxString &tool) {
		m_tools[name] = tool;
	}

	wxString GetSwitch(const wxString &name) const;
	void SetSwitch(const wxString &name, const wxString &value) {
		m_switches[name] = value;
	}
	const wxString &GetObjectSuffix() const {
		return m_objectSuffix;
	}
	void SetObjectSuffix(const wxString &suffix) {
		m_objectSuffix = suffix;
	}
  	const wxString &GetDependSuffix() const {
		return m_dependSuffix;
	}
	void SetDependSuffix(const wxString &suffix) {
		m_dependSuffix = suffix;
	}
	const wxString &GetPreprocessSuffix() const {
		return m_preprocessSuffix;
	}
	void SetPreprocessSuffix(const wxString &suffix) {
		m_preprocessSuffix = suffix;
	}
    
	void SetName(const wxString &name) {
		m_name = name;
	}
	const wxString &GetName() const {
		return m_name;
	}
	const CmpListInfoPattern &GetErrPatterns() const {
		return m_errorPatterns;
	}
	const CmpListInfoPattern &GetWarnPatterns() const {
		return m_warningPatterns;
	}

	void SetErrPatterns(const CmpListInfoPattern &p) {
		m_errorPatterns = p;
	}
	void SetWarnPatterns(const CmpListInfoPattern &p) {
		m_warningPatterns = p;
	}

	void SetGlobalIncludePath(const wxString& globalIncludePath) {
		this->m_globalIncludePath = globalIncludePath;
	}
	void SetGlobalLibPath(const wxString& globalLibPath) {
		this->m_globalLibPath = globalLibPath;
	}
	const wxString& GetGlobalIncludePath() const {
		return m_globalIncludePath;
	}
	const wxString& GetGlobalLibPath() const {
		return m_globalLibPath;
	}

	void SetPathVariable(const wxString& pathVariable) {
		this->m_pathVariable = pathVariable;
	}
	const wxString& GetPathVariable() const {
		return m_pathVariable;
	}

	void SetFileTypes(const std::map<wxString, Compiler::CmpFileTypeInfo>& fileTypes) {
		m_fileTypes.clear();
		this->m_fileTypes = fileTypes;
	}

	const std::map<wxString, Compiler::CmpFileTypeInfo>& GetFileTypes() const {
		return m_fileTypes;
	}
	
	const CmpCmdLineOptions& GetCompilerOptions() const {
		return m_compilerOptions;
	}
	
	void SetCompilerOptions(const CmpCmdLineOptions& cmpOptions) {
		m_compilerOptions = cmpOptions;
	}

	const CmpCmdLineOptions& GetLinkerOptions() const {
		return m_linkerOptions;
	}
	
	void SetLinkerOptions(const CmpCmdLineOptions& cmpOptions) {
		m_linkerOptions = cmpOptions;
	}

	void SetGenerateDependeciesFile(const bool& generateDependeciesFile) {
		this->m_generateDependeciesFile = generateDependeciesFile;
	}
	const bool& GetGenerateDependeciesFile() const {
		return m_generateDependeciesFile;
	}
	bool GetCmpFileType(const wxString &extension, Compiler::CmpFileTypeInfo &ft);
};

typedef SmartPtr<Compiler> CompilerPtr;
#endif // COMPILER_H
