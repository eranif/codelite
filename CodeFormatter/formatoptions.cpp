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

#include "JSON.h"
#include "PHPFormatterBuffer.h"
#include "clClangFormatLocator.h"
#include "clRustLocator.hpp"
#include "cl_standard_paths.h"
#include "editor_config.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "macromanager.h"
#include "phpoptions.h"

FormatOptions::FormatOptions()
    : m_astyleOptions(AS_DEFAULT | AS_INDENT_USES_TABS)
    , m_engine(kCxxFormatEngineClangFormat)
    , m_phpEngine(kPhpFormatEngineBuiltin)
    , m_rustEngine(kRustFormatEngineRustfmt)
    , m_clangFormatOptions(kClangFormatWebKit | kAlignTrailingComments | kBreakConstructorInitializersBeforeComma |
                           kSpaceBeforeAssignmentOperators | kAlignEscapedNewlinesLeft | kClangFormatFile)
    , m_clangBreakBeforeBrace(kLinux)
    , m_clangColumnLimit(120)
    , m_phpFormatOptions(kPFF_Defaults)
    , m_PHPCSFixerPharSettings(0)
    , m_PHPCSFixerPharRules(0)
    , m_generalFlags(0)
    , m_PhpcbfStandard("PEAR")
    , m_PhpcbfEncoding("UTF-8")
    , m_phpcbfSeverity(0)
    , m_PhpcbfPharOptions(0)
{
    // We need something unique to identity the preview file
    m_previewFileName = FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "preview", "sample");
}

FormatOptions::~FormatOptions() {}

#define READ_ENGINE_PROPERTY(property, default_value)           \
    {                                                           \
        int tmp_engine = default_value;                         \
        arch.Read(#property, tmp_engine);                       \
        property = static_cast<decltype(property)>(tmp_engine); \
    }

void FormatOptions::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_options"), m_astyleOptions);
    arch.Read(wxT("m_customFlags"), m_customFlags);

    // By default, use clang-format as it is more robust and advanced
    READ_ENGINE_PROPERTY(m_engine, kCxxFormatEngineClangFormat);
    READ_ENGINE_PROPERTY(m_rustEngine, kRustFormatEngineRustfmt);
    READ_ENGINE_PROPERTY(m_phpEngine, kPhpFormatEngineBuiltin);
    READ_ENGINE_PROPERTY(m_xmlEngine, kXmlFormatEngineBuiltin);
    READ_ENGINE_PROPERTY(m_javaScriptEngine, kJSFormatEngineClangFormat);
    READ_ENGINE_PROPERTY(m_jsonEngine, kJSONFormatEngineBuiltin);

    arch.Read("m_clangFormatOptions", m_clangFormatOptions);
    arch.Read("m_clangFormatExe", m_clangFormatExe);
    arch.Read("m_clangBreakBeforeBrace", m_clangBreakBeforeBrace);
    arch.Read("m_clangBraceWrap", m_clangBraceWrap);
    arch.Read("m_clangColumnLimit", m_clangColumnLimit);
    arch.Read("m_phpFormatOptions", m_phpFormatOptions);
    arch.Read("m_generalFlags", m_generalFlags);
    arch.Read("m_PHPCSFixerPhar", m_PHPCSFixerPhar);
    arch.Read("m_PHPCSFixerPharOptions", m_PHPCSFixerPharOptions);
    arch.Read("m_PHPCSFixerPharSettings", m_PHPCSFixerPharSettings);
    arch.Read("m_PHPCSFixerPharRules", m_PHPCSFixerPharRules);
    arch.Read("m_PhpcbfPhar", m_PhpcbfPhar);
    arch.Read("m_PhpcbfPharOptions", m_PhpcbfPharOptions);
    arch.Read("m_rustCommand", m_rustCommand);
    arch.Read("m_rustConfigFile", m_rustConfigFile);
    arch.Read("m_rustConfigContent", m_rustConfigContent);
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
        m_engine = kCxxFormatEngineAStyle; // Change the active engine to AStyle
        m_clangFormatExe = "";             // Clear the non existed executable
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

    // locate rustfmt
    clRustLocator rustLocator;
    if(!rustLocator.Locate()) {
        clDEBUG() << "Could not locate rust" << endl;
        return;
    } else {
        clDEBUG() << "cargo is installed under:" << rustLocator.GetRustTool("cargo") << endl;
    }

    if(m_rustCommand.empty()) {
        wxString rustfmt = rustLocator.GetRustTool("cargo-fmt");
        m_rustCommand = rustfmt;
        clDEBUG() << "rustfmt command is set to" << rustfmt << endl;
    }
}

