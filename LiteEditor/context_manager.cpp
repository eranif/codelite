//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_manager.cpp
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
#include "context_manager.h"

#include "ColoursAndFontsManager.h"
#include "ContextJavaScript.h"
#include "ContextPhp.h"
#include "ContextPython.hpp"
#include "ContextRust.hpp"
#include "context_base.h"
#include "context_cpp.h"
#include "context_diff.h"
#include "context_html.h"
#include "editor_config.h"
#include "generic_context.h"

#include <wx/tokenzr.h>
#include <wx/versioninfo.h>

ContextManager::ContextManager() { Initialize(); }

ContextManager::~ContextManager() {}

ContextBasePtr ContextManager::NewContext(clEditor* parent, const wxString& lexerName)
{
    // this function is actually a big switch ....
    wxString lex_name = lexerName;
    lex_name.MakeLower();
    auto iter = m_contextPool.find(lex_name);
    if(iter == m_contextPool.end()) {
        return ContextBasePtr(m_contextPool["text"]->NewInstance(parent));
    }

    return ContextBasePtr(iter->second->NewInstance((clEditor*)parent));
}

ContextBasePtr ContextManager::NewContextByFileName(clEditor* parent, const wxFileName& fileName)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexerForFile(fileName.GetFullPath());
    if(!lexer) {
        // could not locate a lexer for this file name, return the default text lexer
        return ContextManager::Get()->NewContext(parent, wxT("Text"));
    }
    return ContextManager::Get()->NewContext(parent, lexer->GetName());
}

void ContextManager::Initialize()
{
    // Popuplate the contexts available
    m_contextPool.clear();

    // register available contexts
    m_contextPool["c++"] = std::make_shared<ContextCpp>();
    m_contextPool["diff"] = std::make_shared<ContextDiff>();
    m_contextPool["html"] = std::make_shared<ContextHtml>();
    m_contextPool["php"] = std::make_shared<ContextPhp>();
    m_contextPool["javascript"] = std::make_shared<ContextJavaScript>();
    m_contextPool["python"] = std::make_shared<ContextPython>();
    m_contextPool["rust"] = std::make_shared<ContextRust>();

    // load generic lexers
    wxArrayString names = ColoursAndFontsManager::Get().GetAllLexersNames();
    for(const auto& name : names) {
        if(m_contextPool.count(name) == 0) {
            m_contextPool.insert({ name, std::make_shared<ContextGeneric>(name) });
        }
    }

    // make sure there is a "fallback" lexer for unrecognized file types
    if(m_contextPool.find("text") == m_contextPool.end()) {
        m_contextPool[wxT("text")] = std::make_shared<ContextGeneric>(wxT("text"));
    }
}
