#include "MarkdownStyler.hpp"

#include "ColoursAndFontsManager.h"

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
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
    {"json", {"true", "false", "null"}},
    {"yaml", {"true", "false", "null"}},
    {"yml", {"true", "false", "null"}},
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
    if (lower == "yml") {
        return "yaml";
    }
    return lower;
}

inline bool IsValidUrlChar(wxChar c)
{
    // Unreserved: ALPHA / DIGIT / "-" / "." / "_" / "~"
    if (wxIsalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')
        return true;

    // Reserved: ":" / "/" / "?" / "#" / "[" / "]" / "@"
    //           "!" / "$" / "&" / "'" / "*"
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

/// Check marks (green) and crosses (red): the style and the UTF-8 bytes of the symbol.
/// All the marks start with the byte 226 (U+26xx, U+27xx, U+2BBD) or 240 (U+1F5xx)
const std::pair<int, std::vector<int>> kMarkSymbols[] = {
    {MarkdownStyles::kCheckMarkSymbol, {226, 156, 147}},      // ✓ U+2713
    {MarkdownStyles::kCheckMarkSymbol, {226, 156, 148}},      // ✔ U+2714
    {MarkdownStyles::kCheckMarkSymbol, {226, 156, 133}},      // ✅ U+2705
    {MarkdownStyles::kCheckMarkSymbol, {226, 152, 145}},      // ☑ U+2611
    {MarkdownStyles::kCheckMarkSymbol, {240, 159, 151, 184}}, // 🗸 U+1F5F8
    {MarkdownStyles::kCheckMarkSymbol, {240, 159, 151, 185}}, // 🗹 U+1F5F9
    {MarkdownStyles::kErrorSymbol, {226, 152, 146}},          // ☒ U+2612
    {MarkdownStyles::kErrorSymbol, {226, 156, 149}},          // ✕ U+2715
    {MarkdownStyles::kErrorSymbol, {226, 156, 150}},          // ✖ U+2716
    {MarkdownStyles::kErrorSymbol, {226, 156, 151}},          // ✗ U+2717
    {MarkdownStyles::kErrorSymbol, {226, 156, 152}},          // ✘ U+2718
    {MarkdownStyles::kErrorSymbol, {226, 157, 140}},          // ❌ U+274C
    {MarkdownStyles::kErrorSymbol, {226, 157, 142}},          // ❎ U+274E
    {MarkdownStyles::kErrorSymbol, {226, 174, 189}},          // ⮽ U+2BBD
    {MarkdownStyles::kErrorSymbol, {240, 159, 151, 153}},     // 🗙 U+1F5D9
    {MarkdownStyles::kErrorSymbol, {240, 159, 151, 180}},     // 🗴 U+1F5F4
    {MarkdownStyles::kErrorSymbol, {240, 159, 151, 181}},     // 🗵 U+1F5F5
    {MarkdownStyles::kErrorSymbol, {240, 159, 151, 182}},     // 🗶 U+1F5F6
    {MarkdownStyles::kErrorSymbol, {240, 159, 151, 183}},     // 🗷 U+1F5F7
};

/// Style a check mark or a cross at the current position. A mark can be followed by the
/// variation selector U+FE0F (the bytes EF B8 8F), it gets the same style.
/// Returns false if the current position is not a mark
bool StyleMark(AccessorBase& accessor)
{
    // All the marks start with the byte 226 or 240, so most characters stop here
    const int first_byte = accessor.GetCurrentChar<int>();
    if (first_byte != 226 && first_byte != 240) {
        return false;
    }
    for (const auto& mark : kMarkSymbols) {
        if (!accessor.StartsWith(mark.second)) {
            continue;
        }
        accessor.SetStyle(mark.first, mark.second.size());
        if (accessor.StartsWith({239, 184, 143})) {
            accessor.SetStyle(mark.first, 3);
        }
        return true;
    }
    return false;
}

} // namespace

wxDEFINE_EVENT(wxEVT_MARKDOWN_LINK_CLICKED, clCommandEvent);

MarkdownStyler::MarkdownStyler(wxStyledTextCtrl* ctrl)
    : clSTCContainerStylerBase(ctrl)
{
    m_states.push(MarkdownState::kDefault);
    if (m_ctrl) {
        InitInternal(); // Will call the pure virtual method "InitStyles"
        m_ctrl->Bind(wxEVT_STC_HOTSPOT_CLICK, &MarkdownStyler::OnHostspotClicked, this);
        EnableLineStyling();
    }
}

MarkdownStyler::~MarkdownStyler()
{
    if (m_ctrl) {
        m_ctrl->Unbind(wxEVT_STC_HOTSPOT_CLICK, &MarkdownStyler::OnHostspotClicked, this);
    }
}

void MarkdownStyler::OnHostspotClicked(wxStyledTextEvent& event)
{
    wxString url = GetUrlFromPosition(event.GetPosition());
    clCommandEvent event_markdown_url{wxEVT_MARKDOWN_LINK_CLICKED};
    event_markdown_url.SetString(url);
    AddPendingEvent(event_markdown_url);
}

void MarkdownStyler::Reset() { RestoreLineState(0); }

namespace
{
// The line state: bits 0-1 are the fence of the code block (0: none, 1: ```, 2: ````, 3: `````), bits 2-4 are about
// the line itself (for the indented code blocks), and the bits from 8 are the id of the language of the code block.
// A line after text has the state 0.
constexpr int kFenceMask = 0x3;
constexpr int kLineBlank = 1 << 2;
constexpr int kLineIndentedCode = 1 << 3;
constexpr int kLineInList = 1 << 4;
constexpr int kLanguageShift = 8;
} // namespace

void MarkdownStyler::RestoreLineState(int state)
{
    m_states = {};
    m_states.push(MarkdownState::kDefault);
    m_currentCodeBlockLanguage.clear();
    m_currentLanguageId = 0;
    m_prevLineBlank = (state & kLineBlank) != 0;
    m_prevLineIndentedCode = (state & kLineIndentedCode) != 0;
    m_inList = (state & kLineInList) != 0;

    switch (state & kFenceMask) {
    case 1:
        m_states.push(MarkdownState::kCodeBlock3);
        break;
    case 2:
        m_states.push(MarkdownState::kCodeBlock4);
        break;
    case 3:
        m_states.push(MarkdownState::kCodeBlock5);
        break;
    default:
        return;
    }

    const int language_id = state >> kLanguageShift;
    if (language_id > 0 && language_id <= static_cast<int>(m_languages.size())) {
        m_currentLanguageId = language_id;
    }
}

int MarkdownStyler::SaveLineState()
{
    // Only a fenced code block continues on the next line: end all the other states
    while (!m_states.empty()) {
        const MarkdownState state = m_states.top();
        if (state == MarkdownState::kDefault || state == MarkdownState::kCodeBlock3 ||
            state == MarkdownState::kCodeBlock4 || state == MarkdownState::kCodeBlock5) {
            break;
        }
        if (state == MarkdownState::kCodeBlockTag) {
            // A fence line without an EOL (the last line of the document)
            EndCodeBlockTag();
        }
        m_states.pop();
    }

    if (m_states.empty()) {
        m_states.push(MarkdownState::kDefault);
    }

    int state = 0;
    if (m_prevLineBlank) {
        state |= kLineBlank;
    }
    if (m_prevLineIndentedCode) {
        state |= kLineIndentedCode;
    }
    if (m_inList) {
        state |= kLineInList;
    }

    int fence = 0;
    switch (m_states.top()) {
    case MarkdownState::kCodeBlock3:
        fence = 1;
        break;
    case MarkdownState::kCodeBlock4:
        fence = 2;
        break;
    case MarkdownState::kCodeBlock5:
        fence = 3;
        break;
    default:
        return state;
    }
    return state | fence | (m_currentLanguageId << kLanguageShift);
}

bool MarkdownStyler::IsInFencedCodeBlock() const
{
    if (m_states.empty()) {
        return false;
    }
    const MarkdownState state = m_states.top();
    return state == MarkdownState::kCodeBlock3 || state == MarkdownState::kCodeBlock4 ||
           state == MarkdownState::kCodeBlock5;
}

int MarkdownStyler::GetLanguageId(const wxString& tag)
{
    const wxString trimmed = tag.Strip(wxString::both);
    if (trimmed.empty()) {
        return 0;
    }

    auto iter = m_languageIds.find(trimmed);
    if (iter != m_languageIds.end()) {
        return iter->second;
    }

    CodeBlockLanguage language;
    language.name = NormalizeLanguage(trimmed);
    const wxString& name = language.name;
    auto keywords = g_languageKeywords.find(name);
    if (keywords != g_languageKeywords.end()) {
        language.keywords = &keywords->second;
    }
    language.is_plain_text = name.empty() || name == "text";
    language.is_diff = name == "diff" || name == "patch";
    language.is_rust = name == "rust";
    language.cpp_comments = name == "c++" || name == "c" || name == "java" || name == "javascript" || name == "rust" ||
                            name == "go" || name == "php";
    language.hash_comments = name == "cmake" || name == "sh" || name == "bash" || name == "zsh" || name == "docker" ||
                             name == "makefile" || name == "php" || name == "python" || name == "ruby";

    m_languages.push_back(std::move(language));
    const int id = static_cast<int>(m_languages.size());
    m_languageIds.insert({trimmed, id});
    return id;
}

const MarkdownStyler::CodeBlockLanguage& MarkdownStyler::GetCurrentLanguage() const
{
    // No tag: the code block is plain text
    static const CodeBlockLanguage kNoLanguage;
    if (m_currentLanguageId <= 0 || m_currentLanguageId > static_cast<int>(m_languages.size())) {
        return kNoLanguage;
    }
    return m_languages[m_currentLanguageId - 1];
}

void MarkdownStyler::EndCodeBlockTag()
{
    m_currentLanguageId = GetLanguageId(m_currentCodeBlockLanguage);
    m_currentCodeBlockLanguage.clear();
}

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
    code_bg = default_bg.ChangeLightness(is_dark ? 105 : 95);
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

    // The tags (` and ```, and the language of a code block) are dimmed, not hidden: the user can see and edit them.
    // The colour is halfway between the text and the background, so it works with any theme.
    auto halfway = [](unsigned char a, unsigned char b) { return static_cast<unsigned char>((a + b) / 2); };
    const wxColour dimmed_fg{halfway(default_fg.Red(), default_bg.Red()),
                             halfway(default_fg.Green(), default_bg.Green()),
                             halfway(default_fg.Blue(), default_bg.Blue())};
    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeBlockTag, dimmed_fg);
    m_ctrl->StyleSetForeground(MarkdownStyles::kBacktick, dimmed_fg);

    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kCodeBlockText, true);
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeBlockText, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kCodeWord, function.GetFgColour());
    m_ctrl->StyleSetBackground(MarkdownStyles::kCodeWord, code_bg);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeader1, header_1.GetFgColour());
    m_ctrl->StyleSetBold(MarkdownStyles::kHeader1, true);

    m_ctrl->StyleSetForeground(MarkdownStyles::kHeader2, header_2.GetFgColour());
    m_ctrl->StyleSetBold(MarkdownStyles::kHeader2, true);

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
    wxColour diff_add_bg = is_dark ? wxColour("#1E4839") : wxColour("#E6FFEC");
    wxColour diff_add_fg = is_dark ? wxColour("#B9E0D3") : wxColour("#117134");

    m_ctrl->StyleSetForeground(MarkdownStyles::kDiffAdd, diff_add_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kDiffAdd, diff_add_bg);
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kDiffAdd, true);

    wxColour diff_del_bg = is_dark ? wxColour("#4C232D") : wxColour("#FFEBE9");
    wxColour diff_del_fg = is_dark ? wxColour("#E8B9B8") : wxColour("#CF222E");

    m_ctrl->StyleSetForeground(MarkdownStyles::kDiffDelete, diff_del_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kDiffDelete, diff_del_bg);
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kDiffDelete, true);

    wxColour diff_header_fg = is_dark ? wxColour("#7EE0FF") : wxColour("#7952B3");
    m_ctrl->StyleSetForeground(MarkdownStyles::kDiffHeader, diff_header_fg);
    m_ctrl->StyleSetBackground(MarkdownStyles::kDiffHeader, code_bg);
    m_ctrl->StyleSetEOLFilled(MarkdownStyles::kDiffHeader, true);

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

    SetStyleCallback([this](AccessorBase& accessor) { this->OnStyle(accessor); });
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

    for (int i = pos - 1; i >= start_pos; --i) {
        if (m_ctrl->GetStyleAt(i) == MarkdownStyles::kUrl) {
            prefix.Prepend(static_cast<wxChar>(m_ctrl->GetCharAt(i)));
        } else {
            break;
        }
    }
    return prefix + suffix;
}