void FormatOptions::Serialize(Archive& arch)
{
    arch.Write(wxT("m_options"), m_astyleOptions);
    arch.Write(wxT("m_customFlags"), m_customFlags);
    arch.Write("m_engine", static_cast<int>(m_engine));
    arch.Write("m_phpEngine", static_cast<int>(m_phpEngine));
    arch.Write("m_rustEngine", static_cast<int>(m_rustEngine));
    arch.Write("m_xmlEngine", static_cast<int>(m_xmlEngine));
    arch.Write("m_javaScriptEngine", static_cast<int>(m_javaScriptEngine));
    arch.Write("m_jsonEngine", static_cast<int>(m_jsonEngine));
    arch.Write("m_clangFormatOptions", m_clangFormatOptions);
    arch.Write("m_clangFormatExe", m_clangFormatExe);
    arch.Write("m_clangBreakBeforeBrace", m_clangBreakBeforeBrace);
    arch.Write("m_clangBraceWrap", m_clangBraceWrap);
    arch.Write("m_clangColumnLimit", m_clangColumnLimit);
    arch.Write("m_phpFormatOptions", m_phpFormatOptions);
    arch.Write("m_generalFlags", m_generalFlags);
    arch.Write("m_PHPCSFixerPhar", m_PHPCSFixerPhar);
    arch.Write("m_PHPCSFixerPharOptions", m_PHPCSFixerPharOptions);
    arch.Write("m_PHPCSFixerPharSettings", m_PHPCSFixerPharSettings);
    arch.Write("m_PHPCSFixerPharRules", m_PHPCSFixerPharRules);
    arch.Write("m_PhpcbfPhar", m_PhpcbfPhar);
    arch.Write("m_PhpcbfPharOptions", m_PhpcbfPharOptions);
    arch.Write("m_rustCommand", m_rustCommand);
    arch.Write("m_rustConfigFile", m_rustConfigFile);
    arch.Write("m_rustConfigContent", m_rustConfigContent);
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

wxString FormatOptions::ClangFormatCommand(const wxFileName& fileName, wxString originalFileName,
                                           const int& cursorPosition, const int& selStart, const int& selEnd) const
{
    wxString command, filePath;

    command << GetClangFormatExe();
    ::WrapWithQuotes(command);

    filePath = fileName.GetFullPath();
    ::WrapWithQuotes(filePath);

    if(cursorPosition != wxNOT_FOUND) {
        command << " -cursor=" << cursorPosition;
    }

    if(originalFileName != "") {
        ::WrapWithQuotes(originalFileName);
        command << " -assume-filename=" << originalFileName;
    } else {
        command << " -i"; // Overwrite input file
    }

    if(selStart != wxNOT_FOUND && selEnd != wxNOT_FOUND) {
        command << " -offset=" << selStart << " -length=" << (selEnd - selStart);
    }

    if((m_clangFormatOptions & kClangFormatFile) && !fileName.GetName().StartsWith(m_previewFileName.GetName()) &&
       HasConfigForFile(fileName, ".clang-format", nullptr)) {
        command << " -style=file";
    } else {
        // Please note that it must be -style={key: value}, not -style="{key: value}" or -style={key:value}
        // so we have to wrap the -style switch as well
        command << " \"-style=" << GenerateClangFormat(true, fileName) << "\"";
    }
    command << " " << filePath;
    return command;
}

wxString FormatOptions::RustfmtCommand(const wxFileName& fileName) const
{
    wxString command, filePath;
    command << GetRustCommand();

    // replace environment variables + macros
    command = MacroManager::Instance()->Expand(command, nullptr, wxEmptyString);
    ::WrapWithQuotes(command);

    filePath = fileName.GetFullPath();
    ::WrapWithQuotes(filePath);
    command << " -- " << filePath;
    return command;
}

FormatOptions::ClangFormatMap FormatOptions::CompileClangFormat(const wxFileName& fileName) const
{
    ClangFormatMap root;

    if(m_clangFormatOptions & kClangFormatChromium) {
        root["BasedOnStyle"] = "Chromium";
    } else if(m_clangFormatOptions & kClangFormatGoogle) {
        root["BasedOnStyle"] = "Google";
    } else if(m_clangFormatOptions & kClangFormatLLVM) {
        root["BasedOnStyle"] = "LLVM";
    } else if(m_clangFormatOptions & kClangFormatMozilla) {
        root["BasedOnStyle"] = "Mozilla";
    } else if(m_clangFormatOptions & kClangFormatMicrosoft) {
        root["BasedOnStyle"] = "Microsoft";
    } else if(m_clangFormatOptions & kClangFormatGNU) {
        root["BasedOnStyle"] = "GNU";
    } else {
        // Default style
        root["BasedOnStyle"] = "WebKit";
    }

    // add tab width and space vs tabs based on the global editor settings
    ClangFormatMap globalSettings = ClangGlobalSettings();
    root.insert(globalSettings.begin(), globalSettings.end());

    if(fileName.IsOk()) {
        if(FileExtManager::IsJavascriptFile(fileName)) {
            root["Language"] = "JavaScript";
        } else if(FileExtManager::IsCxxFile(fileName)) {
            root["Language"] = "Cpp";
        } else if(FileExtManager::IsJavaFile(fileName)) {
            root["Language"] = "Java";
        }
    }

    root["AlignEscapedNewlinesLeft"] = bool(m_clangFormatOptions & kAlignEscapedNewlinesLeft);
    root["AlignTrailingComments"] = bool(m_clangFormatOptions & kAlignTrailingComments);
    root["AllowAllParametersOfDeclarationOnNextLine"] =
        bool(m_clangFormatOptions & kAllowAllParametersOfDeclarationOnNextLine);
    root["AllowShortFunctionsOnASingleLine"] = bool(m_clangFormatOptions & kAllowShortFunctionsOnASingleLine);
    root["AllowShortBlocksOnASingleLine"] = bool(m_clangFormatOptions & kAllowShortBlocksOnASingleLine);
    root["AllowShortLoopsOnASingleLine"] = bool(m_clangFormatOptions & kAllowShortLoopsOnASingleLine);
    root["AllowShortIfStatementsOnASingleLine"] = bool(m_clangFormatOptions & kAllowShortIfStatementsOnASingleLine);
    root["AlwaysBreakBeforeMultilineStrings"] = bool(m_clangFormatOptions & kAlwaysBreakBeforeMultilineStrings);
    root["AlwaysBreakTemplateDeclarations"] = bool(m_clangFormatOptions & kAlwaysBreakTemplateDeclarations);
    root["BinPackParameters"] = bool(m_clangFormatOptions & kBinPackParameters);
    root["BreakBeforeBinaryOperators"] = bool(m_clangFormatOptions & kBreakBeforeBinaryOperators);
    root["BreakBeforeTernaryOperators"] = bool(m_clangFormatOptions & kBreakBeforeTernaryOperators);
    root["BreakConstructorInitializersBeforeComma"] =
        bool(m_clangFormatOptions & kBreakConstructorInitializersBeforeComma);
    root["IndentCaseLabels"] = bool(m_clangFormatOptions & kIndentCaseLabels);
    root["IndentFunctionDeclarationAfterType"] = bool(m_clangFormatOptions & kIndentFunctionDeclarationAfterType);
    root["SpaceBeforeAssignmentOperators"] = bool(m_clangFormatOptions & kSpaceBeforeAssignmentOperators);
    root["SpaceBeforeParens"] = (m_clangFormatOptions & kSpaceBeforeParens ? "Always" : "Never");
    root["SpacesInParentheses"] = bool(m_clangFormatOptions & kSpacesInParentheses);
    root["BreakBeforeBraces"] = ClangBreakBeforeBrace();
    if(m_clangBreakBeforeBrace & kCustom) {
        ClangFormatMap braceWrapping;
        braceWrapping["AfterCaseLabel"] = bool(m_clangBraceWrap & kAfterCaseLabel);
        braceWrapping["AfterClass"] = bool(m_clangBraceWrap & kAfterClass);
        braceWrapping["AfterControlStatement"] = (m_clangBraceWrap & kAfterControlStatement ? "Always" : "Never");
        braceWrapping["AfterEnum"] = bool(m_clangBraceWrap & kAfterEnum);
        braceWrapping["AfterFunction"] = bool(m_clangBraceWrap & kAfterFunction);
        braceWrapping["AfterObjCDeclaration"] = bool(m_clangBraceWrap & kAfterObjCDeclaration);
        braceWrapping["AfterStruct"] = bool(m_clangBraceWrap & kAfterStruct);
        braceWrapping["AfterUnion"] = bool(m_clangBraceWrap & kAfterUnion);
        braceWrapping["AfterExternBlock"] = bool(m_clangBraceWrap & kAfterExternBlock);
        braceWrapping["BeforeCatch"] = bool(m_clangBraceWrap & kBeforeCatch);
        braceWrapping["BeforeElse"] = bool(m_clangBraceWrap & kBeforeElse);
        braceWrapping["BeforeLambdaBody"] = bool(m_clangBraceWrap & kBeforeLambdaBody);
        braceWrapping["BeforeWhile"] = bool(m_clangBraceWrap & kBeforeWhile);
        braceWrapping["IndentBraces"] = bool(m_clangBraceWrap & kIndentBraces);
        braceWrapping["SplitEmptyFunction"] = bool(m_clangBraceWrap & kSplitEmptyFunction);
        braceWrapping["SplitEmptyRecord"] = bool(m_clangBraceWrap & kSplitEmptyRecord);
        braceWrapping["SplitEmptyNamespace"] = bool(m_clangBraceWrap & kSplitEmptyNamespace);
        root["BraceWrapping"] = braceWrapping;
    }
    root["ColumnLimit"] = m_clangColumnLimit;
    root["PointerAlignment"] = (m_clangFormatOptions & kPointerAlignmentRight ? "Right" : "Left");

    return root;
}

wxString FormatOptions::ClangFormatMapToYAML(const ClangFormatMap& compiledMap, bool inlineNotation,
                                             size_t nestLevel) const
{
    wxString content;

    for(const auto& element : compiledMap) {
        const wxString& key = element.first;
        const wxAny& value = element.second;

        if(!content.IsEmpty()) {
            if(inlineNotation) {
                content << ", ";
            } else {
                content << "\n";
                content.Append(' ', nestLevel * 2);
            }
        }
        content << key << ": ";

        if(value.CheckType<ClangFormatMap>()) {
            content << ClangFormatMapToYAML(value.As<ClangFormatMap>(), inlineNotation, nestLevel + 1);
        } else {
            content << value.As<wxString>();
        }
    }

    if(inlineNotation) {
        content.Prepend("{").Append("}");
    }
    return content;
}

wxString FormatOptions::GenerateClangFormat(bool inlineNotation, const wxFileName& fileName) const
{
    ClangFormatMap compiledMap = CompileClangFormat(fileName);
    return ClangFormatMapToYAML(compiledMap, inlineNotation);
}

#define CLANG_FORMAT_CONF_FILE_HEADER "# .clang-format generated by CodeLite"
bool FormatOptions::ExportClangFormatFile(const wxFileName& clangFormatFile) const
{
    wxString content = CLANG_FORMAT_CONF_FILE_HEADER;
    content << "\n" << GenerateClangFormat(false) << "\n";
    clDEBUG() << "Generating .clang-format file...: " << clangFormatFile << endl;
    return FileUtils::WriteFileContent(clangFormatFile, content);
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
    } else if(m_clangBreakBeforeBrace & kMozilla) {
        return "Mozilla";
    } else if(m_clangBreakBeforeBrace & kWhitesmiths) {
        return "Whitesmiths";
    } else if(m_clangBreakBeforeBrace & kCustom) {
        return "Custom";
    } else {
        // the default
        return "Linux";
    }
}

