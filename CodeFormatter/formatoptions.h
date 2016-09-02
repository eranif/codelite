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
        AS_INDENT_CASE | AS_INDENT_LABELS | AS_INDENT_BLOCKS | AS_INDENT_PREPROCESSORS | AS_MAX_INSTATEMENT_INDENT |
        AS_MIN_COND_INDENT,
    AS_ALL_FORMAT_OPTIONS = AS_BREAK_BLOCKS | AS_PAD_PARENTHESIS | AS_BREAK_BLOCKS_ALL | AS_PAD_PARENTHESIS_OUT |
        AS_BREAK_ELSEIF | AS_PAD_PARENTHESIS_IN | AS_PAD_OPER | AS_UNPAD_PARENTHESIS | AS_ONE_LINE_KEEP_STATEMENT |
        AS_FILL_EMPTY_LINES | AS_ONE_LINE_KEEP_BLOCKS,
};

enum FormatterEngine {
    kFormatEngineAStyle,
    kFormatEngineClangFormat,
};

enum PHPFormatterEngine {
    kPhpFormatEngineBuiltin,
    kPhpFormatEnginePhpCsFixer,
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
    FormatterEngine m_engine;
    PHPFormatterEngine m_phpEngine;
    wxString m_clangFormatExe;
    size_t m_clangColumnLimit; // when indenting, limit the line to fit into a column width
    size_t m_phpFormatOptions;
    size_t m_generalFlags;
    wxString m_phpExecutable;
    wxString m_PHPCSFixerPhar;
    wxString m_PHPCSFixerPharOptions;

private:
    wxString ClangFlagToBool(ClangFormatStyle flag) const;
    wxString ClangBreakBeforeBrace() const;
    /**
     * @brief return the global settings based on the
     * editor settings (namely: tab vs spaces, and tab width)
     */
    wxString ClangGlobalSettings() const;

public:
    FormatOptions();
    virtual ~FormatOptions();

    wxString AstyleOptionsAsString() const;
    wxString ClangFormatOptionsAsString(const wxFileName& filename, double clangFormatVersion) const;
    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);

    void SetClangColumnLimit(size_t clangColumnLimit) { this->m_clangColumnLimit = clangColumnLimit; }
    size_t GetClangColumnLimit() const { return m_clangColumnLimit; }

    void SetClangFormatExe(const wxString& clangFormatExe) { this->m_clangFormatExe = clangFormatExe; }
    const wxString& GetClangFormatExe() const { return m_clangFormatExe; }
    FormatterEngine GetEngine() const { return m_engine; }

    void SetClangBreakBeforeBrace(size_t clangBreakBeforeBrace)
    {
        this->m_clangBreakBeforeBrace = clangBreakBeforeBrace;
    }
    size_t GetClangBreakBeforeBrace() const { return m_clangBreakBeforeBrace; }
    void SetEngine(FormatterEngine engine) { m_engine = engine; }

    size_t GetOptions() const { return m_astyleOptions; }
    void SetOption(size_t options) { m_astyleOptions = options; }
    void SetCustomFlags(const wxString& customFlags) { this->m_customFlags = customFlags; }
    const wxString& GetCustomFlags() const { return m_customFlags; }
    void SetClangFormatOptions(size_t clangFormatOptions) { this->m_clangFormatOptions = clangFormatOptions; }
    size_t GetClangFormatOptions() const { return m_clangFormatOptions; }
    size_t GetPHPFormatterOptions() const { return m_phpFormatOptions; }
    void SetPHPFormatterOptions(size_t options) { m_phpFormatOptions = options; }
    bool HasFlag(eCF_GeneralOptions flag) const { return m_generalFlags & flag; }
    void SetFlag(eCF_GeneralOptions flag, bool b) { b ? m_generalFlags |= flag : m_generalFlags &= ~flag; }
    void SetPHPCSFixerPhar(const wxString& PHPCSFixerPhar) { this->m_PHPCSFixerPhar = PHPCSFixerPhar; }
    void SetPHPCSFixerPharOptions(const wxString& PHPCSFixerPharOptions)
    {
        this->m_PHPCSFixerPharOptions = PHPCSFixerPharOptions;
    }
    void SetPhpEngine(const PHPFormatterEngine& phpEngine) { this->m_phpEngine = phpEngine; }
    void SetPhpExecutable(const wxString& phpExecutable) { this->m_phpExecutable = phpExecutable; }
    const wxString& GetPHPCSFixerPhar() const { return m_PHPCSFixerPhar; }
    const wxString& GetPHPCSFixerPharOptions() const { return m_PHPCSFixerPharOptions; }
    const PHPFormatterEngine& GetPhpEngine() const { return m_phpEngine; }
    const wxString& GetPhpExecutable() const { return m_phpExecutable; }
    wxString GetPhpFixerCommand() const;
};

#endif // FORMATOPTIONS_H
