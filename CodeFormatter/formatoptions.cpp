//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : formatoptions.cpp
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
#include "formatoptions.h"
#include "clClangFormatLocator.h"
#include "editor_config.h"

FormatOptions::FormatOptions()
    : m_astyleOptions(AS_DEFAULT | AS_INDENT_USES_TABS)
    , m_engine(kFormatEngineAStyle)
    , m_clangFormatOptions(kClangFormatWebKit | kAlignTrailingComments | kBreakConstructorInitializersBeforeComma | kSpaceBeforeAssignmentOperators)
    , m_clangBreakBeforeBrace(kLinux)
    , m_clangColumnLimit(0) // No limit
{
    if(m_clangFormatExe.IsEmpty()) {
        clClangFormatLocator locator;
        locator.Locate(m_clangFormatExe);
    }
}

FormatOptions::~FormatOptions()
{
}

void FormatOptions::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_options"), m_astyleOptions);
    arch.Read(wxT("m_customFlags"), m_customFlags);

    int engine = kFormatEngineAStyle;
    arch.Read("m_engine", engine);
    m_engine = static_cast<FormatterEngine>(engine);

    arch.Read("m_clangFormatOptions", m_clangFormatOptions);

    wxString clangFormat;
    arch.Read("m_clangFormatExe", clangFormat);
    if(!clangFormat.IsEmpty()) {
        m_clangFormatExe.swap(clangFormat);
    }

    arch.Read("m_clangBreakBeforeBrace", m_clangBreakBeforeBrace);
    arch.Read("m_clangColumnLimit", m_clangColumnLimit);
}

void FormatOptions::Serialize(Archive& arch)
{
    arch.Write(wxT("m_options"), m_astyleOptions);
    arch.Write(wxT("m_customFlags"), m_customFlags);
    arch.Write("m_engine", static_cast<int>(m_engine));
    arch.Write("m_clangFormatOptions", m_clangFormatOptions);
    arch.Write("m_clangFormatExe", m_clangFormatExe);
    arch.Write("m_clangBreakBeforeBrace", m_clangBreakBeforeBrace);
    arch.Write("m_clangColumnLimit", m_clangColumnLimit);
}

wxString FormatOptions::AstyleOptionsAsString() const
{
    //by default use tabs as indentation with size 4
    wxString options;

    if(m_astyleOptions & AS_ANSI) {
        options << wxT(" --style=ansi ");
    }
    if(m_astyleOptions & AS_GNU) {
        options << wxT(" --style=gnu ");
    }
    if(m_astyleOptions & AS_KR) {
        options << wxT(" --style=kr ");
    }
    if(m_astyleOptions & AS_LINUX) {
        options << wxT(" --style=linux ");
    }
    if(m_astyleOptions & AS_JAVA) {
        options << wxT(" --style=java ");
    }
    if(m_astyleOptions & AS_BRACKETS_BREAK) {
        options << wxT(" -b ");
    }
    if(m_astyleOptions & AS_BRACKETS_ATTACH) {
        options << wxT(" -a ");
    }
    if(m_astyleOptions & AS_BRACKETS_LINUX) {
        options << wxT(" -l ");
    }
    if(m_astyleOptions & AS_BRACKETS_BREAK_CLOSING) {
        options << wxT(" -y ");
    }
    if(m_astyleOptions & AS_INDENT_CLASS) {
        options << wxT(" -C ");
    }
    if(m_astyleOptions & AS_INDENT_SWITCHES) {
        options << wxT(" -S ");
    }
    if(m_astyleOptions & AS_INDENT_CASE) {
        options << wxT(" -K ");
    }
    if(m_astyleOptions & AS_INDENT_BLOCKS) {
        options << wxT(" -B ");
    }
    if(m_astyleOptions & AS_INDENT_NAMESPACES) {
        options << wxT(" -N ");
    }
    if(m_astyleOptions & AS_INDENT_LABELS) {
        options << wxT(" -L ");
    }
    if(m_astyleOptions & AS_INDENT_PREPROCESSORS) {
        options << wxT(" -w ");
    }
    if(m_astyleOptions & AS_MAX_INSTATEMENT_INDENT) {
        options << wxT(" -M ");
    }
    if(m_astyleOptions & AS_BREAK_BLOCKS) {
        options << wxT(" -f ");
    }
    if(m_astyleOptions & AS_BREAK_BLOCKS_ALL) {
        options << wxT(" -F ");
    }
    if(m_astyleOptions & AS_BREAK_ELSEIF) {
        options << wxT(" -e ");
    }
    if(m_astyleOptions & AS_PAD_OPER) {
        options << wxT(" -p ");
    }
    if(m_astyleOptions & AS_PAD_PARENTHESIS) {
        options << wxT(" -P ");
    }
    if(m_astyleOptions & AS_PAD_PARENTHESIS_OUT) {
        options << wxT(" -d ");
    }
    if(m_astyleOptions & AS_PAD_PARENTHESIS_IN) {
        options << wxT(" -D ");
    }
    if(m_astyleOptions & AS_ONE_LINE_KEEP_STATEMENT) {
        options << wxT(" -o ");
    }
    if(m_astyleOptions & AS_ONE_LINE_KEEP_BLOCKS) {
        options << wxT(" -O ");
    }
    if(m_astyleOptions & AS_FILL_EMPTY_LINES) {
        options << wxT(" -E ");
    }
    if(m_astyleOptions & AS_UNPAD_PARENTHESIS) {
        options << wxT(" -U ");
    }
    if(m_customFlags.IsEmpty() == false) {
        options << wxT(" ") << m_customFlags;
    }
    return options;
}