FormatOptions::ClangFormatMap FormatOptions::ClangGlobalSettings() const
{
    int indentWidth = EditorConfigST::Get()->GetOptions()->GetIndentWidth();
    bool useTabs = EditorConfigST::Get()->GetOptions()->GetIndentUsesTabs();
    return { { "IndentWidth", indentWidth }, { "UseTab", (useTabs ? "ForIndentation" : "Never") } };
}

bool FormatOptions::GetPhpFixerCommand(const wxFileName& fileName, wxString& command)
{
    command.Clear();
    m_optionsPhp.Load();
    wxString phar, php, parameters, filePath;
    php = m_optionsPhp.GetPhpExe();
    if(php.IsEmpty()) {
        clDEBUG() << "CodeForamtter: GetPhpFixerCommand(): empty php command" << clEndl;
        return false;
    }
    ::WrapWithQuotes(php);
    phar = GetPHPCSFixerPhar();
    if(phar.IsEmpty()) {
        clDEBUG() << "CodeForamtter: GetPhpFixerCommand(): empty php-cs-fixer phar path" << clEndl;
        return false;
    }
    ::WrapWithQuotes(phar);

    parameters = GetPHPCSFixerPharOptions();
    if(parameters.IsEmpty()) {
        if(m_PHPCSFixerPharSettings & kPcfAllowRisky) {
            parameters << " --allow-risky=yes";
        }

        parameters << GetPhpFixerRules(fileName);
    }
    parameters.Trim().Trim(false);
    clDEBUG() << parameters << clEndl;

    filePath = fileName.GetFullPath();
    ::WrapWithQuotes(filePath);

    command << php << " " << phar << " fix " << parameters << " " << filePath;
    return true;
}

