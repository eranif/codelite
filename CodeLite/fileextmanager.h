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

#include <wx/string.h>
#include <map>
#include "codelite_exports.h"

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
        TypeProjectActive,
        TypeWorkspacePHP,
        TypeWorkspacePHPTags,
        TypeWorkspaceDatabase,
        TypeAsm,
        TypeOther = wxNOT_FOUND
    };

private:
    static std::map<wxString, FileType> m_map;

public:
    static FileType GetType(const wxString &filename, FileExtManager::FileType defaultType = FileExtManager::TypeOther);
    static void     Init();
};
#endif // __fileextmanager__
