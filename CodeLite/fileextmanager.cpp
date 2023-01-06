//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : fileextmanager.cpp
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

#include "fileextmanager.h"

#include "JSON.h"
#include "file_logger.h"
#include "fileutils.h"

#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/thread.h>
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>

struct Matcher {
    SmartPtr<wxRegEx> m_regex;
    wxString m_exactMatch;
    FileExtManager::FileType m_fileType;

    Matcher(const wxString& pattern, FileExtManager::FileType fileType, bool regex = true)
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
        auto lines = ::wxStringTokenize(in, "\r\n", wxTOKEN_STRTOK);
        bool use_regex = m_regex;
        for(const auto& line : lines) {
            if(use_regex && m_regex->Matches(line)) {
                return true;
            } else if(!use_regex && line.Find(m_exactMatch) != wxNOT_FOUND) {
                return true;
            }
        }
        return false;
    }
};
namespace
{
std::unordered_map<wxString, FileExtManager::FileType> m_map;
std::unordered_map<wxString, std::vector<FileExtManager::FileType>> m_language_bundle;
std::unordered_map<int, wxString> m_file_type_to_lang;
std::vector<Matcher> m_matchers;
bool init_done = false;
}; // namespace

void FileExtManager::Init()
{
    if(!init_done) {
        init_done = true;
        m_map[wxT("cc")] = TypeSourceCpp;
        m_map[wxT("cpp")] = TypeSourceCpp;
        m_map[wxT("cxx")] = TypeSourceCpp;
        m_map[wxT("c++")] = TypeSourceCpp;
        m_map[wxT("as")] = TypeSourceCpp;  // AngelScript files are handled as C++ source files in CodeLite
        m_map[wxT("ino")] = TypeSourceCpp; // Arduino sketches

        m_map[wxT("c")] = TypeSourceC;
        m_map[wxT("h")] = TypeHeader;
        m_map[wxT("hpp")] = TypeHeader;
        m_map[wxT("hxx")] = TypeHeader;
        m_map[wxT("hh")] = TypeHeader;
        m_map[wxT("h++")] = TypeHeader;
        m_map[wxT("inl")] = TypeHeader;

        m_map[wxT("rc")] = TypeResource;
        m_map[wxT("res")] = TypeResource;

        m_map[wxT("y")] = TypeYacc;

        m_map[wxT("l")] = TypeLex;

        m_map[wxT("ui")] = TypeQtForm;
        m_map[wxT("qrc")] = TypeQtResource;
        m_map[wxT("qml")] = TypeJS;

        m_map[wxT("project")] = TypeProject;
        m_map[wxT("workspace")] = TypeWorkspace;
        m_map[wxT("fbp")] = TypeFormbuilder;

        m_map[wxT("cdp")] = TypeCodedesigner;
        m_map[wxT("erd")] = TypeErd;

        m_map[wxT("php")] = TypePhp;
        m_map[wxT("php5")] = TypePhp;
        m_map[wxT("inc")] = TypePhp;
        m_map[wxT("phtml")] = TypePhp;
        m_map[wxT("ctp")] = TypePhp;

        m_map[wxT("xml")] = TypeXml;
        m_map[wxT("xrc")] = TypeXRC;
        m_map[wxT("css")] = TypeCSS;
        m_map[wxT("less")] = TypeCSS;
        m_map[wxT("sass")] = TypeCSS;
        m_map[wxT("js")] = TypeJS;
        m_map[wxT("javascript")] = TypeJS;
        m_map[wxT("ts")] = TypeJS; // TypeScript, but we consider this a JavaScript
        m_map[wxT("py")] = TypePython;
        m_map["json"] = TypeJSON;
        m_map["conf"] = TypeJSON; // CodeLite configuration files are marked as "conf"

        // Java file
        m_map[wxT("java")] = TypeJava;

        m_map[wxT("exe")] = TypeExe;
        m_map[wxT("html")] = TypeHtml;
        m_map[wxT("htm")] = TypeHtml;

        m_map[wxT("tar")] = TypeArchive;
        m_map[wxT("a")] = TypeArchive;
        m_map[wxT("lib")] = TypeArchive;
        m_map[wxT("zip")] = TypeArchive;
        m_map[wxT("rar")] = TypeArchive;
        m_map[wxT("targz")] = TypeArchive;

        m_map[wxT("dll")] = TypeDll;
        m_map[wxT("so")] = TypeDll;
        m_map[wxT("dylib")] = TypeDll;

        m_map[wxT("bmp")] = TypeBmp;
        m_map[wxT("jpeg")] = TypeBmp;
        m_map[wxT("jpg")] = TypeBmp;
        m_map[wxT("png")] = TypeBmp;
        m_map[wxT("ico")] = TypeBmp;
        m_map[wxT("xpm")] = TypeBmp;
        m_map[wxT("svg")] = TypeSvg;

        m_map[wxT("mk")] = TypeMakefile;

        m_map[wxT("log")] = TypeText;
        m_map[wxT("txt")] = TypeText;
        m_map[wxT("ini")] = TypeText;

        m_map[wxT("script")] = TypeShellScript;
        m_map[wxT("sh")] = TypeShellScript;
        m_map[wxT("bat")] = TypeShellScript;
        m_map[wxT("bash")] = TypeShellScript;

        m_map[wxT("wxcp")] = TypeWxCrafter;
        m_map[wxT("xrc")] = TypeXRC;

        m_map[wxT("sql")] = TypeSQL;
        m_map[wxT("sqlite")] = TypeSQL;
        m_map[wxT("phpwsp")] = TypeWorkspacePHP;
        m_map[wxT("phptags")] = TypeWorkspacePHPTags;

        m_map["pro"] = TypeQMake;
        m_map["pri"] = TypeQMake;
        m_map["cmake"] = TypeCMake;
        m_map["s"] = TypeAsm;
        m_map["yaml"] = TypeYAML;
        m_map["yml"] = TypeYAML;
        m_map["clangd"] = TypeYAML;
        m_map["db"] = TypeDatabase;
        m_map["tags"] = TypeDatabase;
        m_map["lua"] = TypeLua;
        m_map["rs"] = TypeRust;

        m_map["patch"] = TypePatch;
        m_map["diff"] = TypeDiff;

        m_map["rb"] = TypeRuby;
        m_map["md"] = TypeMarkdown;
        m_map["dart"] = TypeDart;

        m_language_bundle.insert({ "C/C++", { TypeSourceCpp, TypeSourceC, TypeHeader } });
        m_language_bundle.insert({ "Windows resource files", { TypeResource } });
        m_language_bundle.insert({ "Yacc", { TypeYacc } });
        m_language_bundle.insert({ "Lex", { TypeLex } });
        m_language_bundle.insert({ "Xml", { TypeProject, TypeWorkspace, TypeXml, TypeXRC, TypeSvg, TypeFormbuilder } });
        m_language_bundle.insert({ "Yaml", { TypeYAML } });
        m_language_bundle.insert({ "Json",
                                   { TypeJSON, TypeWorkspaceFileSystem, TypeWxCrafter, TypeWorkspaceDocker,
                                     TypeWorkspaceNodeJS, TypeWorkspacePHP } });
        m_language_bundle.insert({ "Rust", { TypeRust } });
        m_language_bundle.insert({ "Ruby", { TypeRuby } });
        m_language_bundle.insert({ "Shell script", { TypeShellScript } });
        m_language_bundle.insert({ "Java", { TypeJava } });
        m_language_bundle.insert({ "Javascript/Typescript", { TypeJS } });
        m_language_bundle.insert({ "Python", { TypePython } });
        m_language_bundle.insert({ "PHP", { TypePhp } });
        m_language_bundle.insert({ "CMake", { TypeCMake } });

        // build the reverse search table: file type -> language
        for(auto vt : m_language_bundle) {
            for(auto t : vt.second) {
                m_file_type_to_lang.insert({ (int)t, vt.first });
            }
        }

        // Initialize regexes:
        m_matchers.push_back(Matcher("#[ \t]*!(.*?)sh", TypeShellScript));
        m_matchers.push_back(Matcher("#[ \t]*!(.*?)python", TypePython));
        m_matchers.push_back(Matcher("#[ \t]*!(.*?)ruby", TypeRuby));
        m_matchers.push_back(Matcher("#[ \t]*!(.*?)node", TypeJS));
        m_matchers.push_back(Matcher("<?xml", TypeXml, false));
        m_matchers.push_back(Matcher("<?php", TypePhp, false));
        m_matchers.push_back(Matcher("SQLite format 3", TypeDatabase, false));

        // STL sources places "-*- C++ -*-" at the top of their headers
        m_matchers.push_back(Matcher("-*- C++ -*-", TypeSourceCpp, false));

        // #ifndef WORD
        m_matchers.push_back(Matcher("#ifndef[ \t]+[a-zA-Z0-9_]+", TypeSourceCpp));

        // vim modlines
        m_matchers.push_back(Matcher("/\\* \\-\\*\\- Mode:[ \t]+c\\+\\+", TypeSourceCpp));
        m_matchers.push_back(Matcher("# \\-\\*\\- Mode:[ \t]+python", TypePython));

        // #include <
        m_matchers.push_back(Matcher("#include[ \t]+[\\<\"]", TypeSourceCpp));
    }
}
std::unordered_map<wxString, FileExtManager::FileType> FileExtManager::GetAllSupportedFileTypes()
{
    Init();
    return m_map;
}