wxString FormatOptions::ClangFormatOptionsAsString() const
{
    wxString options;
    options << " -style=\"{ BasedOnStyle: ";
    if(m_clangFormatOptions & kClangFormatChromium) {
        options << "Chromium";
    } else if(m_clangFormatOptions & kClangFormatGoogle) {
        options << "Google";
    } else if(m_clangFormatOptions & kClangFormatLLVM) {
        options << "LLVM";
    } else if(m_clangFormatOptions & kClangFormatMozilla) {
        options << "Mozilla";
    } else if(m_clangFormatOptions & kClangFormatWebKit) {
        options << "WebKit";
    }

    // add tab width and space vs tabs based on the global editor settings
    options << ClangGlobalSettings();

    options << ", AlignEscapedNewlinesLeft: " << ClangFlagToBool(kAlignEscapedNewlinesLeft);
    options << ", AlignTrailingComments : " << ClangFlagToBool(kAlignTrailingComments);
    options << ", AllowAllParametersOfDeclarationOnNextLine : " << ClangFlagToBool(kAllowAllParametersOfDeclarationOnNextLine);
    options << ", AllowShortFunctionsOnASingleLine : " << ClangFlagToBool(kAllowShortFunctionsOnASingleLine);
    options << ", AllowShortBlocksOnASingleLine : " << ClangFlagToBool(kAllowShortBlocksOnASingleLine);
    options << ", AllowShortLoopsOnASingleLine : " << ClangFlagToBool(kAllowShortLoopsOnASingleLine);
    options << ", AllowShortIfStatementsOnASingleLine : " << ClangFlagToBool(kAllowShortIfStatementsOnASingleLine);
    options << ", AlwaysBreakBeforeMultilineStrings : " << ClangFlagToBool(kAlwaysBreakBeforeMultilineStrings);
    options << ", AlwaysBreakTemplateDeclarations : " << ClangFlagToBool(kAlwaysBreakTemplateDeclarations);
    options << ", BinPackParameters : " << ClangFlagToBool(kBinPackParameters);
    options << ", BreakBeforeBinaryOperators : " << ClangFlagToBool(kBreakBeforeBinaryOperators);
    options << ", BreakBeforeTernaryOperators : " << ClangFlagToBool(kBreakBeforeTernaryOperators);
    options << ", BreakConstructorInitializersBeforeComma : " << ClangFlagToBool(kBreakConstructorInitializersBeforeComma);
    options << ", IndentCaseLabels : " << ClangFlagToBool(kIndentCaseLabels);
    options << ", IndentFunctionDeclarationAfterType : " << ClangFlagToBool(kIndentFunctionDeclarationAfterType);
    options << ", SpaceBeforeAssignmentOperators : " << ClangFlagToBool(kSpaceBeforeAssignmentOperators);
    options << ", SpaceBeforeParens : " << (m_clangFormatOptions & kSpaceBeforeParens ? "Always" : "Never");
    options << ", SpacesInParentheses : " << ClangFlagToBool(kSpacesInParentheses);
    options << ", BreakBeforeBraces : " << ClangBreakBeforeBrace();
    options << ", ColumnLimit : " << m_clangColumnLimit;
    
    options << " }\" ";
    return options;
}

wxString FormatOptions::ClangFlagToBool(ClangFormatStyle flag) const
{
    if(m_clangFormatOptions & flag)
        return "true";
    else
        return "false";
}

wxString FormatOptions::ClangBreakBeforeBrace() const
{
    if(m_clangBreakBeforeBrace & kStroustrup) {
        return "Stroustrup";
    } else if(m_clangBreakBeforeBrace & kAllman) {
        return "Allman";
    } else if(m_clangBreakBeforeBrace & kGNU) {
        return "GNU";
    } else if(m_clangBreakBeforeBrace & kAttach) {
        return "Attach";
    } else {
        // the default
        return "Linux";
    }
}

wxString FormatOptions::ClangGlobalSettings() const
{
    int indentWidth = EditorConfigST::Get()->GetOptions()->GetIndentWidth();
    bool useTabs = EditorConfigST::Get()->GetOptions()->GetIndentUsesTabs();
    
    wxString options;
    options << ", IndentWidth: " << indentWidth;
    options << ", UseTab: " << ( useTabs ? "ForIndentation" : "Never" );
    return options;
}
