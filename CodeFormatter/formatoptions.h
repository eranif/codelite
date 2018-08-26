//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : formatoptions.h
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
#ifndef FORMATOPTIONS_H
#define FORMATOPTIONS_H

#include "phpoptions.h"
#include "serialized_object.h"

enum AstyleOptions {
    AS_ANSI = 0x00000001,
    AS_GNU = 0x00000002,
    AS_KR = 0x00000004,
    AS_LINUX = 0x00000008,
    AS_JAVA = 0x00000010,
    AS_BRACKETS_BREAK = 0x00000020,
    AS_BRACKETS_ATTACH = 0x00000040,
    AS_BRACKETS_LINUX = 0x00000080,
    AS_BRACKETS_BREAK_CLOSING = 0x00000100,
    AS_INDENT_CLASS = 0x00000200,
    AS_INDENT_SWITCHES = 0x00000400,
    AS_INDENT_CASE = 0x00000800,
    AS_INDENT_BLOCKS = 0x00001000,
    AS_INDENT_BRACKETS = 0x00002000,
    AS_INDENT_NAMESPACES = 0x00004000,
    AS_INDENT_LABELS = 0x00008000,
    AS_INDENT_PREPROCESSORS = 0x00010000,
    AS_MAX_INSTATEMENT_INDENT = 0x00020000,
    AS_BREAK_BLOCKS = 0x00040000,
    AS_BREAK_BLOCKS_ALL = 0x00080000,
    AS_BREAK_ELSEIF = 0x00100000,
    AS_PAD_OPER = 0x00200000,
    AS_PAD_PARENTHESIS = 0x00400000,
    AS_PAD_PARENTHESIS_OUT = 0x00800000,
    AS_PAD_PARENTHESIS_IN = 0x01000000,
    AS_UNPAD_PARENTHESIS = 0x02000000,
    AS_ONE_LINE_KEEP_STATEMENT = 0x04000000,
    AS_ONE_LINE_KEEP_BLOCKS = 0x08000000,
    AS_FILL_EMPTY_LINES = 0x10000000,
    AS_MIN_COND_INDENT = 0x20000000,
    AS_INDENT_USES_TABS = 0x40000000,
    AS_DEFAULT = AS_LINUX | AS_BRACKETS_LINUX | AS_INDENT_PREPROCESSORS,
    AS_ALL_INDENT_OPTIONS = AS_INDENT_CLASS | AS_INDENT_BRACKETS | AS_INDENT_SWITCHES | AS_INDENT_NAMESPACES |
                            AS_INDENT_CASE |
                            AS_INDENT_LABELS |
                            AS_INDENT_BLOCKS |
                            AS_INDENT_PREPROCESSORS |
                            AS_MAX_INSTATEMENT_INDENT |
                            AS_MIN_COND_INDENT,
    AS_ALL_FORMAT_OPTIONS = AS_BREAK_BLOCKS | AS_PAD_PARENTHESIS | AS_BREAK_BLOCKS_ALL | AS_PAD_PARENTHESIS_OUT |
                            AS_BREAK_ELSEIF |
                            AS_PAD_PARENTHESIS_IN |
                            AS_PAD_OPER |
                            AS_UNPAD_PARENTHESIS |
                            AS_ONE_LINE_KEEP_STATEMENT |
                            AS_FILL_EMPTY_LINES |
                            AS_ONE_LINE_KEEP_BLOCKS,
};

enum CXXFormatterEngine {
    kCxxFormatEngineAStyle,
    kCxxFormatEngineClangFormat,
};

enum PHPFormatterEngine {
    kPhpFormatEngineBuiltin,
    kPhpFormatEnginePhpCsFixer,
    kPhpFormatEnginePhpcbf,
};

enum PHPFixserFormatterSettings {
    kPHPFixserFormatFile = (1 << 1),
};

enum PHPFixserFormatterStyle {
    kPcfAllowRisky = (1 << 0),
    kPcfPHP56Migration = (1 << 1),
    kPcfPHP70Migration = (1 << 2),
    kPcfPHP71Migration = (1 << 3),
    kPcfPSR1 = (1 << 4),
    kPcfPSR2 = (1 << 5),
    kPcfSymfony = (1 << 6),
    kPcfShortArray = (1 << 7),
    kPcfLongArray = (1 << 8),
    kPcfAlignDoubleArrow = (1 << 9),
    kPcfStripDoubleArrow = (1 << 10),
    kPcfIgnoreDoubleArrow = (1 << 16),
    kPcfAlignEquals = (1 << 11),
    kPcfStripEquals = (1 << 12),
    kPcfIgnoreEquals = (1 << 17),
    kPcfBlankLineAfterNamespace = (1 << 13),
    kPcfBlankLineAfterOpeningTag = (1 << 14),
    kPcfBlankLineBeforeReturn = (1 << 15),
    kPcfConcatSpaceNone = (1 << 18),
    kPcfConcatSpaceOne = (1 << 19),
    kPcfEmptyReturnStrip = (1 << 20),
    kPcfEmptyReturnKeep = (1 << 21),
    kPcfCombineConsecutiveUnsets = (1 << 22),
    kPcfLinebreakAfterOpeningTag = (1 << 23),
    kPcfMbStrFunctions = (1 << 24),
    kPcfNoBlankLinesBeforeNamespace = (1 << 25),
    kPcfNoMultilineWhitespaceBeforeSemicolons = (1 << 26),
    kPcfNoNullPropertyInitialization = (1 << 27),
    kPcfNoPhp4Constructor = (1 << 28),
    kPcfNoShortEchoTag = (1 << 29),
    kPcfNoUnreachableDefaultArgumentValue = (1 << 30),
    kPcfNoUselessElse = (1 << 31),
};

