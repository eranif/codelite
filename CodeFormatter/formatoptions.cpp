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
#include "PHPFormatterBuffer.h"
#include "clClangFormatLocator.h"
#include "editor_config.h"
#include "formatoptions.h"
#include "globals.h"
#include "phpoptions.h"

FormatOptions::FormatOptions()
    : m_astyleOptions(AS_DEFAULT | AS_INDENT_USES_TABS)
    , m_engine(kFormatEngineClangFormat)
    , m_phpEngine(kPhpFormatEngineBuiltin)
    , m_clangFormatOptions(kClangFormatWebKit | kAlignTrailingComments | kBreakConstructorInitializersBeforeComma |
          kSpaceBeforeAssignmentOperators |
          kAlignEscapedNewlinesLeft)
    , m_clangBreakBeforeBrace(kLinux)
    , m_clangColumnLimit(120)
    , m_phpFormatOptions(kPFF_Defaults)
    , m_PHPCSFixerPharRules(0)
    , m_generalFlags(0)
    , m_PhpcbfStandard("phpcs.xml")
    , m_PhpcbfEncoding("UTF-8")
    , m_phpcbfSeverity(0)
    , m_PhpcbfPharOptions(0)
{
}

FormatOptions::~FormatOptions()
{
}

void FormatOptions::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_options"), m_astyleOptions);
    arch.Read(wxT("m_customFlags"), m_customFlags);

    // By default, use clang-format as it is more robust and advanced
    int engine = kFormatEngineClangFormat;
    arch.Read("m_engine", engine);
    m_engine = static_cast<FormatterEngine>(engine);

    engine = kPhpFormatEngineBuiltin;
    arch.Read("m_phpEngine", engine);
    m_phpEngine = static_cast<PHPFormatterEngine>(engine);

    arch.Read("m_clangFormatOptions", m_clangFormatOptions);
    arch.Read("m_clangFormatExe", m_clangFormatExe);
    arch.Read("m_clangBreakBeforeBrace", m_clangBreakBeforeBrace);
    arch.Read("m_clangColumnLimit", m_clangColumnLimit);
    arch.Read("m_phpFormatOptions", m_phpFormatOptions);
    arch.Read("m_generalFlags", m_generalFlags);
    arch.Read("m_PHPCSFixerPhar", m_PHPCSFixerPhar);
    arch.Read("m_PHPCSFixerPharOptions", m_PHPCSFixerPharOptions);
    arch.Read("m_PHPCSFixerPharRules", m_PHPCSFixerPharRules);
    arch.Read("m_PhpcbfPhar", m_PhpcbfPhar);
    arch.Read("m_PhpcbfPharOptions", m_PhpcbfPharOptions);

    AutodetectSettings();
}

void FormatOptions::AutodetectSettings()
{
    wxFileName clangFormatExe(m_clangFormatExe);
    if(m_clangFormatExe.IsEmpty() || !clangFormatExe.Exists()) {
        clClangFormatLocator locator;
        if(locator.Locate(m_clangFormatExe)) {
            clangFormatExe.SetPath(m_clangFormatExe);
        }
    }
    if(m_clangFormatExe.IsEmpty() || !clangFormatExe.Exists()) {
        m_engine = kFormatEngineAStyle; // Change the active engine to AStyle
        m_clangFormatExe = "";          // Clear the non existed executable
    }

    // Find an installed php style fixer
    wxFileName phpcsfixerFormatPhar(m_PHPCSFixerPhar);
#ifndef __WXMSW__
    if(m_PHPCSFixerPhar.IsEmpty() || !phpcsfixerFormatPhar.Exists()) {
        if(clFindExecutable("php-cs-fixer", phpcsfixerFormatPhar)) {
            m_PHPCSFixerPhar = phpcsfixerFormatPhar.GetFullPath();
        }
    }
#endif
    if(m_PHPCSFixerPhar.IsEmpty() || !phpcsfixerFormatPhar.Exists()) {
        if(m_phpEngine == kPhpFormatEnginePhpCsFixer) {
            m_phpEngine = kPhpFormatEngineBuiltin;
        }
        m_PHPCSFixerPhar = ""; // Clear the non existed executable
    }

    // Find an installed php style fixer
    wxFileName phpcbfFormatPhar(m_PhpcbfPhar);
#ifndef __WXMSW__
    if(m_PhpcbfPhar.IsEmpty() || !phpcbfFormatPhar.Exists()) {
        if(clFindExecutable("phpcbf", phpcbfFormatPhar)) {
            m_PhpcbfPhar = phpcbfFormatPhar.GetFullPath();
        }
    }
#endif
    if(m_PhpcbfPhar.IsEmpty() || !phpcbfFormatPhar.Exists()) {
        if(m_phpEngine == kPhpFormatEnginePhpcbf) {
            m_phpEngine = kPhpFormatEngineBuiltin;
        }
        m_PhpcbfPhar = ""; // Clear the non existed executable
    }
}

