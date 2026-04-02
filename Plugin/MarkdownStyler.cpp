#include "MarkdownStyler.hpp"

#include "ColoursAndFontsManager.h"

#include <unordered_map>
#include <unordered_set>
#include <wx/settings.h>

namespace
{
// Common keywords for various programming languages
const std::unordered_map<wxString, std::unordered_set<wxString>> g_languageKeywords = {
    {"c++", {"alignas",     "alignof",   "and",        "and_eq",    "asm",      "auto",         "bitand",
             "bitor",       "bool",      "break",      "case",      "catch",    "char",         "char8_t",
             "char16_t",    "char32_t",  "class",      "compl",     "concept",  "const",        "consteval",
             "constexpr",   "constinit", "const_cast", "continue",  "co_await", "co_return",    "co_yield",
             "decltype",    "default",   "delete",     "do",        "double",   "dynamic_cast", "else",
             "enum",        "explicit",  "export",     "extern",    "false",    "float",        "for",
             "friend",      "goto",      "if",         "inline",    "int",      "long",         "mutable",
             "namespace",   "new",       "noexcept",   "not",       "not_eq",   "nullptr",      "operator",
             "or",          "or_eq",     "private",    "protected", "public",   "register",     "reinterpret_cast",
             "requires",    "return",    "short",      "signed",    "sizeof",   "static",       "static_assert",
             "static_cast", "struct",    "switch",     "template",  "this",     "thread_local", "throw",
             "true",        "try",       "typedef",    "typeid",    "typename", "union",        "unsigned",
             "using",       "virtual",   "void",       "volatile",  "wchar_t",  "while",        "xor",
             "xor_eq",      "override",  "final"}},
    {"c", {"auto",       "break",     "case",           "char",         "const",    "continue", "default",  "do",
           "double",     "else",      "enum",           "extern",       "float",    "for",      "goto",     "if",
           "inline",     "int",       "long",           "register",     "restrict", "return",   "short",    "signed",
           "sizeof",     "static",    "struct",         "switch",       "typedef",  "union",    "unsigned", "void",
           "volatile",   "while",     "_Alignas",       "_Alignof",     "_Atomic",  "_Bool",    "_Complex", "_Generic",
           "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"}},
    {"python", {"False", "None",     "True",  "and",    "as",   "assert", "async",  "await",    "break",
                "class", "continue", "def",   "del",    "elif", "else",   "except", "finally",  "for",
                "from",  "global",   "if",    "import", "in",   "is",     "lambda", "nonlocal", "not",
                "or",    "pass",     "raise", "return", "try",  "while",  "with",   "yield"}},
    {"java",
     {"abstract",  "assert",   "boolean",  "break",    "byte",    "case",         "catch",     "char",       "class",
      "const",     "continue", "default",  "do",       "double",  "else",         "enum",      "extends",    "final",
      "finally",   "float",    "for",      "goto",     "if",      "implements",   "import",    "instanceof", "int",
      "interface", "long",     "native",   "new",      "package", "private",      "protected", "public",     "return",
      "short",     "static",   "strictfp", "super",    "switch",  "synchronized", "this",      "throw",      "throws",
      "transient", "try",      "void",     "volatile", "while",   "true",         "false",     "null"}},
    {"javascript", {"await",    "break", "case",   "catch",  "class",      "const",   "continue", "debugger", "default",
                    "delete",   "do",    "else",   "enum",   "export",     "extends", "false",    "finally",  "for",
                    "function", "if",    "import", "in",     "instanceof", "new",     "null",     "return",   "super",
                    "switch",   "this",  "throw",  "true",   "try",        "typeof",  "var",      "void",     "while",
                    "with",     "yield", "let",    "static", "async",      "of"}},
    {"rust", {"as",     "async", "await", "break", "const",  "continue", "crate", "dyn",  "else",   "enum",
              "extern", "false", "fn",    "for",   "if",     "impl",     "in",    "let",  "loop",   "match",
              "mod",    "move",  "mut",   "pub",   "ref",    "return",   "self",  "Self", "static", "struct",
              "super",  "trait", "true",  "type",  "unsafe", "use",      "where", "while"}},
    {"go", {"break", "case",   "chan",   "const",  "continue", "default", "defer",     "else", "fallthrough",
            "for",   "func",   "go",     "goto",   "if",       "import",  "interface", "map",  "package",
            "range", "return", "select", "struct", "switch",   "type",    "var"}},
    {"php",
     {"abstract",   "and",       "array",     "as",       "break",      "callable",     "case",       "catch",
      "class",      "clone",     "const",     "continue", "declare",    "default",      "die",        "do",
      "echo",       "else",      "elseif",    "empty",    "enddeclare", "endfor",       "endforeach", "endif",
      "endswitch",  "endwhile",  "eval",      "exit",     "extends",    "final",        "finally",    "for",
      "foreach",    "function",  "global",    "goto",     "if",         "implements",   "include",    "include_once",
      "instanceof", "insteadof", "interface", "isset",    "list",       "namespace",    "new",        "or",
      "print",      "private",   "protected", "public",   "require",    "require_once", "return",     "static",
      "switch",     "throw",     "trait",     "try",      "unset",      "use",          "var",        "while",
      "xor",        "yield"}},
    {"diff", {"diff",         "index",         "---",         "+++",       "@@",       "From",  "Date",
              "Subject",      "Signed-off-by", "Reviewed-by", "Tested-by", "Acked-by", "Cc",    "Reported-by",
              "Suggested-by", "Fixes",         "Link",        "new",       "deleted",  "file",  "mode",
              "similarity",   "rename",        "copy",        "from",      "to",       "binary"}},
    {"cmake",
     {"if",
      "else",
      "elseif",
      "endif",
      "foreach",
      "endforeach",
      "while",
      "endwhile",
      "function",
      "endfunction",
      "macro",
      "endmacro",
      "set",
      "unset",
      "list",
      "string",
      "math",
      "file",
      "find_package",
      "include",
      "add_executable",
      "add_library",
      "target_link_libraries",
      "target_include_directories",
      "target_compile_options",
      "project",
      "cmake_minimum_required",
      "option",
      "message",
      "return",
      "break",
      "continue",
      "add_subdirectory",
      "install",
      "configure_file",
      "enable_testing",
      "add_test",
      "set_property",
      "get_property",
      "add_custom_command",
      "add_custom_target",
      "add_definitions",
      "include_directories",
      "link_directories",
      "aux_source_directory",
      "site_name",
      "target_compile_definitions",
      "target_compile_features",
      "target_sources",
      "cmake_policy"}},
    {"bash", {"if",      "then",     "else",  "elif",     "fi",      "case",     "esac",   "for",    "select",
              "while",   "until",    "do",    "done",     "in",      "function", "time",   "coproc", "let",
              "declare", "typeset",  "local", "readonly", "export",  "unset",    "shift",  "return", "exit",
              "break",   "continue", "trap",  "source",   "eval",    "exec",     "test",   "alias",  "unalias",
              "set",     "unset",    "read",  "echo",     "printf",  "getopts",  "ulimit", "umask",  "wait",
              "jobs",    "bg",       "fg",    "disown",   "suspend", "true",     "false",  "enable", "disable",
              "builtin", "command",  "type",  "hash",     "help",    "cd",       "pwd",    "pushd",  "popd"}},
    {"ruby",
     {"BEGIN",    "END",       "alias",   "and",     "begin",  "break",       "case",        "class",         "def",
      "defined?", "do",        "else",    "elsif",   "end",    "ensure",      "false",       "for",           "if",
      "in",       "module",    "next",    "nil",     "not",    "or",          "redo",        "rescue",        "retry",
      "return",   "self",      "super",   "then",    "true",   "undef",       "unless",      "until",         "when",
      "while",    "yield",     "require", "include", "extend", "attr_reader", "attr_writer", "attr_accessor", "private",
      "public",   "protected", "raise",   "puts",    "print",  "gets",        "lambda",      "proc"}},
    {"docker", {"FROM",       "RUN",        "CMD",     "LABEL",   "EXPOSE", "ENV",     "ADD",        "COPY",
                "ENTRYPOINT", "VOLUME",     "USER",    "WORKDIR", "ARG",    "ONBUILD", "STOPSIGNAL", "HEALTHCHECK",
                "SHELL",      "MAINTAINER", "AS",      "apt-get", "apk",    "yum",     "dnf",        "zypper",
                "update",     "upgrade",    "install", "remove",  "clean",  "cache",   "bash",       "sh"}},
    {"makefile",
     {"define",
      "endef",
      "ifdef",
      "ifndef",
      "ifeq",
      "ifneq",
      "else",
      "endif",
      "include",
      "sinclude",
      "override",
      "export",
      "unexport",
      "private",
      "vpath",
      "VPATH",
      "MAKEFILES",
      "MAKEFILE_LIST",
      "MAKE",
      "SHELL",
      "CURDIR",
      "MAKECMDGOALS",
      ".PHONY",
      ".SUFFIXES",
      ".DEFAULT",
      ".PRECIOUS",
      ".INTERMEDIATE",
      ".SECONDARY",
      ".SECONDEXPANSION",
      ".DELETE_ON_ERROR",
      ".IGNORE",
      ".LOW_RESOLUTION_TIME",
      ".SILENT",
      ".EXPORT_ALL_VARIABLES",
      ".NOTPARALLEL",
      ".ONESHELL",
      ".POSIX",
      "CC",
      "CXX",
      "LD",
      "AR",
      "AS",
      "CFLAGS",
      "CXXFLAGS",
      "LDFLAGS",
      "ASFLAGS",
      "ARFLAGS",
      "CPP",
      "CPPFLAGS",
      "RM",
      "MKDIR",
      "INSTALL"}},
    {"zsh", {"if",       "then",    "else",    "elif",     "fi",      "case",     "esac",     "for",      "select",
             "while",    "until",   "do",      "done",     "in",      "function", "time",     "coproc",   "nocorrect",
             "foreach",  "end",     "repeat",  "autoload", "bindkey", "compctl",  "declare",  "typeset",  "local",
             "readonly", "export",  "unset",   "shift",    "return",  "exit",     "break",    "continue", "trap",
             "source",   "eval",    "exec",    "test",     "alias",   "unalias",  "set",      "unset",    "read",
             "echo",     "printf",  "getopts", "ulimit",   "umask",   "wait",     "jobs",     "bg",       "fg",
             "disown",   "suspend", "true",    "false",    "enable",  "disable",  "builtin",  "command",  "type",
             "hash",     "help",    "cd",      "pwd",      "pushd",   "popd",     "setopt",   "unsetopt", "limit",
             "unlimit",  "whence",  "where",   "which",    "rehash",  "emulate",  "zmodload", "zcompile"}},
    {"tcsh",
     {"if",     "then",     "else",   "else if",  "endif",   "switch",   "case",       "breaksw",  "endsw",  "foreach",
      "end",    "while",    "break",  "continue", "goto",    "repeat",   "alias",      "unalias",  "set",    "unset",
      "setenv", "unsetenv", "source", "eval",     "exec",    "exit",     "shift",      "echo",     "printf", "test",
      "cd",     "pushd",    "popd",   "dirs",     "jobs",    "bg",       "fg",         "suspend",  "kill",   "wait",
      "nice",   "nohup",    "time",   "umask",    "limit",   "unlimit",  "builtin",    "command",  "where",  "which",
      "rehash", "hashstat", "unhash", "alloc",    "bindkey", "complete", "uncomplete", "filetest", "glob",   "history",
      "hup",    "log",      "onintr", "sched",    "stop",    "suspend",  "true",       "false"}},
    {"sh", {"if",     "then",     "else", "elif", "fi",    "case",   "esac",     "for",     "while",  "until",
            "do",     "done",     "in",   "set",  "unset", "export", "readonly", "shift",   "return", "exit",
            "break",  "continue", "trap", "eval", "exec",  "test",   "alias",    "unalias", "read",   "echo",
            "printf", "getopts",  "cd",   "pwd",  "true",  "false",  "command",  "type",    "hash",   "umask",
            "ulimit", "wait",     "jobs", "bg",   "fg",    "kill",   "times",    "local"}},
};

// Normalize language names (e.g., "cpp" -> "c++")
wxString NormalizeLanguage(const wxString& lang)
{
    wxString lower = lang.Lower();
    if (lower == "cpp" || lower == "cxx" || lower == "cc") {
        return "c++";
    } else if (lower == "js" || lower == "ts" || lower == "typescript") {
        return "javascript";
    } else if (lower == "py") {
        return "python";
    } else if (lower == "rs") {
        return "rust";
    } else if (lower == "golang") {
        return "go";
    }
    return lower;
}

inline bool IsValidUrlChar(wxChar c)
{
    // Unreserved: ALPHA / DIGIT / "-" / "." / "_" / "~"
    if (wxIsalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')
        return true;

    // Reserved: ":" / "/" / "?" / "#" / "[" / "]" / "@"
    //           "!" / "$" / "&" / "'" / "(" / ")" / "*"
    //           "+" / "," / ";" / "="
    switch (c) {
    case ':':
    case '/':
    case '?':
    case '#':
    case '[':
    case ']':
    case '@':
    case '!':
    case '$':
    case '&':
    case '\'':
    case '(':
    case ')':
    case '*':
    case '+':
    case ',':
    case ';':
    case '=':
        return true;
    default:
        return false;
    }
}

// ⟪ ⟫

/// Check if `sv` is a number
bool IsNumber(wxStringView sv)
{
    wxString s{sv.data(), sv.length()};
    long v{wxNOT_FOUND};
    return s.ToCLong(&v);
}

} // namespace

wxDEFINE_EVENT(wxEVT_MARKDOWN_LINK_CLICKED, clCommandEvent);

MarkdownStyler::MarkdownStyler(wxStyledTextCtrl* ctrl)
    : clSTCContainerStylerBase(ctrl)
{
    InitInternal(); // Will call the pure virtual method "InitStyles"
    m_ctrl->Bind(wxEVT_STC_HOTSPOT_CLICK, &MarkdownStyler::OnHostspotClicked, this);
}

MarkdownStyler::~MarkdownStyler() { m_ctrl->Unbind(wxEVT_STC_HOTSPOT_CLICK, &MarkdownStyler::OnHostspotClicked, this); }

void MarkdownStyler::OnHostspotClicked(wxStyledTextEvent& event)
{
    wxString url = GetUrlFromPosition(event.GetPosition());
    clCommandEvent event_markdown_url{wxEVT_MARKDOWN_LINK_CLICKED};
    event_markdown_url.SetString(url);
    AddPendingEvent(event_markdown_url);
}

void MarkdownStyler::Reset() {}

void MarkdownStyler::InitStyles()
{
    wxColour default_bg, default_fg, code_bg;
    auto markdown_lexer = ColoursAndFontsManager::Get().GetLexer("markdown");
    CHECK_PTR_RET(markdown_lexer);

    auto rust_lexer = ColoursAndFontsManager::Get().GetLexer("rust");
    CHECK_PTR_RET(rust_lexer);

    auto diff_lexer = ColoursAndFontsManager::Get().GetLexer("diff");
    CHECK_PTR_RET(diff_lexer);

    bool is_dark = markdown_lexer->IsDark();
    auto default_prop = markdown_lexer->GetProperty(wxSTC_MARKDOWN_DEFAULT);
    default_bg = default_prop.GetBgColour();
    default_fg = default_prop.GetFgColour();
    code_bg = default_bg.ChangeLightness(is_dark ? 110 : 90);
    wxColour actor_colour = default_bg.ChangeLightness(is_dark ? 120 : 80);
    wxColour key_colour_fg = is_dark ? wxColour("#FF9933") : wxColour("#CC33FF");
    wxColour key_bracket_colour_fg = default_bg.ChangeLightness(is_dark ? 120 : 80);

    auto keyword = rust_lexer->GetProperty(wxSTC_RUST_WORD);
    auto macro = rust_lexer->GetProperty(wxSTC_RUST_MACRO);
    auto variable = rust_lexer->GetProperty(wxSTC_RUST_WORD5);
    auto string = rust_lexer->GetProperty(wxSTC_RUST_STRING);
    auto number = rust_lexer->GetProperty(wxSTC_RUST_LIFETIME);
    auto block_comment = rust_lexer->GetProperty(wxSTC_RUST_COMMENTBLOCK);
    auto line_comment = rust_lexer->GetProperty(wxSTC_RUST_COMMENTLINE);
    auto codeblock_function = rust_lexer->GetProperty(wxSTC_RUST_WORD6);
    auto diff_del = diff_lexer->GetProperty(wxSTC_DIFF_DELETED);
    auto diff_add = diff_lexer->GetProperty(wxSTC_DIFF_ADDED);
    auto check_mark = diff_lexer->GetProperty(wxSTC_DIFF_ADDED);
    auto error_mark = diff_lexer->GetProperty(wxSTC_DIFF_DELETED);

    auto header_1 = markdown_lexer->GetProperty(wxSTC_MARKDOWN_HEADER1);
    auto header_2 = markdown_lexer->GetProperty(wxSTC_MARKDOWN_HEADER2);
    auto other_headers = markdown_lexer->GetProperty(wxSTC_MARKDOWN_HEADER3);
    auto function = markdown_lexer->GetProperty(wxSTC_MARKDOWN_CODE);
    auto link = markdown_lexer->GetProperty(wxSTC_MARKDOWN_LINK);

    for (int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_ctrl->StyleSetBackground(i, default_bg);
    }

    m_ctrl->StyleSetForeground(MarkdownStyles::kDefault, default_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kDefault, default_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHorizontalLine, block_comment.GetFgColour());

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockTag, default_bg);
    m_ctrl->StyleSetForeground(MarkdownStyles::kBacktick, default_bg);

    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kCodeBlockText, true);
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockText, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeWord, function.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeWord, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeader1, header_1.GetFgColour());
    m_ctrl->StyleSetBold(MarkdownStyles::kHeader1, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeader2, header_2.GetFgColour());
    m_ctrl->StyleSetBold(MarkdownStyles::kHeader1, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeaderText, default_fg);
    m_ctrl->StyleSetBold(MarkdownStyles::kHeaderText, true);
    m_ctrl->StyleSetItalic(MarkdownStyles::kHeaderText, true);
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kHeaderText, true);

    const std::vector<int> header_styles = {kHeader2, kHeader3, kHeader4, kHeader5, kHeader6};
    for (int style : header_styles) {
        m_ctrl->StyleSetForeground(style, other_headers.GetFgColour());
    }

    // Strong style
    m_ctrl->StyleSetForeground(MarkdownStyles::kStrong2Text, default_fg);
    m_ctrl->StyleSetBold(MarkdownStyles::kStrong2Text, true);
    m_ctrl->StyleSetForeground(MarkdownStyles::kStrong2Tag, keyword.GetFgColour());

    // Emphasis style
    m_ctrl->StyleSetForeground(MarkdownStyles::kEmphasis2Text, default_fg);
    m_ctrl->StyleSetItalic(MarkdownStyles::kEmphasis2Text, true);
    m_ctrl->StyleSetForeground(MarkdownStyles::kEmphasis2Tag, keyword.GetFgColour());

    // List items
    m_ctrl->StyleSetForeground(MarkdownStyles::kNumberedListItem, string.GetFgColour());
    m_ctrl->StyleSetForeground(MarkdownStyles::kNumberedListItemDot, header_1.GetFgColour());
    m_ctrl->StyleSetForeground(MarkdownStyles::kListItem, header_1.GetFgColour());

    // URL
    m_ctrl->StyleSetForeground(MarkdownStyles::kUrl, wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT));
    m_ctrl->StyleSetUnderline(MarkdownStyles::kUrl, true);
    m_ctrl->StyleSetHotSpot(MarkdownStyles::kUrl, true);

    // Custom markings
    m_ctrl->StyleSetForeground(MarkdownStyles::kActor, actor_colour);
    m_ctrl->StyleSetBold(MarkdownStyles::kActor, false);

    m_ctrl->StyleSetForeground(MarkdownStyles::kKeyboardKey, key_colour_fg);
    m_ctrl->StyleSetForeground(MarkdownStyles::kKeyboardKeyBracket, key_bracket_colour_fg);

    // Code block syntax highlighting styles
    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockKeyword, keyword.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockKeyword, code_bg);

    // Diff styles
    wxColour diff_add_fg = diff_add.GetFgColour();
    wxColour diff_add_bg = diff_add_fg.ChangeLightness(is_dark ? 50 : 150);

    m_ctrl->StyleSetForeground(MarkdownStyles::kDiffAdd, diff_add_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kDiffAdd, diff_add_bg);
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kDiffAdd, true);

    wxColour diff_del_fg = diff_del.GetFgColour();
    wxColour diff_del_bg = diff_del_fg.ChangeLightness(is_dark ? 50 : 150);

    m_ctrl->StyleSetForeground(MarkdownStyles::kDiffDelete, diff_del_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kDiffDelete, diff_del_bg);
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kDiffDelete, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockString, string.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockString, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockNumber, number.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockNumber, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockFunction, codeblock_function.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockFunction, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockOperator, variable.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockOperator, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockMacro, macro.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockMacro, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockComment, line_comment.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockComment, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCheckMarkSymbol, check_mark.GetFgColour());
    m_ctrl->StyleSetForeground(MarkdownStyles::kErrorSymbol, error_mark.GetFgColour());

    SetStyleCallback([this](clSTCAccessor& accessor) { this->OnStyle(accessor); });
}

