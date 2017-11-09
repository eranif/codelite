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
#include <wx/filename.h>
#include "fileutils.h"
#include <wx/regex.h>
#include <wx/xml/xml.h>
#include "json_node.h"

std::unordered_map<wxString, FileExtManager::FileType> FileExtManager::m_map;
std::vector<FileExtManager::Matcher::Ptr_t> FileExtManager::m_matchers;

void FileExtManager::Init()
{
    static bool init_done(false);

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
        m_map[wxT("js")] = TypeJS;
        m_map[wxT("javascript")] = TypeJS;
        m_map[wxT("py")] = TypePython;

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

        m_map[wxT("mk")] = TypeMakefile;

        m_map[wxT("log")] = TypeText;
        m_map[wxT("txt")] = TypeText;
        m_map[wxT("ini")] = TypeText;

        m_map[wxT("script")] = TypeScript;
        m_map[wxT("sh")] = TypeScript;
        m_map[wxT("bat")] = TypeScript;
        m_map[wxT("bash")] = TypeScript;

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

        // Initialize regexes:
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("#[ \t]*![ \t]*/bin/bash", TypeScript)));
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("#[ \t]*![ \t]*/bin/sh", TypeScript)));
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("#[ \t]*![ \t]*/usr/bin/sh", TypeScript)));
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("#[ \t]*![ \t]*/usr/bin/bash", TypeScript)));
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("#[ \t]*![ \t]*/bin/python", TypePython)));
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("#[ \t]*![ \t]*/usr/bin/python", TypePython)));
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("<?xml", TypeXml, false)));
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("<?php", TypePhp, false)));

        // STL sources places "-*- C++ -*-" at the top of their headers
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("-*- C++ -*-", TypeSourceCpp, false)));

        // #ifndef WORD
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("#ifndef[ \t]+[a-zA-Z0-9_]+", TypeSourceCpp)));

        // vim modlines
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("/\\* \\-\\*\\- Mode:[ \t]+c\\+\\+", TypeSourceCpp)));
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("# \\-\\*\\- Mode:[ \t]+python", TypePython)));

        // #include <
        m_matchers.push_back(Matcher::Ptr_t(new Matcher("#include[ \t]+[\\<\"]", TypeSourceCpp)));
    }
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

    std::unordered_map<wxString, FileType>::iterator iter = m_map.find(e);
    if(iter == m_map.end()) {
        // try to see if the file is a makefile
        if(fn.GetFullName().CmpNoCase(wxT("makefile")) == 0) {
            return TypeMakefile;
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
                JSONRoot root(content);
                if(!root.isOk()) return TypeWorkspace;
                if(root.toElement().hasNamedObject("NodeJS")) {
                    return TypeWorkspaceNodeJS;
                } else {
                    return TypeWorkspacePHP;
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
    if(!FileUtils::ReadFileContent(filename, fileContent)) return false;

    // Use only the first 4K bytes from the input file (tested with default STL headers)
    if(fileContent.length() > 4096) {
        fileContent.Truncate(4096);
    }

    for(size_t i = 0; i < m_matchers.size(); ++i) {
        if(m_matchers.at(i)->Matches(fileContent)) {
            fileType = m_matchers.at(i)->m_fileType;
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
    if(iter == m_map.end()) return TypeOther;
    return iter->second;
}