bool MarkdownStyler::IsKeyword(const wxString& word, const CodeBlockLanguage& language) const
{
    return language.keywords != nullptr && language.keywords->count(word) > 0;
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
/// Return true if the byte cannot start a markdown construct in the default state: it gets the default style.
/// Keep it in sync with the checks of the default state in MarkdownStyler::StyleRange()
bool IsPlainTextByte(int byte)
{
    switch (byte) {
    case '#':
    case '-':
    case '*':
    case '+':
    case '_':
    case '`':
    case 'h': // http:// and https://
    case 226: // the first byte of ❰ (actor), ⟪ (keyboard key) and of most check marks and crosses
    case 240: // the first byte of the other check marks and crosses
        return false;
    default:
        return byte < '0' || byte > '9'; // a numbered list item
    }
}

bool IsNotLineFeed(int byte) { return byte != '\n'; }
bool IsSpaceOrTab(int byte) { return byte == ' ' || byte == '\t'; }
bool IsBacktick(int byte) { return byte == '`'; }
bool IsWordByte(int byte) { return ::wxIsalnum(static_cast<wxChar>(byte)) || byte == '_'; }

void StyleToEndOfLine(AccessorBase& accessor, int style, bool include_lf)
{
    const size_t count = accessor.CountWhile(IsNotLineFeed);
    if (count > 0) {
        // Style the entire range in one call.
        accessor.SetStyle(style, count);
    }

    if (include_lf && accessor.CanNext() && accessor.GetCurrentChar<int>() == '\n') {
        accessor.SetStyle(style, 1);
    }
}
} // namespace

