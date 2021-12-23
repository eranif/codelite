//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_config.h
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
#ifndef BUILD_CONFIGURATION_COMMON_H
#define BUILD_CONFIGURATION_COMMON_H

#include "codelite_exports.h"
#include "configuration_object.h"

#include <wx/arrstr.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK BuildConfigCommon : public ConfObject
{
    wxArrayString m_includePath;
    wxString m_compileOptions;
    wxString m_linkOptions;
    wxArrayString m_libs;
    wxArrayString m_libPath;
    wxArrayString m_preprocessor;
    wxString m_resCompileOptions;
    wxString m_resCmpIncludePath;
    wxString m_cCompileOptions;
    wxString m_assemblerOptions;
    wxString m_confType; // xml node name

public:
    BuildConfigCommon(wxXmlNode* node, wxString confType = wxT("Configuration"));
    virtual ~BuildConfigCommon();
    wxXmlNode* ToXml() const;
    BuildConfigCommon* Clone() const;

    //--------------------------------
    // Setters / Getters
    //--------------------------------

    wxString GetPreprocessor() const;
    void GetPreprocessor(wxArrayString& arr) { arr = m_preprocessor; }
    void SetPreprocessor(const wxString& prepr);

    const wxString& GetCompileOptions() const { return m_compileOptions; }
    void SetCompileOptions(const wxString& options) { m_compileOptions = options; }

    void SetAssemblerOptions(const wxString& assemblerOptions) { this->m_assemblerOptions = assemblerOptions; }
    const wxString& GetAssemblerOptions() const { return m_assemblerOptions; }
    void SetCCompileOptions(const wxString& cCompileOptions) { this->m_cCompileOptions = cCompileOptions; }
    const wxString& GetCCompileOptions() const { return m_cCompileOptions; }
    const wxString& GetLinkOptions() const { return m_linkOptions; }
    void SetLinkOptions(const wxString& options) { m_linkOptions = options; }

    wxString GetIncludePath() const;
    void SetIncludePath(const wxString& path);
    void SetIncludePath(const wxArrayString& paths) { m_includePath = paths; }

    wxString GetLibraries() const;
    void SetLibraries(const wxString& libs);
    void SetLibraries(const wxArrayString& libs) { m_libs = libs; }

    wxString GetLibPath() const;
    void SetLibPath(const wxString& path);
    void SetLibPath(const wxArrayString& libPaths) { m_libPath = libPaths; }

    const wxString& GetResCmpIncludePath() const { return m_resCmpIncludePath; }
    void SetResCmpIncludePath(const wxString& path) { m_resCmpIncludePath = path; }

    const wxString& GetResCompileOptions() const { return m_resCompileOptions; }
    void SetResCmpOptions(const wxString& options) { m_resCompileOptions = options; }
};

typedef SmartPtr<BuildConfigCommon> BuildConfigCommonPtr;

#endif // BUILD_CONFIGURATION_COMMON_H