std::unordered_map<wxString, std::vector<FileExtManager::FileType>> FileExtManager::GetLanguageBundles()
{
    Init();
    return m_language_bundle;
}

FileExtManager::FileType FileExtManager::GetType(const wxString& filename, FileExtManager::FileType defaultType)
{
    Init();

    wxFileName fn(filename);
    if(fn.IsOk() == false) {
        return defaultType;
    }

    wxString e(fn.GetExt());
    e.MakeLower();
    e.Trim().Trim(false);

    auto iter = m_map.find(e);
    if(iter == m_map.end()) {
        // try to see if the file is a makefile
        if(fn.GetFullName().CmpNoCase(wxT("makefile")) == 0) {
            return TypeMakefile;
        } else if(fn.GetFullName().Lower() == "dockerfile") {
            return TypeDockerfile;
        } else if(fn.GetFullName().CmpNoCase("README") == 0) {
            return TypeMarkdown;
        } else if(fn.GetFullName().CmpNoCase(".clangd") == 0) {
            return TypeYAML;
        } else {
            // try auto detecting
            FileType autoDetectType = defaultType;
            if(AutoDetectByContent(filename, autoDetectType)) {
                return autoDetectType;
            }
        }
        return defaultType;
    } else if((iter->second == TypeText) && (fn.GetFullName().CmpNoCase("CMakeLists.txt") == 0)) {
        return TypeCMake;
    }

    FileExtManager::FileType type = iter->second;
    if(fn.Exists() && (type == TypeWorkspace)) {
        wxString content;
        if(FileUtils::ReadFileContent(fn, content)) {
            if(content.Contains("<CodeLite_Workspace")) {
                return TypeWorkspace;
            } else {
                JSON root(content);
                if(!root.isOk())
                    return TypeWorkspace;
                if(root.toElement().hasNamedObject("NodeJS")) {
                    return TypeWorkspaceNodeJS;
                } else if(root.toElement().hasNamedObject("Docker")) {
                    return TypeWorkspaceDocker;
                } else if(root.toElement().namedObject("workspace_type").toString() == "File System Workspace") {
                    return TypeWorkspaceFileSystem;
                } else if(root.toElement().namedObject("metadata").namedObject("type").toString() == "php") {
                    return TypeWorkspacePHP;
                } else {
                    return TypeWorkspace;
                }
            }
        } else {
            return TypeWorkspace;
        }
    }
    return iter->second;
}