void FormatOptions::Serialize(Archive& arch)
{
    arch.Write(wxT("m_options"), m_astyleOptions);
    arch.Write(wxT("m_customFlags"), m_customFlags);
    arch.Write("m_engine", static_cast<int>(m_engine));
    arch.Write("m_phpEngine", static_cast<int>(m_phpEngine));
    arch.Write("m_clangFormatOptions", m_clangFormatOptions);
    arch.Write("m_clangFormatExe", m_clangFormatExe);
    arch.Write("m_clangBreakBeforeBrace", m_clangBreakBeforeBrace);
    arch.Write("m_clangColumnLimit", m_clangColumnLimit);
    arch.Write("m_phpFormatOptions", m_phpFormatOptions);
    arch.Write("m_generalFlags", m_generalFlags);
    arch.Write("m_PHPCSFixerPhar", m_PHPCSFixerPhar);
    arch.Write("m_PHPCSFixerPharOptions", m_PHPCSFixerPharOptions);
    arch.Write("m_PHPCSFixerPharRules", m_PHPCSFixerPharRules);
    arch.Write("m_PhpcbfPhar", m_PhpcbfPhar);
    arch.Write("m_PhpcbfPharOptions", m_PhpcbfPharOptions);
}

wxString FormatOptions::AstyleOptionsAsString() const
{
    // by default use tabs as indentation with size 4
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

wxString FormatOptions::ClangFormatOptionsAsString(const wxFileName& filename, double clangFormatVersion) const
{
    // If the option: "File" is selected, it overrides everything here
    if(m_clangFormatOptions & kClangFormatFile) {
        // All our settings are taken from .clang-format file
        return " -style=file";
    }

    wxString options, forceLanguage;

    // Try to autodetect the file type
    if(clangFormatVersion >= 3.5) {
        if(FileExtManager::IsJavascriptFile(filename)) {
            forceLanguage << "Language : JavaScript";

        } else if(FileExtManager::IsCxxFile(filename)) {
            forceLanguage << "Language : Cpp";

        } else if(FileExtManager::IsJavaFile(filename)) {
            forceLanguage << "Language : Java";
        }
    }

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

    // Language
    if(!forceLanguage.IsEmpty()) {
        options << ", " << forceLanguage << " ";
    }
    options << ", AlignEscapedNewlinesLeft: " << ClangFlagToBool(kAlignEscapedNewlinesLeft);
    options << ", AlignTrailingComments : " << ClangFlagToBool(kAlignTrailingComments);
    options << ", AllowAllParametersOfDeclarationOnNextLine : "
            << ClangFlagToBool(kAllowAllParametersOfDeclarationOnNextLine);
    if(clangFormatVersion >= 3.5) {
        options << ", AllowShortFunctionsOnASingleLine : " << ClangFlagToBool(kAllowShortFunctionsOnASingleLine);
        options << ", AllowShortBlocksOnASingleLine : " << ClangFlagToBool(kAllowShortBlocksOnASingleLine);
    }
    options << ", AllowShortLoopsOnASingleLine : " << ClangFlagToBool(kAllowShortLoopsOnASingleLine);
    options << ", AllowShortIfStatementsOnASingleLine : " << ClangFlagToBool(kAllowShortIfStatementsOnASingleLine);
    options << ", AlwaysBreakBeforeMultilineStrings : " << ClangFlagToBool(kAlwaysBreakBeforeMultilineStrings);
    options << ", AlwaysBreakTemplateDeclarations : " << ClangFlagToBool(kAlwaysBreakTemplateDeclarations);
    options << ", BinPackParameters : " << ClangFlagToBool(kBinPackParameters);
    options << ", BreakBeforeBinaryOperators : " << ClangFlagToBool(kBreakBeforeBinaryOperators);
    options << ", BreakBeforeTernaryOperators : " << ClangFlagToBool(kBreakBeforeTernaryOperators);
    options << ", BreakConstructorInitializersBeforeComma : "
            << ClangFlagToBool(kBreakConstructorInitializersBeforeComma);
    options << ", IndentCaseLabels : " << ClangFlagToBool(kIndentCaseLabels);
    options << ", IndentFunctionDeclarationAfterType : " << ClangFlagToBool(kIndentFunctionDeclarationAfterType);
    options << ", SpaceBeforeAssignmentOperators : " << ClangFlagToBool(kSpaceBeforeAssignmentOperators);
    if(clangFormatVersion >= 3.5) {
        options << ", SpaceBeforeParens : " << (m_clangFormatOptions & kSpaceBeforeParens ? "Always" : "Never");
    }
    options << ", SpacesInParentheses : " << ClangFlagToBool(kSpacesInParentheses);
    options << ", BreakBeforeBraces : " << ClangBreakBeforeBrace();
    options << ", ColumnLimit : " << m_clangColumnLimit;
    if(clangFormatVersion >= 3.5) {
        options << ", PointerAlignment : " << (m_clangFormatOptions & kPointerAlignmentRight ? "Right" : "Left");
    }
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
    options << ", UseTab: " << (useTabs ? "ForIndentation" : "Never");
    return options;
}

wxString FormatOptions::GetPhpFixerCommand()
{
    m_optionsPhp.Load();
    wxString command, phar, php, parameters;
    php << m_optionsPhp.GetPhpExe();
    ::WrapWithQuotes(php);

    phar << GetPHPCSFixerPhar();
    ::WrapWithQuotes(phar);

    parameters << GetPHPCSFixerPharOptions();
    if(parameters.IsEmpty()) {
        if(m_PHPCSFixerPharRules & kAllowRisky) {
            parameters << " --allow-risky=yes";
        }
    }
    parameters.Trim().Trim(false);
    command << php << " " << phar << " fix " << parameters;
    return command;
}

wxString FormatOptions::GetPhpcbfCommand()
{
    m_optionsPhp.Load();
    wxString command, phar, php, parameters;
    php << m_optionsPhp.GetPhpExe();
    ::WrapWithQuotes(php);

    phar << GetPhpcbfPhar();
    ::WrapWithQuotes(phar);

    if(m_PhpcbfStandard != "phpcs.xml") {
        parameters << " --standard=" << m_PhpcbfStandard;
    }
    if(m_PhpcbfEncoding != "") {
        parameters << " --encoding=" << m_PhpcbfEncoding;
    }
    if(m_phpcbfSeverity) {
        parameters << " --severity=" << m_phpcbfSeverity;
    }
    if(m_PhpcbfPharOptions & kWarningSeverity0) {
        parameters << " -n";
    }
    parameters.Trim().Trim(false);
    // no-patch is needed for files in /tmp, or it thinkgs it's risky...
    command << php << " " << phar << " " << parameters;
    return command;
}