enum PhpbcfFormatterStyle {
    kWarningSeverity0 = (1 << 0),
    kPhpbcfFormatFile = (1 << 1),
};

enum ClangFormatStyle {
    kClangFormatLLVM = (1 << 0),
    kClangFormatGoogle = (1 << 1),
    kClangFormatWebKit = (1 << 2),
    kClangFormatChromium = (1 << 3),
    kClangFormatMozilla = (1 << 4),
    kAlignEscapedNewlinesLeft = (1 << 5),
    kAlignTrailingComments = (1 << 6),
    kAllowAllParametersOfDeclarationOnNextLine = (1 << 7),
    kAllowShortBlocksOnASingleLine = (1 << 8),
    kAllowShortLoopsOnASingleLine = (1 << 9),
    kAllowShortIfStatementsOnASingleLine = (1 << 10),
    kAlwaysBreakBeforeMultilineStrings = (1 << 11),
    kAlwaysBreakTemplateDeclarations = (1 << 12),
    kBinPackParameters = (1 << 13),
    kBreakBeforeBinaryOperators = (1 << 14),
    kBreakBeforeTernaryOperators = (1 << 15),
    kBreakConstructorInitializersBeforeComma = (1 << 16),
    kIndentCaseLabels = (1 << 17),
    kIndentFunctionDeclarationAfterType = (1 << 18),
    kSpaceBeforeAssignmentOperators = (1 << 19),
    kSpaceBeforeParens = (1 << 20),
    kSpacesInParentheses = (1 << 21),
    kAllowShortFunctionsOnASingleLine = (1 << 22),
    kPointerAlignmentRight = (1 << 23),
    kClangFormatFile = (1 << 24),
};

enum ClangBreakBeforeBraceOpt {
    kLinux = 0x00000001,
    kAttach = 0x00000002,
    kStroustrup = 0x00000004,
    kAllman = 0x00000008,
    kGNU = 0x00000010,
};

// Genral options
enum eCF_GeneralOptions {
    kCF_AutoFormatOnFileSave = (1 << 0),
};

class FormatOptions : public SerializedObject
{
    size_t m_astyleOptions;
    size_t m_clangFormatOptions;
    size_t m_clangBreakBeforeBrace;
    wxString m_customFlags;
    CXXFormatterEngine m_engine;
    PHPFormatterEngine m_phpEngine;
    wxString m_clangFormatExe;
    size_t m_clangColumnLimit; // when indenting, limit the line to fit into a column width
    size_t m_phpFormatOptions;
    size_t m_generalFlags;
    wxString m_PHPCSFixerPhar;
    wxString m_PHPCSFixerPharOptions;
    size_t m_PHPCSFixerPharSettings;
    size_t m_PHPCSFixerPharRules;
    wxString m_PhpcbfPhar;
    size_t m_phpcbfSeverity;
    wxString m_PhpcbfEncoding;
    wxString m_PhpcbfStandard;
    size_t m_PhpcbfPharOptions;
    PhpOptions m_optionsPhp;

private:
    wxString ClangFlagToBool(ClangFormatStyle flag) const;
    wxString ClangBreakBeforeBrace() const;
    /**
     * @brief return the global settings based on the
     * editor settings (namely: tab vs spaces, and tab width)
     */
    wxString ClangGlobalSettings() const;

    /**
     * @brief Check if there is a file of the given name in any of the parent directories of the input file
     */
    bool HasConfigForFile(const wxFileName& fileName, const wxString& configName) const;

public:
    FormatOptions();
    virtual ~FormatOptions();