wxString FormatOptions::GetPhpFixerRules(const wxFileName& fileName)
{
    // If the rules file option is enabled it overrides everything here
    if(m_PHPCSFixerPharSettings & kPHPFixserFormatFile) {
        wxFileName fileLoc;
        // Even if the user specified to use rules file, we only enable it incase it exists
        if(HasConfigForFile(fileName, ".php_cs", &fileLoc) || HasConfigForFile(fileName, ".php_cs.dist", &fileLoc)) {
            return "";
        }
    }

    JSON root(cJSON_Object);
    JSONItem rules = root.toElement();
    if(m_PHPCSFixerPharRules & kPcfPHP56Migration) {
        rules.addProperty("@PHP56Migration", true);
    }
    if(m_PHPCSFixerPharRules & kPcfPHP70Migration) {
        rules.addProperty("@PHP70Migration", true);
        if(m_PHPCSFixerPharSettings & kPcfAllowRisky) {
            rules.addProperty("@PHP70Migration:risky", true);
        }
    }
    if(m_PHPCSFixerPharRules & kPcfPHP71Migration) {
        rules.addProperty("@PHP71Migration", true);
        if(m_PHPCSFixerPharSettings & kPcfAllowRisky) {
            rules.addProperty("@PHP71Migration:risky", true);
        }
    }
    if(m_PHPCSFixerPharRules & kPcfPSR1) {
        rules.addProperty("@PSR1", true);
    }
    if(m_PHPCSFixerPharRules & kPcfPSR2) {
        rules.addProperty("@PSR2", true);
    }
    if(m_PHPCSFixerPharRules & kPcfSymfony) {
        rules.addProperty("@Symfony", true);
        if(m_PHPCSFixerPharSettings & kPcfAllowRisky) {
            rules.addProperty("@Symfony:risky", true);
        }
    }
    if(m_PHPCSFixerPharRules & (kPcfShortArray | kPcfLongArray)) {
        JSONItem array_syntax = JSONItem::createObject("array_syntax");
        array_syntax.addProperty("syntax", m_PHPCSFixerPharRules & kPcfShortArray ? "short" : "long");
        rules.addProperty("array_syntax", array_syntax);
    }
    if(m_PHPCSFixerPharRules &
       (kPcfAlignDoubleArrow | kPcfStripDoubleArrow | kPcfAlignEquals | kPcfStripEquals | kPcfIgnoreDoubleArrow)) {
        JSONItem binary_operator_spaces = JSONItem::createObject("binary_operator_spaces");
        if(m_PHPCSFixerPharRules & (kPcfAlignDoubleArrow | kPcfStripDoubleArrow | kPcfIgnoreDoubleArrow)) {
            binary_operator_spaces.addProperty("align_double_arrow",
                                               m_PHPCSFixerPharRules & kPcfIgnoreDoubleArrow  ? cJSON_NULL
                                               : m_PHPCSFixerPharRules & kPcfAlignDoubleArrow ? true
                                                                                              : false);
        }
        if(m_PHPCSFixerPharRules & (kPcfAlignEquals | kPcfStripEquals | kPcfIgnoreEquals)) {
            binary_operator_spaces.addProperty("align_equals", m_PHPCSFixerPharRules & kPcfIgnoreEquals  ? cJSON_NULL
                                                               : m_PHPCSFixerPharRules & kPcfAlignEquals ? true
                                                                                                         : false);
        }
        rules.addProperty("binary_operator_spaces", binary_operator_spaces);
    }
    if(m_PHPCSFixerPharRules & (kPcfConcatSpaceNone | kPcfConcatSpaceOne)) {
        JSONItem concat_space = JSONItem::createObject("concat_space");
        concat_space.addProperty("spacing", m_PHPCSFixerPharRules & kPcfConcatSpaceNone ? "none" : "one");
        rules.addProperty("concat_space", concat_space);
    }
    if(m_PHPCSFixerPharRules & (kPcfEmptyReturnStrip | kPcfEmptyReturnKeep)) {
        rules.addProperty("phpdoc_no_empty_return", m_PHPCSFixerPharRules & kPcfEmptyReturnStrip ? true : false);
    }
    if(m_PHPCSFixerPharRules & kPcfBlankLineAfterOpeningTag) {
        rules.addProperty("blank_line_after_opening_tag", true);
    }
    if(m_PHPCSFixerPharRules & kPcfBlankLineBeforeReturn) {
        rules.addProperty("blank_line_before_return", true);
    }
    if(m_PHPCSFixerPharRules & kPcfCombineConsecutiveUnsets) {
        rules.addProperty("combine_consecutive_unsets", true);
    }
    if(m_PHPCSFixerPharRules & kPcfLinebreakAfterOpeningTag) {
        rules.addProperty("linebreak_after_opening_tag", true);
    }
    if(m_PHPCSFixerPharRules & kPcfMbStrFunctions) {
        rules.addProperty("mb_str_functions", true);
    }
    if(m_PHPCSFixerPharRules & kPcfNoBlankLinesBeforeNamespace) {
        rules.addProperty("no_blank_lines_before_namespace", true);
    }
    if(m_PHPCSFixerPharRules & kPcfNoMultilineWhitespaceBeforeSemicolons) {
        rules.addProperty("no_multiline_whitespace_before_semicolons", true);
    }
    if(m_PHPCSFixerPharRules & kPcfNoNullPropertyInitialization) {
        rules.addProperty("no_null_property_initialization", true);
    }
    if(m_PHPCSFixerPharRules & kPcfNoPhp4Constructor) {
        rules.addProperty("no_php4_constructor", true);
    }
    if(m_PHPCSFixerPharRules & kPcfNoShortEchoTag) {
        rules.addProperty("no_short_echo_tag", true);
    }
    if(m_PHPCSFixerPharRules & kPcfNoUnreachableDefaultArgumentValue) {
        rules.addProperty("no_unreachable_default_argument_value", true);
    }
    if(m_PHPCSFixerPharRules & kPcfNoUselessElse) {
        rules.addProperty("no_useless_else", true);
    }

    wxString rulesString = rules.FormatRawString(false);
    if(rulesString == "{}") {
        return "";
    }

    return " --rules='" + rulesString + "'";
}

