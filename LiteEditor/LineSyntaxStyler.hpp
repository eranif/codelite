#pragma once

#include "macros.h"

#include <vector>
#include <wx/string.h>

/// A basic syntax highlighter for a single line of code, without its context (the lines before it are not known).
///
/// It splits a line into parts and gives each part a style: the keywords of the language, the functions (a word
/// followed by '('), the classes (a word followed by "::"), the strings, the numbers and the comments. A comment that
/// starts on a previous line is not known.
///
/// Example:
///
///     LineSyntaxStyler styler{"const return this"};
///     for (const auto& part : styler.Split("return this->foo(\"x\"); // done")) {
///         ... // "return": Keyword, "foo": Function, "\"x\"": String, "// done": Comment
///     }
class LineSyntaxStyler
{
public:
    enum class Style {
        Default,
        Keyword,
        Function,
        Class,
        String,
        Number,
        Comment,
    };

    struct Part {
        size_t start = 0; // in characters
        size_t length = 0;
        Style style = Style::Default;
    };

    /// `keywords`: the keywords of the language, separated by white space. The comments are C style ("//" and
    /// "/* */") until SetLanguage() is called
    explicit LineSyntaxStyler(const wxString& keywords = wxEmptyString);

    /// Set the comment syntax of the language, by the name of its CodeLite lexer (for example "c++", "python" or
    /// "lua")
    void SetLanguage(const wxString& lexerName);

    /// The parts of `line`, in order. Together they cover the whole line
    std::vector<Part> Split(const wxString& line) const;

private:
    /// The length of the comment marker that starts at `pos`, or 0. `blockEnd` is set to the end marker of a block
    /// comment, and is empty for a line comment
    size_t CommentAt(const wxString& line, size_t pos, wxString& blockEnd) const;

    wxStringSet_t m_keywords;
    std::vector<wxString> m_lineComments{"//"};
    wxString m_blockCommentStart{"/*"};
    wxString m_blockCommentEnd{"*/"};
};