    void AutodetectSettings();
    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);
    bool HasFlag(eCF_GeneralOptions flag) const
    {
        return m_generalFlags & flag;
    }
    void SetFlag(eCF_GeneralOptions flag, bool b)
    {
        b ? m_generalFlags |= flag : m_generalFlags &= ~flag;
    }
    void SetCustomFlags(const wxString& customFlags)
    {
        this->m_customFlags = customFlags;
    }
    const wxString& GetCustomFlags() const
    {
        return m_customFlags;
    }

    // C++
    void SetEngine(CXXFormatterEngine engine)
    {
        m_engine = engine;
    }
    CXXFormatterEngine GetEngine() const
    {
        return m_engine;
    }

    // AStyle
    void SetOption(size_t options)
    {
        m_astyleOptions = options;
    }
    size_t GetOptions() const
    {
        return m_astyleOptions;
    }
    wxString AstyleOptionsAsString() const;

    // Clang
    wxString ClangFormatCommand(const wxFileName& fileName, wxString originalFileName = "",
                                const int& cursorPosition = wxNOT_FOUND, const int& selStart = wxNOT_FOUND,
                                const int& selEnd = wxNOT_FOUND) const;
    wxString GetClangFormatStyleAsString(const wxFileName& fileName) const;
    void SetClangFormatExe(const wxString& clangFormatExe)
    {
        this->m_clangFormatExe = clangFormatExe;
    }
    const wxString& GetClangFormatExe() const
    {
        return m_clangFormatExe;
    }
    void SetClangColumnLimit(size_t clangColumnLimit)
    {
        this->m_clangColumnLimit = clangColumnLimit;
    }
    size_t GetClangColumnLimit() const
    {
        return m_clangColumnLimit;
    }
    void SetClangBreakBeforeBrace(size_t clangBreakBeforeBrace)
    {
        this->m_clangBreakBeforeBrace = clangBreakBeforeBrace;
    }
    size_t GetClangBreakBeforeBrace() const
    {
        return m_clangBreakBeforeBrace;
    }
    void SetClangFormatOptions(size_t clangFormatOptions)
    {
        this->m_clangFormatOptions = clangFormatOptions;
    }
    size_t GetClangFormatOptions() const
    {
        return m_clangFormatOptions;
    }

    // PHP
    void SetPhpEngine(const PHPFormatterEngine& phpEngine)
    {
        this->m_phpEngine = phpEngine;
    }
    const PHPFormatterEngine& GetPhpEngine() const
    {
        return m_phpEngine;
    }

    // PHP Formatter
    size_t GetPHPFormatterOptions() const
    {
        return m_phpFormatOptions;
    }
    void SetPHPFormatterOptions(size_t options)
    {
        m_phpFormatOptions = options;
    }

    // PHP-CS-FIXER
    bool GetPhpFixerCommand(const wxFileName& fileName, wxString& command);
    wxString GetPhpFixerRules(const wxFileName& fileName);
    void SetPHPCSFixerPhar(const wxString& PHPCSFixerPhar)
    {
        this->m_PHPCSFixerPhar = PHPCSFixerPhar;
    }
    const wxString& GetPHPCSFixerPhar() const
    {
        return m_PHPCSFixerPhar;
    }
    void SetPHPCSFixerPharOptions(const wxString& PHPCSFixerPharOptions)
    {
        this->m_PHPCSFixerPharOptions = PHPCSFixerPharOptions;
    }
    const wxString& GetPHPCSFixerPharOptions() const
    {
        return m_PHPCSFixerPharOptions;
    }
    void SetPHPCSFixerPharSettings(const size_t& PHPCSFixerPharSettings)
    {
        this->m_PHPCSFixerPharSettings = PHPCSFixerPharSettings;
    }
    size_t GetPHPCSFixerPharSettings() const
    {
        return m_PHPCSFixerPharSettings;
    }
    void SetPHPCSFixerPharRules(const size_t& PHPCSFixerPharRules)
    {
        this->m_PHPCSFixerPharRules = PHPCSFixerPharRules;
    }
    size_t GetPHPCSFixerPharRules() const
    {
        return m_PHPCSFixerPharRules;
    }

    // PHPCBF
    bool GetPhpcbfCommand(const wxFileName& fileName, wxString& command);
    wxString GetPhpcbfStandard(const wxFileName& fileName);
    void SetPhpcbfPhar(const wxString& PhpcbfPhar)
    {
        this->m_PhpcbfPhar = PhpcbfPhar;
    }
    const wxString& GetPhpcbfPhar() const
    {
        return m_PhpcbfPhar;
    }
    void SetPhpcbfSeverity(size_t phpcbfSeverity)
    {
        this->m_phpcbfSeverity = phpcbfSeverity;
    }
    size_t GetPhpcbfSeverity() const
    {
        return m_phpcbfSeverity;
    }
    void SetPhpcbfEncoding(const wxString& PhpcbfEncoding)
    {
        this->m_PhpcbfEncoding = PhpcbfEncoding;
    }
    const wxString& GetPhpcbfEncoding() const
    {
        return m_PhpcbfEncoding;
    }
    void SetPhpcbfStandard(const wxString& PhpcbfStandard)
    {
        this->m_PhpcbfStandard = PhpcbfStandard;
    }
    const wxString& GetPhpcbfStandard() const
    {
        return m_PhpcbfStandard;
    }
    void SetPhpcbfOptions(size_t phpcbfPharOptions)
    {
        this->m_PhpcbfPharOptions = phpcbfPharOptions;
    }
    size_t GetPhpcbfOptions() const
    {
        return m_PhpcbfPharOptions;
    }
};

#endif // FORMATOPTIONS_H