wxString MarkdownStyler::GetUrlFromPosition(int pos)
{
    int line_number = m_ctrl->LineFromPosition(pos);
    int start_pos = m_ctrl->PositionFromLine(line_number);
    int end_pos = m_ctrl->GetLineEndPosition(line_number);

    wxString prefix, suffix;
    for (int i = pos; i < end_pos; ++i) {
        if (m_ctrl->GetStyleAt(i) == MarkdownStyles::kUrl) {
            suffix << static_cast<wxChar>(m_ctrl->GetCharAt(i));
        } else {
            break;
        }
    }

    for (int i = pos - 1; pos >= start_pos; --i) {
        char c = m_ctrl->GetCharAt(i);
        if (m_ctrl->GetStyleAt(i) == MarkdownStyles::kUrl) {
            prefix.Prepend(static_cast<wxChar>(m_ctrl->GetCharAt(i)));
        } else {
            break;
        }
    }
    return prefix + suffix;
}

bool MarkdownStyler::IsKeyword(const wxString& word, const wxString& language) const
{
    wxString normalizedLang = NormalizeLanguage(language);
    auto it = g_languageKeywords.find(normalizedLang);
    if (it == g_languageKeywords.end()) {
        return false;
    }
    return it->second.find(word) != it->second.end();
}