bool FormatOptions::GetPhpcbfCommand(const wxFileName& fileName, wxString& command)
{
    command.Clear();
    m_optionsPhp.Load();
    wxString phar, php, parameters, filePath;
    php = m_optionsPhp.GetPhpExe();
    if(php.IsEmpty()) {
        clDEBUG() << "CodeForamtter: GetPhpcbfCommand(): empty php command" << clEndl;
        return false;
    }
    ::WrapWithQuotes(php);

    phar = GetPhpcbfPhar();
    if(phar.IsEmpty()) {
        clDEBUG() << "CodeForamtter: GetPhpcbfCommand(): empty phpcbf phar path" << clEndl;
        return false;
    }
    ::WrapWithQuotes(phar);

    parameters << GetPhpcbfStandard(fileName);
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

    filePath = fileName.GetFullPath();
    ::WrapWithQuotes(filePath);

    command << php << " " << phar << " " << parameters << " " << filePath;
    return true;
}

wxString FormatOptions::GetPhpcbfStandard(const wxFileName& fileName)
{
    // If the rules file option is enabled it overrides everything here
    if(m_PhpcbfPharOptions & kPhpbcfFormatFile) {
        wxFileName fileLoc;
        // Even if the user specified to use rules file, we only enable it incase it exists
        if(HasConfigForFile(fileName, "phpcs.xml", &fileLoc)) {
            return "";
        }
    }

    return " --standard=" + m_PhpcbfStandard;
}

