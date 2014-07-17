//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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

std::map<wxString, FileExtManager::FileType> FileExtManager::m_map;

void FileExtManager::Init()
{
    static bool init_done(false);

    if ( !init_done ) {
        init_done = true;

        m_map[wxT("cc") ] = TypeSourceCpp;
        m_map[wxT("cpp")] = TypeSourceCpp;
        m_map[wxT("cxx")] = TypeSourceCpp;
        m_map[wxT("c++")] = TypeSourceCpp;
        m_map[wxT("c")  ] = TypeSourceC;

        m_map[wxT("h")   ] = TypeHeader;
        m_map[wxT("hpp") ] = TypeHeader;
        m_map[wxT("hxx") ] = TypeHeader;
        m_map[wxT("hh")  ] = TypeHeader;
        m_map[wxT("h++") ] = TypeHeader;
        m_map[wxT("inl") ] = TypeHeader;

        m_map[wxT("rc")  ] = TypeResource;
        m_map[wxT("res") ] = TypeResource;

        m_map[wxT("y")   ] = TypeYacc;
        m_map[wxT("l")   ] = TypeLex;
        m_map[wxT("ui")  ] = TypeQtForm;
        m_map[wxT("qrc") ] = TypeQtResource;

        m_map[wxT("project") ]   = TypeProject;
        m_map[wxT("workspace") ] = TypeWorkspace;
        m_map[wxT("fbp") ] = TypeFormbuilder;
        m_map[wxT("cdp") ] = TypeCodedesigner;
        m_map[wxT("erd") ] = TypeErd;

        m_map[wxT("php")]   = TypePhp;
        m_map[wxT("inc")]   = TypePhp;
        m_map[wxT("phtml")] = TypePhp;

        m_map[wxT("xml")] = TypeXml;
        m_map[wxT("xrc")] = TypeXRC;
        m_map[wxT("css")] = TypeCSS;
        m_map[wxT("js")]  = TypeJS;
        m_map[wxT("javascript")]  = TypeJS;
        m_map[wxT("py")]  = TypePython;

        m_map[wxT("exe")] = TypeExe;
        m_map[wxT("html")] = TypeHtml;
        m_map[wxT("htm")] = TypeHtml;

        m_map[wxT("tar")]   = TypeArchive;
        m_map[wxT("a")]     = TypeArchive;
        m_map[wxT("lib")]   = TypeArchive;
        m_map[wxT("zip")]   = TypeArchive;
        m_map[wxT("rar")]   = TypeArchive;
        m_map[wxT("targz")] = TypeArchive;

        m_map[wxT("dll")]    = TypeDll;
        m_map[wxT("so")]     = TypeDll;
        m_map[wxT("dylib")]  = TypeDll;

        m_map[wxT("bmp")]  = TypeBmp;
        m_map[wxT("jpeg")] = TypeBmp;
        m_map[wxT("jpg")]  = TypeBmp;
        m_map[wxT("png")]  = TypeBmp;
        m_map[wxT("ico")]  = TypeBmp;
        m_map[wxT("xpm")]  = TypeBmp;

        m_map[wxT("mk")] = TypeMakefile;

        m_map[wxT("log")] = TypeText;
        m_map[wxT("txt")] = TypeText;
        m_map[wxT("ini")] = TypeText;

        m_map[wxT("script")] = TypeScript;
        m_map[wxT("sh")]     = TypeScript;
        m_map[wxT("bat")]    = TypeScript;
        m_map[wxT("bash")]   = TypeScript;

        m_map[wxT("wxcp")] = TypeWxCrafter;
        m_map[wxT("xrc") ] = TypeXRC;

        m_map[wxT("sql")]     = TypeSQL;
        m_map[wxT("phpwsp")]  = TypeWorkspacePHP;
        m_map[wxT("phptags")] = TypeWorkspacePHPTags;

        m_map["s"] = TypeAsm;
    }
}

FileExtManager::FileType FileExtManager::GetType(const wxString& filename, FileExtManager::FileType defaultType)
{
    Init();

    wxFileName fn( filename );
    if ( fn.IsOk() == false ) {
        return defaultType;
    }

    wxString e ( fn.GetExt() );
    e.MakeLower();
    e.Trim().Trim(false);

    std::map<wxString, FileType>::iterator iter = m_map.find(e);
    if ( iter == m_map.end() ) {
        // try to see if the file is a makefile
        if(fn.GetFullName().CmpNoCase(wxT("makefile")) == 0) {
            return TypeMakefile;
        }
        return defaultType;
    }
    return iter->second;
}

bool FileExtManager::IsCxxFile(const wxString& filename)
{
    FileType ft = GetType(filename);
    return ft == TypeSourceC || ft == TypeSourceCpp || ft == TypeHeader;
}

bool FileExtManager::IsJavascriptFile(const wxString& filename)
{
    FileType ft = GetType(filename);
    return ft == TypeJS;
}