bool MarkdownStyler::IsOperator(wxChar ch) const
{
    switch (ch) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '=':
    case '<':
    case '>':
    case '!':
    case '&':
    case '|':
    case '^':
    case '~':
    case '?':
    case ':':
        return true;
    default:
        return false;
    }
}

namespace
{
void StyleToEndOfLine(clSTCAccessor& accessor, int style, bool include_lf)
{
    while (accessor.CanNext() && accessor.GetCurrentChar<int>() != '\n') {
        accessor.SetStyle(style, 1);
    }

    if (include_lf && accessor.CanNext() && accessor.GetCurrentChar<int>() == '\n') {
        accessor.SetStyle(style, 1);
    }
}
} // namespace

void MarkdownStyler::StyleCodeBlockContent(clSTCAccessor& accessor, const wxString& language)
{
    // Normalize language for consistency
    wxString normalizedLang = NormalizeLanguage(language);

    // Handle diff syntax specially
    if (normalizedLang == "diff" || normalizedLang == "patch") {
        wxChar ch = accessor.GetCurrentChar<wxChar>();

        // Check if we're at the start of a line for diff-specific line markers
        if (accessor.IsAtStartOfLine()) {
            // Lines starting with '+'
            if (ch == '+') {
                // Check for '+++' (file marker)
                if (accessor.GetSubstr(3) == "+++") {
                    // Style the entire line as keyword
                    StyleToEndOfLine(accessor, MarkdownStyles::kCodeBlockKeyword, false);
                    return;
                } else {
                    // Style the entire line as string (additions in green)
                    StyleToEndOfLine(accessor, MarkdownStyles::kDiffAdd, true);
                    return;
                }
            }
            // Lines starting with '-'
            else if (ch == '-') {
                // Check for '---' (file marker)
                if (accessor.GetSubstr(3) == "---") {
                    // Style the entire line as keyword
                    StyleToEndOfLine(accessor, MarkdownStyles::kCodeBlockKeyword, false);
                    return;
                } else {
                    // Style the entire line as comment (deletions in muted color)
                    StyleToEndOfLine(accessor, MarkdownStyles::kDiffDelete, true);
                    return;
                }
            }
            // Lines starting with '@@' or starting with 'diff', 'index', etc.
            else if ((ch == '@' && accessor.GetCharAt<wxChar>(1) == '@') || accessor.GetSubstr(4) == "diff" ||
                     accessor.GetSubstr(5) == "index") {
                // Style the entire line as keyword
                StyleToEndOfLine(accessor, MarkdownStyles::kCodeBlockKeyword, false);
                return;
            }
        }

        // Check for comments inside a diff / patch
        if (StyleDiffPatchCommentInCodeBlock(accessor)) {
            return;
        }
    }

    // Handle C++ style comments (//) for languages that support them
    // Most C-family and many modern languages use this style
    bool supportsCppComments = (normalizedLang == "c++" || normalizedLang == "c" || normalizedLang == "java" ||
                                normalizedLang == "javascript" || normalizedLang == "rust" || normalizedLang == "go" ||
                                normalizedLang == "php");

    bool supportsHashComments =
        (normalizedLang == "cmake" || normalizedLang == "sh" || normalizedLang == "bash" || normalizedLang == "zsh" ||
         normalizedLang == "docker" || normalizedLang == "makefile" || normalizedLang == "php" ||
         normalizedLang == "python" || normalizedLang == "ruby");

    if (supportsCppComments && accessor.GetCurrentChar<wxChar>() == '/' && accessor.GetCharAt<wxChar>(1) == '/') {
        // Style the entire line as a comment
        while (accessor.CanNext() && accessor.GetCurrentChar<wxChar>() != '\n') {
            accessor.SetStyle(MarkdownStyles::kCodeBlockComment, 1);
        }
        return;
    }

    if (supportsHashComments && accessor.GetCurrentChar<wxChar>() == '#') {
        // Style the entire line as a comment
        while (accessor.CanNext() && accessor.GetCurrentChar<wxChar>() != '\n') {
            accessor.SetStyle(MarkdownStyles::kCodeBlockComment, 1);
        }
        return;
    }

    wxChar ch = accessor.GetCurrentChar<wxChar>();

    // Handle strings (double quotes)
    if (ch == '"') {
        accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
        while (accessor.CanNext()) {
            ch = accessor.GetCurrentChar<wxChar>();
            if (ch == '\\') {
                // Escape sequence
                accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
                if (accessor.CanNext()) {
                    accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
                }
            } else if (ch == '"') {
                accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
                break;
            } else if (ch == '\n') {
                accessor.SetStyle(MarkdownStyles::kCodeBlockText, 1);
                break;
            } else {
                accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
            }
        }
        return;
    }

    // Handle strings (single quotes)
    if (ch == '\'') {
        accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
        while (accessor.CanNext()) {
            ch = accessor.GetCurrentChar<wxChar>();
            if (ch == '\\') {
                // Escape sequence
                accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
                if (accessor.CanNext()) {
                    accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
                }
            } else if (ch == '\'') {
                accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
                break;
            } else if (ch == '\n') {
                accessor.SetStyle(MarkdownStyles::kCodeBlockText, 1);
                break;
            } else {
                accessor.SetStyle(MarkdownStyles::kCodeBlockString, 1);
            }
        }
        return;
    }

    // Handle operators
    if (IsOperator(ch)) {
        accessor.SetStyle(MarkdownStyles::kCodeBlockOperator, 1);
        return;
    }

    // Handle numbers
    if (::wxIsdigit(ch)) {
        int count = 1;
        while (accessor.CanPeek(count)) {
            wxChar next = accessor.GetCharAt<wxChar>(count);
            if (::wxIsdigit(next) || next == '.' || next == 'x' || next == 'X' || next == 'e' || next == 'E' ||
                next == 'f' || next == 'F' || next == 'u' || next == 'U' || next == 'l' || next == 'L') {
                count++;
            } else if (::wxIsalpha(next)) {
                // Could be a hex digit
                if ((next >= 'a' && next <= 'f') || (next >= 'A' && next <= 'F')) {
                    count++;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        accessor.SetStyle(MarkdownStyles::kCodeBlockNumber, count);
        return;
    }

    // Handle identifiers (keywords and functions)
    if (::wxIsalpha(ch) || ch == '_') {
        wxString word;
        word << ch;
        int count = 1;

        while (accessor.CanPeek(count)) {
            wxChar next = accessor.GetCharAt<wxChar>(count);
            if (::wxIsalnum(next) || next == '_') {
                word << next;
                count++;
            } else {
                break;
            }
        }

        // Check if it's a Rust macro (identifier followed by '!')
        if (language == "rust" || language == "rs") {
            bool isMacro = false;
            int peek = count;
            while (accessor.CanPeek(peek)) {
                wxChar next = accessor.GetCharAt<wxChar>(peek);
                if (next == '!') {
                    isMacro = true;
                    // Include the '!' in the macro style
                    count = peek + 1;
                    break;
                } else if (next == ' ' || next == '\t') {
                    peek++;
                } else {
                    break;
                }
            }
            if (isMacro) {
                accessor.SetStyle(MarkdownStyles::kCodeBlockMacro, count);
                return;
            }
        }

        // Check if it's a keyword
        if (IsKeyword(word, language)) {
            accessor.SetStyle(MarkdownStyles::kCodeBlockKeyword, count);
            return;
        }

        // Check if it's followed by '(' which indicates a function
        bool isFunction = false;
        int peek = count;
        while (accessor.CanPeek(peek)) {
            wxChar next = accessor.GetCharAt<wxChar>(peek);
            if (next == '(') {
                isFunction = true;
                break;
            } else if (next == ' ' || next == '\t') {
                peek++;
            } else {
                break;
            }
        }

        if (isFunction) {
            accessor.SetStyle(MarkdownStyles::kCodeBlockFunction, count);
        } else {
            accessor.SetStyle(MarkdownStyles::kCodeBlockText, count);
        }
        return;
    }

    // Default: regular code text
    accessor.SetStyle(MarkdownStyles::kCodeBlockText, 1);
}

void MarkdownStyler::OnStyle(clSTCAccessor& accessor)
{
    m_states.push(MarkdownState::kDefault);
    while (accessor.CanNext() && !m_states.empty()) {
        MarkdownState current_state = m_states.top();
        wxChar ch = accessor.GetCurrentChar<wxChar>();

        switch (current_state) {
        case MarkdownState::kDefault:
            // we support up to 99 bullets.
            if (::wxIsdigit(ch) && accessor.IsAtStartOfLine()) {
                if (::wxIsdigit(accessor.GetCharAt<wxChar>(1)) && accessor.GetCharAt<wxChar>(2) == '.') {
                    accessor.SetStyle(MarkdownStyles::kNumberedListItem, 2);
                    accessor.SetStyle(MarkdownStyles::kNumberedListItemDot, 1);
                    break;
                } else if (accessor.GetCharAt<wxChar>(1) == '.') {
                    accessor.SetStyle(MarkdownStyles::kNumberedListItem, 1);
                    accessor.SetStyle(MarkdownStyles::kNumberedListItemDot, 1);
                    break;
                }
            }

            if (accessor.IsAtStartOfLine() && accessor.StartsWith({226, 157, 176}) &&
                accessor.Contains({226, 157, 177}) != wxNOT_FOUND) {
                int actor_end = accessor.Contains({226, 157, 177}, 1);
                accessor.SetStyleUntilEndOfLine(MarkdownStyles::kActor);
                accessor.SetStyle(MarkdownStyles::kDefault, 1);
                break;
            }

            if (accessor.StartsWith({226, 156, 148})) {
                accessor.SetStyle(MarkdownStyles::kCheckMarkSymbol, 3);
                break;
            }

            if (accessor.StartsWith({226, 157, 140, 239, 184, 143}) || accessor.StartsWith({226, 157, 140})) {
                accessor.SetStyle(
                    MarkdownStyles::kErrorSymbol, accessor.StartsWith({226, 157, 140, 239, 184, 143}) ? 6 : 3);
                break;
            }

            if (accessor.StartsWith({226, 159, 170})) {
                int keyboard_key_start = accessor.Contains({226, 159, 170});
                int keyboard_key_end = accessor.Contains({226, 159, 171});
                if (keyboard_key_end - keyboard_key_start == 4) {
                    accessor.SetStyle(MarkdownStyles::kKeyboardKeyBracket, 3); // ⟪ (3 positions)
                    accessor.SetStyle(MarkdownStyles::kKeyboardKey, 1);        // the letter itself
                    accessor.SetStyle(MarkdownStyles::kKeyboardKeyBracket, 3); // ⟫ (3 positions)
                    break;
                }
            }

            // not a number, run the switch statement.
            switch (ch) {
            case 'h':
                if (accessor.GetSubstr(7) == "http://") {
                    accessor.SetStyle(MarkdownStyles::kUrl, 7);
                    m_states.push(MarkdownState::kUrl);
                } else if (accessor.GetSubstr(8) == "https://") {
                    accessor.SetStyle(MarkdownStyles::kUrl, 8);
                    m_states.push(MarkdownState::kUrl);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '#':
                if (accessor.IsAtStartOfLine()) {
                    if (accessor.GetSubstr(6) == "######") {
                        accessor.SetStyle(MarkdownStyles::kHeader6, 6);
                    } else if (accessor.GetSubstr(5) == "#####") {
                        accessor.SetStyle(MarkdownStyles::kHeader5, 5);
                    } else if (accessor.GetSubstr(4) == "####") {
                        accessor.SetStyle(MarkdownStyles::kHeader4, 4);
                    } else if (accessor.GetSubstr(3) == "###") {
                        accessor.SetStyle(MarkdownStyles::kHeader3, 3);
                    } else if (accessor.GetSubstr(2) == "##") {
                        accessor.SetStyle(MarkdownStyles::kHeader2, 2);
                    } else {
                        accessor.SetStyle(MarkdownStyles::kHeader1, 1);
                    }
                    m_states.push(MarkdownState::kHeaderText);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '-':
                if (accessor.IsAtStartOfLine() && accessor.GetCharAt<wxChar>(1) == ' ') {
                    accessor.SetStyle(MarkdownStyles::kListItem, 2);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '*':
                if (accessor.GetSubstr(2) == "**" && accessor.CurrentLineContains(2, "**")) {
                    accessor.SetStyle(MarkdownStyles::kStrong2Tag, 2);
                    m_states.push(MarkdownState::kStrong2Text);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '_':
                if (accessor.GetSubstr(2) == "__" && accessor.CurrentLineContains(2, "__")) {
                    accessor.SetStyle(MarkdownStyles::kEmphasis2Tag, 2);
                    m_states.push(MarkdownState::kEmphasis2Text);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '`':
                if (accessor.GetSubstr(3) == "```") {
                    // code block
                    accessor.SetStyle(MarkdownStyles::kBacktick, 3);
                    m_currentCodeBlockLanguage.Clear();
                    m_states.push(MarkdownState::kCodeBlock3);
                    m_states.push(MarkdownState::kCodeBlockTag);
                } else if (accessor.GetSubstr(4) == "````") {
                    // code block
                    accessor.SetStyle(MarkdownStyles::kBacktick, 4);
                    m_currentCodeBlockLanguage.Clear();
                    m_states.push(MarkdownState::kCodeBlock4);
                    m_states.push(MarkdownState::kCodeBlockTag);
                } else if (accessor.GetSubstr(5) == "`````") {
                    // code block
                    accessor.SetStyle(MarkdownStyles::kBacktick, 5);
                    m_currentCodeBlockLanguage.Clear();
                    m_states.push(MarkdownState::kCodeBlock5);
                    m_states.push(MarkdownState::kCodeBlockTag);
                } else {
                    accessor.SetStyle(MarkdownStyles::kBacktick, 1);
                    m_states.push(MarkdownState::kCodeWord);
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kDefault, 1);
                break;
            }
            break;
        case MarkdownState::kUrl:
            if (IsValidUrlChar(ch)) {
                accessor.SetStyle(MarkdownStyles::kUrl, 1);
            } else {
                accessor.SetStyle(MarkdownStyles::kDefault, 1);
                m_states.pop();
            }
            break;
        case MarkdownState::kCodeBlockTag:
            switch (ch) {
            case '\n':
                accessor.SetStyle(MarkdownStyles::kCodeBlockTag, 1);
                m_states.pop();
                break;
            default:
                m_currentCodeBlockLanguage << ch;
                accessor.SetStyle(MarkdownStyles::kCodeBlockTag, 1);
                break;
            }
            break;
        case MarkdownState::kCodeBlock3:
            switch (ch) {
            case '`':
                if (accessor.IsAtStartOfLine() && accessor.GetSubstr(3) == "```") {
                    accessor.SetStyle(MarkdownStyles::kBacktick, 3);
                    m_currentCodeBlockLanguage.Clear();
                    m_states.pop();
                } else {
                    StyleCodeBlockContent(accessor, m_currentCodeBlockLanguage);
                }
                break;
            default:
                StyleCodeBlockContent(accessor, m_currentCodeBlockLanguage);
                break;
            }
            break;
        case MarkdownState::kCodeBlock4:
            switch (ch) {
            case '`':
                if (accessor.IsAtStartOfLine() && accessor.GetSubstr(4) == "````") {
                    accessor.SetStyle(MarkdownStyles::kBacktick, 4);
                    m_currentCodeBlockLanguage.Clear();
                    m_states.pop();
                } else {
                    StyleCodeBlockContent(accessor, m_currentCodeBlockLanguage);
                }
                break;
            default:
                StyleCodeBlockContent(accessor, m_currentCodeBlockLanguage);
                break;
            }
            break;
        case MarkdownState::kCodeBlock5:
            switch (ch) {
            case '`':
                if (accessor.IsAtStartOfLine() && accessor.GetSubstr(5) == "`````") {
                    accessor.SetStyle(MarkdownStyles::kBacktick, 5);
                    m_currentCodeBlockLanguage.Clear();
                    m_states.pop();
                } else {
                    StyleCodeBlockContent(accessor, m_currentCodeBlockLanguage);
                }
                break;
            default:
                StyleCodeBlockContent(accessor, m_currentCodeBlockLanguage);
                break;
            }
            break;
        case MarkdownState::kCodeBlockContent:
            switch (ch) {
            case '\n':
                accessor.SetStyle(MarkdownStyles::kCodeBlockText, 1);
                m_states.pop();
                break;
            default:
                StyleCodeBlockContent(accessor, m_currentCodeBlockLanguage);
                break;
            }
            break;
        case MarkdownState::kCodeWord:
            switch (ch) {
            case '`':
                accessor.SetStyle(MarkdownStyles::kBacktick, 1);
                m_states.pop();
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kCodeWord, 1);
                break;
            }
            break;
        case MarkdownState::kHeaderText:
            switch (ch) {
            case '`':
                // we support backticks in header text.
                if (accessor.GetCharAt<wxChar>(1) != '`') {
                    accessor.SetStyle(MarkdownStyles::kBacktick, 1);
                    m_states.push(MarkdownState::kCodeWord);
                } else {
                    accessor.SetStyle(MarkdownStyles::kHeaderText, 1);
                }
                break;
            case '\n':
                accessor.SetStyle(MarkdownStyles::kHeaderText, 1);
                m_states.pop();
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kHeaderText, 1);
                break;
            }
            break;
        case MarkdownState::kStrong2Text:
            switch (ch) {
            case '*':
                if (accessor.GetSubstr(2) == "**") {
                    accessor.SetStyle(MarkdownStyles::kStrong2Tag, 2);
                    m_states.pop();
                } else {
                    accessor.SetStyle(MarkdownStyles::kStrong2Text, 1);
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kStrong2Text, 1);
                break;
            }
            break;
        case MarkdownState::kEmphasis2Text:
            switch (ch) {
            case '_':
                if (accessor.GetSubstr(2) == "__") {
                    accessor.SetStyle(MarkdownStyles::kEmphasis2Tag, 2);
                    m_states.pop();
                } else {
                    accessor.SetStyle(MarkdownStyles::kEmphasis2Text, 1);
                }
                break;
            default:
                accessor.SetStyle(MarkdownStyles::kEmphasis2Text, 1);
                break;
            }
            break;
        }
    }
}

bool MarkdownStyler::StyleDiffPatchCommentInCodeBlock(clSTCAccessor& accessor)
{
    wxChar ch = accessor.GetCurrentChar<wxChar>();

    // Check for various comment styles
    bool isComment = false;

    // Single-line comment styles
    if (ch == '#') {
        // Hash comments: shell, python, ruby, perl, etc.
        isComment = true;
    } else if (ch == '/' && accessor.GetCharAt<wxChar>(1) == '/') {
        // C++ style comments: //, used in C, C++, Java, JavaScript, Rust, Go, etc.
        isComment = true;
    } else if (ch == '-' && accessor.GetCharAt<wxChar>(1) == '-') {
        // SQL, Lua, Haskell style comments: --
        isComment = true;
    } else if (ch == ';') {
        // Semicolon comments: Lisp, Assembly, INI files
        isComment = true;
    } else if (ch == '%') {
        // Percent comments: LaTeX, MATLAB, Erlang
        isComment = true;
    } else if (ch == '/' && accessor.GetCharAt<wxChar>(1) == '*') {
        // C-style block comments: /* ... */
        isComment = true;
    } else if (ch == '<' && accessor.GetCharAt<wxChar>(1) == '!' && accessor.GetCharAt<wxChar>(2) == '-' &&
               accessor.GetCharAt<wxChar>(3) == '-') {
        // HTML/XML comments: <!-- ... -->
        isComment = true;
    }

    // If it's a comment, style the entire line
    if (isComment) {
        accessor.SetStyleUntilEndOfLine(MarkdownStyles::kCodeBlockComment);
    }
    return isComment;
}