bool FormatOptions::HasConfigForFile(const wxFileName& fileName, const wxString& configName, wxFileName* fileloc) const
{
    wxFileName configFile(fileName.GetPath(), configName);
    while(configFile.GetDirCount()) {
        if(configFile.FileExists()) {
            if(fileloc) {
                *fileloc = configFile;
            }
            return true;
        }
        configFile.RemoveLastDir();
    }
    return false;
}

#define RUSTFMT_CONF_FILE_HEADER "# .rustfmt.toml generated by CodeLite"
void FormatOptions::GenerateRustfmtTomlFile(const wxFileName& sourceToFormat, wxFileName configFile) const
{
    if(configFile.FileExists()) {
        wxString oldContent;
        if(FileUtils::ReadFileContent(configFile, oldContent) && !oldContent.Contains(RUSTFMT_CONF_FILE_HEADER)) {
            clDEBUG() << "No need to generate rustfmt config file" << endl;
            // this was file was not generated by CodeLite, do not override its content
            return;
        }
    }

    // write the content (override if needed)
    wxString content;
    content << RUSTFMT_CONF_FILE_HEADER << "\n";
    content << GetRustConfigContent();
    if(!configFile.DirExists()) {
        configFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    FileUtils::WriteFileContent(configFile, content);
    clDEBUG() << "Generated file:" << configFile << endl;
}