bool MarkdownStyler::StyleClosingFence(AccessorBase& accessor, std::string_view fence)
{
    if (!accessor.IsAtLineStart()) {
        return false;
    }

    // The closing fence can be indented, like the opening one (e.g. in a list item). But in a diff, a line that
    // starts with a space is a context line (e.g. a fence of a markdown file in the diff): there, the closing fence
    // must be at the start of the line
    const size_t indent = GetCurrentLanguage().is_diff ? 0 : accessor.CountWhile(IsSpaceOrTab);
    if (!accessor.Matches(fence, indent)) {
        return false;
    }

    accessor.SetStyle(MarkdownStyles::kDefault, indent);
    // The closing fence can be longer than the opening one
    accessor.SetStyle(MarkdownStyles::kBacktick, accessor.CountWhile(IsBacktick));
    m_currentCodeBlockLanguage.Clear();
    m_currentLanguageId = 0;
    m_states.pop();
    return true;
}

namespace
{
/// Return true if a list item starts at `offset`: "- ", "* ", "+ ", "1. " or "1) "
bool StartsListItem(const AccessorBase& accessor, size_t offset)
{
    auto is_blank = [](int ch) { return ch == ' ' || ch == '\t'; };
    const int ch = accessor.GetCharAt<int>(offset);
    if (ch == '-' || ch == '*' || ch == '+') {
        return is_blank(accessor.GetCharAt<int>(offset + 1));
    }

    size_t end = offset;
    while (end - offset < 9 && accessor.GetCharAt<int>(end) >= '0' && accessor.GetCharAt<int>(end) <= '9') {
        ++end;
    }
    if (end == offset) {
        return false;
    }
    const int delimiter = accessor.GetCharAt<int>(end);
    return (delimiter == '.' || delimiter == ')') && is_blank(accessor.GetCharAt<int>(end + 1));
}
} // namespace

