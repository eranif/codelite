//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : generic_context.h
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
#pragma once

#include "context_base.h"

#include <memory>

class MarkdownStyler;

class clEditor;

/**
 * \ingroup LiteEditor
 * \brief the basic editor from which complicated editors derives from (e.g. ContextCpp)
 *
 * \version 1.0
 * first version
 *
 * \date 04-30-2007
 *
 * \author Eran
 *
 *
 */
class ContextGeneric : public ContextBase
{
    /// Markdown files are styled by MarkdownStyler (a container lexer), and not by wxSTC_LEX_MARKDOWN
    std::unique_ptr<MarkdownStyler> m_markdownStyler;

public:
    //---------------------------------------
    // ctors-dtor
    //---------------------------------------
    // The constructors and the destructor are in the .cpp file, where MarkdownStyler is a complete type
    ContextGeneric(clEditor* container, const wxString& name);
    ContextGeneric();
    explicit ContextGeneric(const wxString& name);
    ~ContextGeneric() override;
    std::shared_ptr<ContextBase> NewInstance(clEditor* container) override;

    //---------------------------------------
    // Operations
    //---------------------------------------
    void ApplySettings() override;

    void ProcessIdleActions() override;
};
