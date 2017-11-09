//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : fileextmanager.h
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

#ifndef __fileextmanager__
#define __fileextmanager__

#include <wx/filename.h>
#include <wx/string.h>
#include <map>
#include "codelite_exports.h"
#include <wx/regex.h>
#include "smart_ptr.h"
#include <vector>
#include "wxStringHash.h"

class WXDLLIMPEXP_CL FileExtManager
{
public:
    enum FileType {
        TypeSourceC,
        TypeSourceCpp,
        TypeHeader,
        TypeResource,
        TypeYacc,
        TypeLex,
        TypeQtForm,
        TypeQtResource,
        TypeWorkspace,
        TypeProject,
        TypeFormbuilder,
        TypeCodedesigner,
        TypeErd,
        TypePhp,
        TypeXml,
        TypeCSS,
        TypeJS,
        TypePython,
        TypeExe,
        TypeHtml,
        TypeArchive,
        TypeDll,
        TypeBmp,
        TypeMakefile,
        TypeText,
        TypeScript,
        TypeWxCrafter,
        TypeXRC,
        TypeSQL,
        TypeFolder,
        TypeFolderExpanded, // For UI purposes only
        TypeProjectActive,  // For UI purposes only
        TypeWorkspacePHP,
        TypeWorkspaceNodeJS,
        TypeWorkspacePHPTags,
        TypeWorkspaceDatabase,
        TypeAsm,
        TypeJava,
        TypeQMake,
        TypeCMake,
        TypeOther = wxNOT_FOUND
    };

    struct Matcher {
        SmartPtr<wxRegEx> m_regex;
        wxString m_exactMatch;
        FileType m_fileType;

        Matcher(const wxString& pattern, FileType fileType, bool regex = true)
            : m_fileType(fileType)
        {
            if(regex) {
                m_regex = new wxRegEx(pattern, wxRE_ADVANCED | wxRE_ICASE);
            } else {
                m_exactMatch = pattern;
            }
        }

        bool Matches(const wxString& in) const
        {
            if(m_regex) {
                return m_regex->Matches(in);
            } else {
                return in.Find(m_exactMatch) != wxNOT_FOUND;
            }
        }
        typedef SmartPtr<Matcher> Ptr_t;
    };

private:
    static std::unordered_map<wxString, FileType> m_map;
    static std::vector<FileExtManager::Matcher::Ptr_t> m_matchers;

public:
    static FileType GetType(const wxString& filename, FileExtManager::FileType defaultType = FileExtManager::TypeOther);
    static void Init();

    /**
     * @brief return true if the file is a C/C++ file
     */
    static bool IsCxxFile(const wxString& filename);
    static bool IsCxxFile(const wxFileName& filename) { return IsCxxFile(filename.GetFullPath()); }

    /**
     * @brief return true if a file is a JavaScript file
     */
    static bool IsJavascriptFile(const wxString& filename);
    static bool IsJavascriptFile(const wxFileName& filename) { return IsJavascriptFile(filename.GetFullPath()); }

    /**
     * @param return true if a file is a PHP file
     */
    static bool IsPHPFile(const wxString& filename);
    static bool IsPHPFile(const wxFileName& filename) { return IsPHPFile(filename.GetFullPath()); }

    /**
     * @param return true if a file is a Java file
     */
    static bool IsJavaFile(const wxString& filename);
    static bool IsJavaFile(const wxFileName& filename) { return IsJavaFile(filename.GetFullPath()); }

    /**
     * @param return true if the file is of type 'type'
     */
    static bool IsFileType(const wxString& filename, FileExtManager::FileType type);
    static bool IsFileType(const wxFileName& filename, FileExtManager::FileType type)
    {
        return IsFileType(filename.GetFullPath(), type);
    }

    /**
     * @brief attempt to autodetect the file type by examining its content
     */
    static bool AutoDetectByContent(const wxString& filename, FileExtManager::FileType& fileType);

    /**
     * @brief return the file type only by checking its extension
     */
    static FileExtManager::FileType GetTypeFromExtension(const wxString& filename)
    {
        return GetTypeFromExtension(wxFileName(filename));
    }
    static FileExtManager::FileType GetTypeFromExtension(const wxFileName& filename);
};

#endif // __fileextmanager__