bool FileExtManager::IsCxxFile(const wxString& filename)
{

    FileType ft = GetType(filename);
    if(ft == TypeOther) {
        // failed to detect the type
        if(!AutoDetectByContent(filename, ft)) {
            return false;
        }
    }
    return (ft == TypeSourceC) || (ft == TypeSourceCpp) || (ft == TypeHeader);
}

bool FileExtManager::AutoDetectByContent(const wxString& filename, FileExtManager::FileType& fileType)
{
    wxString fileContent;
    if(!FileUtils::ReadBufferFromFile(filename, fileContent, 1024)) {
        clWARNING() << "Failed to read file's content" << endl;
        return false;
    }
    return GetContentType(fileContent, fileType);
}

bool FileExtManager::GetContentType(const wxString& string_content, FileExtManager::FileType& fileType)
{
    for(size_t i = 0; i < m_matchers.size(); ++i) {
        if(m_matchers[i].Matches(string_content)) {
            LOG_IF_TRACE
            {
                if(m_matchers[i].m_regex) {
                    clDEBUG1() << "Matching part is:" << m_matchers[i].m_regex->GetMatch(string_content, 0) << endl;
                }
            }
            fileType = m_matchers[i].m_fileType;
            return true;
        }
    }
    return false;
}

bool FileExtManager::IsFileType(const wxString& filename, FileExtManager::FileType type)
{

    FileType ft = GetType(filename);
    if(ft == TypeOther) {
        // failed to detect the type
        if(!AutoDetectByContent(filename, ft)) {
            return false;
        }
    }
    return (ft == type);
}

bool FileExtManager::IsJavascriptFile(const wxString& filename) { return FileExtManager::IsFileType(filename, TypeJS); }

bool FileExtManager::IsPHPFile(const wxString& filename) { return FileExtManager::IsFileType(filename, TypePhp); }

bool FileExtManager::IsJavaFile(const wxString& filename) { return FileExtManager::IsFileType(filename, TypeJava); }

FileExtManager::FileType FileExtManager::GetTypeFromExtension(const wxFileName& filename)
{

    std::unordered_map<wxString, FileExtManager::FileType>::iterator iter = m_map.find(filename.GetExt().Lower());
    if(iter == m_map.end())
        return TypeOther;
    return iter->second;
}

bool FileExtManager::IsSymlinkFile(const wxString& filename)
{
    return wxFileName::Exists(filename, wxFILE_EXISTS_NO_FOLLOW | wxFILE_EXISTS_SYMLINK) &&
           wxFileName::FileExists(filename);
}

bool FileExtManager::IsSymlinkFolder(const wxString& filename)
{
    return wxFileName::Exists(filename, wxFILE_EXISTS_NO_FOLLOW | wxFILE_EXISTS_SYMLINK) &&
           wxFileName::DirExists(filename);
}

wxString FileExtManager::GetLanguageFromType(FileExtManager::FileType file_type)
{
    if(m_file_type_to_lang.count((int)file_type) == 0) {
        return wxEmptyString;
    }

    return m_file_type_to_lang.find((int)file_type)->second;
}