void MarkdownStyler::StyleLine(AccessorBase& accessor)
{
    // The indentation of the line, in columns: a tab moves to the next multiple of 4
    const size_t indent = accessor.CountWhile(IsSpaceOrTab);
    size_t columns = 0;
    for (size_t i = 0; i < indent; ++i) {
        columns = accessor.GetCharAt<int>(i) == '\t' ? (columns / 4 + 1) * 4 : columns + 1;
    }
    const int first_char = accessor.GetCharAt<int>(indent);
    const bool is_blank = !accessor.CanPeek(indent + 1) || first_char == '\n' || first_char == '\r';
    const bool in_fence = IsInFencedCodeBlock();
    const bool starts_list_item = !is_blank && StartsListItem(accessor, indent);

    const bool is_indented_code = !in_fence && StyleIndentedCodeLine(accessor, columns, is_blank);
    if (!is_indented_code) {
        StyleRange(accessor);
    }

    // The flags for the next line
    if (is_blank) {
        // A blank line does not end an indented code block or a list
        m_prevLineBlank = true;
        return;
    }
    m_prevLineBlank = false;
    m_prevLineIndentedCode = is_indented_code;
    if (!in_fence && !is_indented_code) {
        // A list ends at a line that is not indented and is not a list item
        m_inList = starts_list_item || (m_inList && columns > 0);
    }
}

