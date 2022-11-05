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

#ifndef __FILEEXTMANAGER__
#define __FILEEXTMANAGER__

#include "codelite_exports.h"
#include "smart_ptr.h"
#include "wxStringHash.h"

#include <map>
#include <vector>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL FileExtManager
{
public:
    enum FileType {
        TypeOther = wxNOT_FOUND,
        TypeSourceC, // 0
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
        TypeSvg,
        TypeMakefile,
        TypeText,
        TypeShellScript,
        TypeWxCrafter,
        TypeXRC,
        TypeSQL,
        TypeFolder,
        TypeFolderExpanded,          // For UI purposes only
        TypeProjectActive,           // For UI purposes only
        TypeProjectExpanded,         // For UI purposes only
        TypeWorkspaceFolder,         // For UI purposes only
        TypeWorkspaceFolderExpanded, // For UI purposes only
        TypeWorkspacePHP,
        TypeWorkspaceFileSystem,
        TypeWorkspaceDocker,
        TypeWorkspaceNodeJS,
        TypeWorkspacePHPTags,
        TypeWorkspaceDatabase,
        TypeAsm,
        TypeJava,
        TypeQMake,
        TypeCMake,
        TypeDockerfile,
        TypeYAML,
        TypeDatabase,
        TypeFileSymlink,
        TypeFolderSymlink,
        TypeFolderSymlinkExpanded, // For UI purposes only
        TypeLua,
        TypeRust,
        TypeRuby,
        TypeDiff,
        TypePatch,
        TypeJSON,
        TypeMarkdown,
        TypeDart,
        TypeLast,
    };

public:
    static FileType GetType(const wxString& filename, FileExtManager::FileType defaultType = FileExtManager::TypeOther);
    static void Init();

    static bool IsSymlinkFolder(const wxString& filename);
    static bool IsSymlinkFolder(const wxFileName& filename) { return IsSymlinkFolder(filename.GetFullPath()); }

    static bool IsSymlinkFile(const wxString& filename);
    static bool IsSymlinkFile(const wxFileName& filename) { return IsSymlinkFile(filename.GetFullPath()); }

    static bool IsSymlink(const wxString& filename) { return IsSymlinkFolder(filename) || IsSymlinkFile(filename); }
    static bool IsSymlink(const wxFileName& filename) { return IsSymlinkFolder(filename) || IsSymlinkFile(filename); }

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
     * @brief given input string, return the content type
     */
    static bool GetContentType(const wxString& string_content, FileExtManager::FileType& fileType);

    /**
     * @brief return the file type only by checking its extension
     */
    static FileExtManager::FileType GetTypeFromExtension(const wxString& filename)
    {
        return GetTypeFromExtension(wxFileName(filename));
    }
    static FileExtManager::FileType GetTypeFromExtension(const wxFileName& filename);

    /**
     * @brief return map of all supported file types
     * the returned map contains pairs of file extension -> FileType enumerator
     */
    static std::unordered_map<wxString, FileExtManager::FileType> GetAllSupportedFileTypes();
    /**
     * @brief return map of file types grouped by languages
     */
    static std::unordered_map<wxString, std::vector<FileExtManager::FileType>> GetLanguageBundles();

    static wxString GetLanguageFromType(FileExtManager::FileType file_type);
};

#endif // __fileextmanager__
