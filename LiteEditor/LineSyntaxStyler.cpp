#include "LineSyntaxStyler.hpp"

#include <algorithm>
#include <wx/tokenzr.h>

namespace
{
bool IsWordStart(wxUniChar ch) { return wxIsalpha(ch) || ch == '_' || ch == '$'; }
bool IsWordChar(wxUniChar ch) { return wxIsalnum(ch) || ch == '_' || ch == '$'; }
} // namespace

LineSyntaxStyler::LineSyntaxStyler(const wxString& keywords)
{
    for (const wxString& keyword : ::wxStringTokenize(keywords, " \t\r\n", wxTOKEN_STRTOK)) {
        m_keywords.insert(keyword);
    }
}

void LineSyntaxStyler::SetLanguage(const wxString& lexerName)
{
    const wxString name = lexerName.Lower();
    if (name == "python" || name == "script" || name == "cmake" || name == "makefile" || name == "ruby" ||
        name == "yaml" || name == "dockerfile" || name == "properties" || name == "tcl") {
        m_lineComments = {"#"};
        m_blockCommentStart.clear();
        m_blockCommentEnd.clear();
    } else if (name == "lua" || name == "sql") {
        m_lineComments = {"--"};
        m_blockCommentStart.clear();
        m_blockCommentEnd.clear();
    } else if (name == "php") {
        m_lineComments = {"//", "#"};
        m_blockCommentStart = "/*";
        m_blockCommentEnd = "*/";
    } else {
        // C, C++, Rust, Go, Java, JavaScript, ...
        m_lineComments = {"//"};
        m_blockCommentStart = "/*";
        m_blockCommentEnd = "*/";
    }
}

size_t LineSyntaxStyler::CommentAt(const wxString& line, size_t pos, wxString& blockEnd) const
{
    blockEnd.clear();
    if (!m_blockCommentStart.empty() && line.compare(pos, m_blockCommentStart.length(), m_blockCommentStart) == 0) {
        blockEnd = m_blockCommentEnd;
        return m_blockCommentStart.length();
    }
    for (const wxString& marker : m_lineComments) {
        if (line.compare(pos, marker.length(), marker) == 0) {
            return marker.length();
        }
    }
    return 0;
}

std::vector<LineSyntaxStyler::Part> LineSyntaxStyler::Split(const wxString& line) const
{
    std::vector<Part> parts;
    // Add a part. The parts of the same style next to each other are merged
    auto add = [&parts](size_t start, size_t length, Style style) {
        if (length == 0) {
            return;
        }
        if (!parts.empty() && parts.back().style == style && parts.back().start + parts.back().length == start) {
            parts.back().length += length;
            return;
        }
        parts.push_back({start, length, style});
    };

    const size_t size = line.length();
    size_t pos = 0;
    while (pos < size) {
        const wxUniChar ch = line[pos];

        // A comment: a line comment goes to the end of the line, a block comment to its end marker
        wxString blockEnd;
        if (const size_t markerLength = CommentAt(line, pos, blockEnd); markerLength > 0) {
            size_t end = size;
            if (!blockEnd.empty()) {
                const size_t found = line.find(blockEnd, pos + markerLength);
                end = found == wxString::npos ? size : found + blockEnd.length();
            }
            add(pos, end - pos, Style::Comment);
            pos = end;
            continue;
        }

        // A string. A single quote without a closing quote on the line is not a string (for example, a Rust lifetime)
        if (ch == '"' || ch == '\'') {
            size_t end = pos + 1;
            while (end < size && line[end] != ch) {
                end += line[end] == '\\' ? 2 : 1;
            }
            if (end < size || ch == '"') {
                end = std::min(end + 1, size);
                add(pos, end - pos, Style::String);
                pos = end;
                continue;
            }
            add(pos, 1, Style::Default);
            ++pos;
            continue;
        }

        // A number (including the hexadecimal digits and the suffixes, for example 0x1Fu or 1.5f)
        if (wxIsdigit(ch)) {
            size_t end = pos + 1;
            while (end < size && (IsWordChar(line[end]) || line[end] == '.')) {
                ++end;
            }
            add(pos, end - pos, Style::Number);
            pos = end;
            continue;
        }

        // A word: a keyword, a function (followed by '('), a class (followed by "::"), or a plain word
        if (IsWordStart(ch)) {
            size_t end = pos + 1;
            while (end < size && IsWordChar(line[end])) {
                ++end;
            }
            Style style = Style::Default;
            if (m_keywords.count(line.Mid(pos, end - pos))) {
                style = Style::Keyword;
            } else {
                size_t next = end;
                while (next < size && (line[next] == ' ' || line[next] == '\t')) {
                    ++next;
                }
                if (next < size && line[next] == '(') {
                    style = Style::Function;
                } else if (line.compare(next, 2, "::") == 0) {
                    style = Style::Class;
                }
            }
            add(pos, end - pos, style);
            pos = end;
            continue;
        }

        add(pos, 1, Style::Default);
        ++pos;
    }
    return parts;
}