bool MarkdownStyler::StyleIndentedCodeLine(AccessorBase& accessor, size_t indent_columns, bool is_blank)
{
    // CommonMark: a line indented by 4 columns or more, after a blank line or after another line of the block (an
    // indented code block cannot continue a paragraph). In a list, an indented line continues the list item.
    if (is_blank || indent_columns < 4 || m_inList || !(m_prevLineBlank || m_prevLineIndentedCode)) {
        return false;
    }
    StyleToEndOfLine(accessor, MarkdownStyles::kCodeBlockText, true);
    return true;
}

void MarkdownStyler::StyleCodeBlockContent(AccessorBase& accessor, const CodeBlockLanguage& language)
{
    // If no language is provided or explicitly set to "text", don't attempt to highlight keywords/operators, etc.
    if (language.is_plain_text) {
        StyleToEndOfLine(accessor, MarkdownStyles::kCodeBlockText, true);
        return;
    }

    // Handle diff syntax specially
    if (language.is_diff) {
        wxChar ch = accessor.GetCurrentChar<wxChar>();

        // Check if we're at the start of a line for diff-specific line markers
        if (accessor.IsAtLineStart()) {
            // Lines starting with '+'
            if (ch == '+') {
                // Check for '+++' (file marker)
                if (accessor.Matches("+++")) {
                    // Style the entire line as keyword
                    StyleToEndOfLine(accessor, MarkdownStyles::kDiffHeader, true);
                } else {
                    // Style the entire line as string (additions in green)
                    StyleToEndOfLine(accessor, MarkdownStyles::kDiffAdd, true);
                }
                return;
            }
            // Lines starting with '-'
            else if (ch == '-') {
                // Check for '---' (file marker)
                if (accessor.Matches("---")) {
                    // Style the entire line as keyword
                    StyleToEndOfLine(accessor, MarkdownStyles::kDiffHeader, true);
                } else {
                    // Style the entire line as comment (deletions in muted color)
                    StyleToEndOfLine(accessor, MarkdownStyles::kDiffDelete, true);
                }
                return;
            }
            // Lines starting with '@@' or starting with 'diff', 'index', etc.
            else if ((ch == '@' && accessor.GetCharAt<wxChar>(1) == '@') || accessor.Matches("diff") ||
                     accessor.Matches("index")) {
                // Style the entire line as keyword
                StyleToEndOfLine(accessor, MarkdownStyles::kDiffHeader, true);
                return;
            } else if (ch == ' ') {
                // A context line
                if (StyleDiffPatchCommentInCodeBlock(accessor)) {
                    return;
                }
            }
        }
        StyleToEndOfLine(accessor, MarkdownStyles::kCodeBlockText, true);
        return;
    }

    // Handle C++ style comments (//) for languages that support them
    // Most C-family and many modern languages use this style
    const bool supportsCppComments = language.cpp_comments;
    const bool supportsHashComments = language.hash_comments;

    if (supportsCppComments && accessor.GetCurrentChar<wxChar>() == '/' && accessor.GetCharAt<wxChar>(1) == '/') {
        // Style the entire line as a comment
        StyleToEndOfLine(accessor, MarkdownStyles::kCodeBlockComment, false);
        return;
    }

    if (supportsHashComments && accessor.GetCurrentChar<wxChar>() == '#') {
        // Style the entire line as a comment
        StyleToEndOfLine(accessor, MarkdownStyles::kCodeBlockComment, false);
        return;
    }

    wxChar ch = accessor.GetCurrentChar<wxChar>();

    // Spaces and tabs (for example, the indentation): style them at once
    if (const size_t blank_count = accessor.CountWhile(IsSpaceOrTab); blank_count > 0) {
        accessor.SetStyle(MarkdownStyles::kCodeBlockText, blank_count);
        return;
    }

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
        // The first byte is a word byte too, so the count is at least 1
        int count = static_cast<int>(accessor.CountWhile(IsWordByte));

        // Check if it's a Rust macro (identifier followed by '!')
        if (language.is_rust) {
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

        // Check if it's a keyword. Create the word only when the language has keywords
        if (language.keywords != nullptr && IsKeyword(accessor.GetSubstr(count), language)) {
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

void MarkdownStyler::OnStyle(AccessorBase& accessor)
{
    RestoreLineState(0);
    StyleRange(accessor);
}

void MarkdownStyler::StyleRange(AccessorBase& accessor)
{
    while (accessor.CanNext() && !m_states.empty()) {
        MarkdownState current_state = m_states.top();
        wxChar ch = accessor.GetCurrentChar<wxChar>();
        const int position_before = accessor.GetPosition();

        switch (current_state) {
        case MarkdownState::kDefault:
            // Most of the text is plain: style it at once, up to the next byte that can start a construct
            if (const size_t plain_count = accessor.CountWhile(IsPlainTextByte); plain_count > 0) {
                accessor.SetStyle(MarkdownStyles::kDefault, plain_count);
                break;
            }

            // we support up to 99 bullets.
            if (::wxIsdigit(ch) && accessor.IsAtLineStartIgnoringWhitespace()) {
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

            // Check for actor: ❰ACTOR_NAME❱
            // Check the first byte before StartsWith(), which creates a vector
            if (ch == 226 && accessor.StartsWith({226, 157, 176}) && accessor.IsAtLineStartIgnoringWhitespace() &&
                accessor.Contains({226, 157, 177}) != wxNOT_FOUND) {
                accessor.SetStyleUntilEndOfLine(MarkdownStyles::kActor);
                accessor.SetStyle(MarkdownStyles::kDefault, 1);
                break;
            }

            // Check marks and crosses
            if (StyleMark(accessor)) {
                break;
            }

            // ⟪y/n/t⟫
            if (ch == 226 && accessor.StartsWith({226, 159, 170})) {
                const int keyboard_key_start = accessor.GetPosition();
                // search for the closing brace.
                int keyboard_key_end = accessor.Contains({226, 159, 171});
                if (keyboard_key_end != wxNOT_FOUND && keyboard_key_end - keyboard_key_start >= 3) {
                    accessor.SetStyle(MarkdownStyles::kKeyboardKeyBracket, 3); // ⟪ is 3 positions
                    for (int i = keyboard_key_start + 3; i < keyboard_key_end; ++i) {
                        // Anything else in between, unless it is a "/" is marked
                        // as keyboard-key
                        if (accessor.GetCurrentChar<wxChar>() == '/') {
                            accessor.SetStyle(MarkdownStyles::kKeyboardKeyBracket, 1);
                        } else {
                            accessor.SetStyle(MarkdownStyles::kKeyboardKey, 1);
                        }
                    }
                    accessor.SetStyle(MarkdownStyles::kKeyboardKeyBracket, 3); // ⟫ is 3 positions
                    break;
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 3); // Style the ⟪ as normal.
                    break;
                }
            }

            // not a number, run the switch statement.
            switch (ch) {
            case 'h':
                if (accessor.Matches("http://")) {
                    accessor.SetStyle(MarkdownStyles::kUrl, 7);
                    m_states.push(MarkdownState::kUrl);
                } else if (accessor.Matches("https://")) {
                    accessor.SetStyle(MarkdownStyles::kUrl, 8);
                    m_states.push(MarkdownState::kUrl);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '#':
                if (accessor.IsAtLineStartIgnoringWhitespace()) {
                    if (accessor.Matches("######")) {
                        accessor.SetStyle(MarkdownStyles::kHeader6, 6);
                    } else if (accessor.Matches("#####")) {
                        accessor.SetStyle(MarkdownStyles::kHeader5, 5);
                    } else if (accessor.Matches("####")) {
                        accessor.SetStyle(MarkdownStyles::kHeader4, 4);
                    } else if (accessor.Matches("###")) {
                        accessor.SetStyle(MarkdownStyles::kHeader3, 3);
                    } else if (accessor.Matches("##")) {
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
                if (accessor.IsAtLineStartIgnoringWhitespace() && IsSpaceOrTab(accessor.GetCharAt<int>(1))) {
                    accessor.SetStyle(MarkdownStyles::kListItem, 2);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '*':
                // A single '*' followed by a blank at the start of a line is a bullet list item, like '-'
                if (accessor.IsAtLineStartIgnoringWhitespace() && IsSpaceOrTab(accessor.GetCharAt<int>(1))) {
                    accessor.SetStyle(MarkdownStyles::kListItem, 2);
                } else if (accessor.Matches("**") && accessor.CurrentLineContains(2, "**")) {
                    accessor.SetStyle(MarkdownStyles::kStrong2Tag, 2);
                    m_states.push(MarkdownState::kStrong2Text);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '+':
                if (accessor.IsAtLineStartIgnoringWhitespace() && IsSpaceOrTab(accessor.GetCharAt<int>(1))) {
                    accessor.SetStyle(MarkdownStyles::kListItem, 2);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '_':
                if (accessor.Matches("__") && accessor.CurrentLineContains(2, "__")) {
                    accessor.SetStyle(MarkdownStyles::kEmphasis2Tag, 2);
                    m_states.push(MarkdownState::kEmphasis2Text);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
                }
                break;
            case '`':
                // A fence opens a code block only at the start of a line (after spaces, e.g. in a list item). The
                // longest fence first: "````" also starts with "```"
                if (accessor.IsAtLineStartIgnoringWhitespace() && accessor.Matches("`````")) {
                    // code block
                    accessor.SetStyle(MarkdownStyles::kBacktick, 5);
                    m_currentCodeBlockLanguage.Clear();
                    m_currentLanguageId = 0;
                    m_states.push(MarkdownState::kCodeBlock5);
                    m_states.push(MarkdownState::kCodeBlockTag);
                } else if (accessor.IsAtLineStartIgnoringWhitespace() && accessor.Matches("````")) {
                    // code block
                    accessor.SetStyle(MarkdownStyles::kBacktick, 4);
                    m_currentCodeBlockLanguage.Clear();
                    m_currentLanguageId = 0;
                    m_states.push(MarkdownState::kCodeBlock4);
                    m_states.push(MarkdownState::kCodeBlockTag);
                } else if (accessor.IsAtLineStartIgnoringWhitespace() && accessor.Matches("```")) {
                    // code block
                    accessor.SetStyle(MarkdownStyles::kBacktick, 3);
                    m_currentCodeBlockLanguage.Clear();
                    m_currentLanguageId = 0;
                    m_states.push(MarkdownState::kCodeBlock3);
                    m_states.push(MarkdownState::kCodeBlockTag);
                } else if (accessor.CurrentLineContains(1, "`")) {
                    // An inline code span ends on the same line
                    accessor.SetStyle(MarkdownStyles::kBacktick, 1);
                    m_states.push(MarkdownState::kCodeWord);
                } else {
                    accessor.SetStyle(MarkdownStyles::kDefault, 1);
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
                EndCodeBlockTag();
                m_states.pop();
                break;
            default:
                m_currentCodeBlockLanguage << ch;
                accessor.SetStyle(MarkdownStyles::kCodeBlockTag, 1);
                break;
            }
            break;
        case MarkdownState::kCodeBlock3:
            if (!StyleClosingFence(accessor, "```")) {
                StyleCodeBlockContent(accessor, GetCurrentLanguage());
            }
            break;
        case MarkdownState::kCodeBlock4:
            if (!StyleClosingFence(accessor, "````")) {
                StyleCodeBlockContent(accessor, GetCurrentLanguage());
            }
            break;
        case MarkdownState::kCodeBlock5:
            if (!StyleClosingFence(accessor, "`````")) {
                StyleCodeBlockContent(accessor, GetCurrentLanguage());
            }
            break;
        case MarkdownState::kCodeBlockContent:
            switch (ch) {
            case '\n':
                accessor.SetStyle(MarkdownStyles::kCodeBlockText, 1);
                m_states.pop();
                break;
            default:
                StyleCodeBlockContent(accessor, GetCurrentLanguage());
                break;
            }
            break;
        case MarkdownState::kCodeWord:
            switch (ch) {
            case '`':
                accessor.SetStyle(MarkdownStyles::kBacktick, 1);
                m_states.pop();
                break;
            case '\n':
                // An inline code span does not continue on the next line
                accessor.SetStyle(MarkdownStyles::kDefault, 1);
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
                if (accessor.GetCharAt<wxChar>(1) != '`' && accessor.CurrentLineContains(1, "`")) {
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
                if (accessor.Matches("**")) {
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
                if (accessor.Matches("__")) {
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

        if (accessor.GetPosition() == position_before) {
            // Each step must style at least one byte, or the loop never ends
            accessor.SetStyle(MarkdownStyles::kDefault, 1);
        }
    }
}

bool MarkdownStyler::StyleDiffPatchCommentInCodeBlock(AccessorBase& accessor)
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