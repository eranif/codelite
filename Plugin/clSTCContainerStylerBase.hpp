#pragma once

#include "StringUtils.h"
#include "codelite_exports.h"
#include "file_logger.h"

#include <algorithm>
#include <functional>
#include <string_view>
#include <vector>
#include <wx/event.h>
#include <wx/stc/stc.h>

#define CHECK_COND_RET_VAL(cond, what) \
    if (!(cond)) {                     \
        return what;                   \
    }

class WXDLLIMPEXP_SDK AccessorBase
{
public:
    virtual ~AccessorBase() = default;

    virtual wxString GetSubstr(size_t count) const = 0;
    virtual bool StartsWith(const std::vector<int>& chars) const = 0;
    virtual int Contains(std::vector<int> chars, size_t from = 0) const = 0;

    template <typename T>
    T GetCurrentChar() const
    {
        return static_cast<T>(GetCurrentCharAsInt());
    }

    template <typename T>
    T GetCharAt(size_t at) const
    {
        return static_cast<T>(GetCharAtAsInt(at));
    }

    virtual bool IsEofInNSteps(size_t n = 0) const = 0;
    virtual int GetPosition() const = 0;
    virtual wxString GetWordAtCurrentPosition() const = 0;
    virtual bool IsAtLineStart() const = 0;
    virtual bool IsAtLineStartIgnoringWhitespace() const = 0;
    virtual void SetStyle(int style, size_t count) = 0;
    virtual void SetStyleUntilEndOfLine(int style) = 0;
    virtual bool CurrentLineContains(size_t from, const char* str) = 0;
    virtual bool CurrentLineContains(size_t from, const wxString& str) = 0;
    virtual bool CanNext() const = 0;
    virtual bool CanNextFromPos(size_t pos) const = 0;
    virtual bool CanPeek(size_t count) const = 0;

    virtual bool HasReachedEnd() const = 0;

    /// Return true if the bytes at the current position + `offset` are `str` (ASCII). Unlike `GetSubstr(n) == str`, it
    /// does not create a string. A subclass can do it faster.
    virtual bool Matches(std::string_view str, size_t offset = 0) const
    {
        for (size_t i = 0; i < str.size(); ++i) {
            if (GetCharAtAsInt(offset + i) != static_cast<unsigned char>(str[i])) {
                return false;
            }
        }
        return true;
    }

    /// Return the number of bytes, from the current position, for which `pred(byte)` is true. It stops at the end of
    /// the text. A subclass can do it faster.
    virtual size_t CountWhile(bool (*pred)(int)) const
    {
        size_t count = 0;
        while (CanPeek(count + 1) && pred(GetCharAtAsInt(count))) {
            ++count;
        }
        return count;
    }

protected:
    virtual int GetCurrentCharAsInt() const = 0;
    virtual int GetCharAtAsInt(size_t at) const = 0;
};

class WXDLLIMPEXP_SDK clSTCAccessor : public AccessorBase
{
public:
    clSTCAccessor(wxStyledTextCtrl* ctrl)
        : m_ctrl(ctrl)
    {
    }

    wxString GetSubstr(size_t count) const override
    {
        int curpos = m_ctrl->GetEndStyled();
        int end = std::min(static_cast<int>(m_ctrl->GetLastPosition()), static_cast<int>(curpos + count));
        return m_ctrl->GetTextRange(curpos, end);
    }

    /// Return the current character pointed by the this object.
    int GetCurrentCharAsInt() const override
    {
        int curpos = m_ctrl->GetEndStyled();
        return m_ctrl->GetCharAt(curpos);
    }

    /**
     * @brief Returns whether the bytes at the current position equal the given character sequence.
     *
     * Checks only the next `chars.size()` bytes. This is called for every character of the document, so it
     * must not scan the rest of the document (Contains() does).
     *
     * @param chars std::vector<int> The character sequence to compare against.
     * @return bool True if the next `chars.size()` bytes equal `chars`, otherwise false.
     */
    bool StartsWith(const std::vector<int>& chars) const override
    {
        if (chars.empty() || !CanPeek(chars.size())) {
            return false;
        }
        const int curpos = GetPosition();
        for (size_t i = 0; i < chars.size(); ++i) {
            if (m_ctrl->GetCharAt(curpos + static_cast<int>(i)) != chars[i]) {
                return false;
            }
        }
        return true;
    }

    /// Starting from `from` check if the `chars` sequence exists
    int Contains(std::vector<int> chars, size_t from = 0) const override
    {
        const size_t start_pos = static_cast<size_t>(m_ctrl->GetEndStyled()) + from;
        const size_t last_pos = static_cast<size_t>(m_ctrl->GetLastPosition());

        if (chars.empty() || start_pos >= last_pos) {
            return wxNOT_FOUND;
        }

        for (size_t pos = start_pos; pos + chars.size() <= last_pos; ++pos) {
            bool matches = true;
            for (size_t i = 0; i < chars.size(); ++i) {
                if (m_ctrl->GetCharAt(static_cast<int>(pos + i)) != chars[i]) {
                    matches = false;
                    break;
                }
            }

            if (matches) {
                return static_cast<int>(pos);
            }
        }

        return wxNOT_FOUND;
    }

    /// Return character from the current position and a given offset.
    /// This is basically like returning `m_buffer[m_curpos + at]`
    int GetCharAtAsInt(size_t at) const override
    {
        int curpos = m_ctrl->GetEndStyled();
        return m_ctrl->GetCharAt(m_ctrl->PositionRelative(curpos, at));
    }

    /// Return if EOF will be reached in `n` steps.
    /// If `n` == 0 we return if the current position is at EOF.
    /// Otherwise, we add `n` to the current position and check.
    bool IsEofInNSteps(size_t n = 0) const override
    {
        int curpos = m_ctrl->GetEndStyled() + n;
        return curpos > m_ctrl->GetLastPosition();
    }

    /// Return the current position
    int GetPosition() const override { return m_ctrl->GetEndStyled(); }

    /// Return the word at the current position
    wxString GetWordAtCurrentPosition() const override
    {
        int curpos = m_ctrl->GetEndStyled();
        if (curpos < 0 || curpos > m_ctrl->GetLastPosition()) {
            return wxEmptyString;
        }

        // Find the start of the word
        int start = m_ctrl->WordStartPosition(curpos, true);

        // Find the end of the word
        int end = m_ctrl->WordEndPosition(curpos, true);

        // Extract and return the word
        if (start >= end) {
            return wxEmptyString;
        }
        return m_ctrl->GetTextRange(start, end);
    }

    /// Returns true if the current caret position is at the start of a line.
    ///
    /// Checks the styled-end position in the associated control and determines whether
    /// it is either at the beginning of the document or immediately preceded by a
    /// newline character. This method does not modify any state.
    ///
    /// @return bool True if the current position is at line start; otherwise false.
    bool IsAtLineStart() const override
    {
        int curpos = m_ctrl->GetEndStyled();
        if (curpos == 0) {
            return true;
        }
        int pos_before = m_ctrl->PositionBefore(curpos);
        return m_ctrl->GetCharAt(pos_before) == '\n';
    }

    /**
     * @brief Determines whether the current caret position is at the start of a line (ignoring leading whitespace).
     *
     * The function obtains the current styled end position, then walks backwards
     * over any spaces or tabs until it encounters a non-whitespace character,
     * a newline, or the beginning of the document.  It returns {@code true}
     * if the caret is positioned at the start of a line, and {@code false}
     * otherwise.
     *
     * @return {@code true} if the caret is at the start of a line, {@code false} otherwise.
     */
    bool IsAtLineStartIgnoringWhitespace() const override
    {
        int curpos = m_ctrl->GetEndStyled();
        if (curpos == 0) {
            return true;
        }

        // Walk backward until we find the first non whitespace char
        int start_of_line = m_ctrl->PositionFromLine(m_ctrl->LineFromPosition(curpos));
        int index = m_ctrl->PositionBefore(curpos);
        while (index >= start_of_line) {
            int ch = m_ctrl->GetCharAt(index);
            if (ch == ' ' || ch == '\t') {
                if (index == 0) {
                    break;
                }
                index = m_ctrl->PositionBefore(index);
                continue;
            } else if (ch == '\n') {
                return true;
            } else {
                return false;
            }
        }

        // if we reached here, it means we could not find any char which is not a whitespace or newline.
        return true;
    }

    /// Apply style to the next `count` bytes. This also moves the last styled position.
    void SetStyle(int style, size_t count) override { m_ctrl->SetStyling(count, style); }

    /// Apply style until end of line reached or end of buffer.
    void SetStyleUntilEndOfLine(int style) override
    {
        int curpos = m_ctrl->GetEndStyled();
        int end_pos = m_ctrl->GetLineEndPosition(m_ctrl->LineFromPosition(curpos));
        m_ctrl->SetStyling(end_pos - curpos, style);
    }

    /// Check if `sv` exists in the current line.
    bool CurrentLineContains(size_t from, const char* str) override
    {
        wxString sv(str, wxConvUTF8, strlen(str));
        wxString substr = GetSubStringUntilNewLine(from);
        if (substr.empty()) {
            return false;
        }
        return substr.find(sv) != wxString::npos;
    }

    /// Check if `sv` exists in the current line.
    bool CurrentLineContains(size_t from, const wxString& str) override
    {
        wxString substr = GetSubStringUntilNewLine(from);
        if (substr.empty()) {
            return false;
        }
        return substr.find(str) != wxString::npos;
    }

    bool CanNextFromPos(size_t pos) const override { return static_cast<int>(pos) < m_ctrl->GetLastPosition(); }

    /// Check if we can peek ahead 'count' characters from the current position
    bool CanPeek(size_t count) const override
    {
        return (m_ctrl->GetEndStyled() + static_cast<int>(count)) <= m_ctrl->GetLastPosition();
    }

    bool HasReachedEnd() const override { return m_ctrl->GetEndStyled() >= m_ctrl->GetLastPosition(); }
    bool CanNext() const override { return !HasReachedEnd(); }

    /// Byte positions: GetCharAtAsInt() counts in characters, but SetStyle() counts in bytes
    bool Matches(std::string_view str, size_t offset = 0) const override
    {
        if (!CanPeek(offset + str.size())) {
            return false;
        }
        const int curpos = GetPosition() + static_cast<int>(offset);
        for (size_t i = 0; i < str.size(); ++i) {
            if (static_cast<unsigned char>(m_ctrl->GetCharAt(curpos + static_cast<int>(i))) !=
                static_cast<unsigned char>(str[i])) {
                return false;
            }
        }
        return true;
    }

    size_t CountWhile(bool (*pred)(int)) const override
    {
        const int curpos = GetPosition();
        const int last_pos = m_ctrl->GetLastPosition();
        int pos = curpos;
        while (pos < last_pos && pred(static_cast<unsigned char>(m_ctrl->GetCharAt(pos)))) {
            ++pos;
        }
        return static_cast<size_t>(pos - curpos);
    }

private:
    wxString GetSubStringUntilNewLine(size_t from)
    {
        int start_pos =
            std::min(static_cast<int>(m_ctrl->GetEndStyled() + from), static_cast<int>(m_ctrl->GetLastPosition()));
        int end_pos = m_ctrl->GetLineEndPosition(m_ctrl->LineFromPosition(start_pos));
        return m_ctrl->GetTextRange(start_pos, end_pos);
    }

    wxStyledTextCtrl* m_ctrl{nullptr};
};

/// An accessor for one line of text, held in memory as UTF-8 bytes.
///
/// It reads the text that was copied from the control once, and collects the styles in a vector, so the styler
/// does not call the control for each byte. All the positions it accepts are byte offsets. It never reads or
/// styles past the end of the line: the style of a line depends only on its text and on the state at its start.
class WXDLLIMPEXP_SDK clSTCLineAccessor : public AccessorBase
{
public:
    /// `line` is the text of the line, with its EOL. `doc_offset` is the document position of its first byte.
    /// The styles are added to `styles`, one for each byte.
    clSTCLineAccessor(std::string_view line, int doc_offset, std::vector<char>& styles)
        : m_line(line)
        , m_offset(doc_offset)
        , m_styles(styles)
    {
        while (m_firstNonBlank < m_line.size() && (m_line[m_firstNonBlank] == ' ' || m_line[m_firstNonBlank] == '\t')) {
            ++m_firstNonBlank;
        }
    }

    wxString GetSubstr(size_t count) const override
    {
        return wxString::FromUTF8(m_line.data() + m_pos, std::min(count, m_line.size() - m_pos));
    }

    bool StartsWith(const std::vector<int>& chars) const override
    {
        if (chars.empty() || !CanPeek(chars.size())) {
            return false;
        }
        for (size_t i = 0; i < chars.size(); ++i) {
            if (ByteAt(m_pos + i) != chars[i]) {
                return false;
            }
        }
        return true;
    }

    /// Search `chars` from the current position + `from`, until the end of the line. Returns its document position,
    /// or wxNOT_FOUND.
    int Contains(std::vector<int> chars, size_t from = 0) const override
    {
        if (chars.empty()) {
            return wxNOT_FOUND;
        }
        for (size_t pos = m_pos + from; pos + chars.size() <= m_line.size(); ++pos) {
            bool matches = true;
            for (size_t i = 0; i < chars.size(); ++i) {
                if (ByteAt(pos + i) != chars[i]) {
                    matches = false;
                    break;
                }
            }
            if (matches) {
                return m_offset + static_cast<int>(pos);
            }
        }
        return wxNOT_FOUND;
    }

    bool IsEofInNSteps(size_t n = 0) const override { return m_pos + n > m_line.size(); }
    int GetPosition() const override { return m_offset + static_cast<int>(m_pos); }

    wxString GetWordAtCurrentPosition() const override
    {
        auto is_word_char = [](char ch) { return wxIsalnum(static_cast<unsigned char>(ch)) || ch == '_'; };
        size_t start = m_pos;
        while (start > 0 && is_word_char(m_line[start - 1])) {
            --start;
        }
        size_t end = m_pos;
        while (end < m_line.size() && is_word_char(m_line[end])) {
            ++end;
        }
        return wxString::FromUTF8(m_line.data() + start, end - start);
    }

    /// The accessor always starts at the start of a line
    bool IsAtLineStart() const override { return m_pos == 0; }
    bool IsAtLineStartIgnoringWhitespace() const override { return m_pos <= m_firstNonBlank; }

    void SetStyle(int style, size_t count) override
    {
        const size_t n = std::min(count, m_line.size() - m_pos);
        m_styles.insert(m_styles.end(), n, static_cast<char>(style));
        m_pos += n;
    }

    /// Style until the EOL (not included), like wxStyledTextCtrl::GetLineEndPosition()
    void SetStyleUntilEndOfLine(int style) override
    {
        size_t end = m_pos;
        while (end < m_line.size() && m_line[end] != '\n' && m_line[end] != '\r') {
            ++end;
        }
        SetStyle(style, end - m_pos);
    }

    bool CurrentLineContains(size_t from, const char* str) override
    {
        const size_t start = std::min(m_pos + from, m_line.size());
        return m_line.substr(start).find(str) != std::string_view::npos;
    }

    bool CurrentLineContains(size_t from, const wxString& str) override
    {
        const auto utf8 = str.utf8_str();
        return CurrentLineContains(from, utf8.data());
    }

    bool CanNext() const override { return m_pos < m_line.size(); }
    bool CanNextFromPos(size_t pos) const override
    {
        return static_cast<int>(pos) < m_offset + static_cast<int>(m_line.size());
    }
    bool CanPeek(size_t count) const override { return m_pos + count <= m_line.size(); }
    bool HasReachedEnd() const override { return m_pos >= m_line.size(); }

    bool Matches(std::string_view str, size_t offset = 0) const override
    {
        const size_t start = m_pos + offset;
        return start <= m_line.size() && m_line.compare(start, str.size(), str) == 0;
    }

    size_t CountWhile(bool (*pred)(int)) const override
    {
        size_t end = m_pos;
        while (end < m_line.size() && pred(static_cast<unsigned char>(m_line[end]))) {
            ++end;
        }
        return end - m_pos;
    }

protected:
    int GetCurrentCharAsInt() const override { return CanNext() ? ByteAt(m_pos) : 0; }
    int GetCharAtAsInt(size_t at) const override { return (m_pos + at < m_line.size()) ? ByteAt(m_pos + at) : 0; }

private:
    int ByteAt(size_t i) const { return static_cast<unsigned char>(m_line[i]); }

    std::string_view m_line;
    int m_offset{0};
    size_t m_pos{0};
    size_t m_firstNonBlank{0}; // the first byte that is not a space or a tab
    std::vector<char>& m_styles;
};

class WXDLLIMPEXP_SDK clSTCContainerStylerBase : public wxEvtHandler
{
public:
    clSTCContainerStylerBase(wxStyledTextCtrl* stc);
    ~clSTCContainerStylerBase() override;

    void SetStyleCallback(std::function<void(AccessorBase&)> cb) { m_on_style_callback = std::move(cb); }

    /// Initialise the subclass colours & styles.
    virtual void InitStyles() = 0;

    /// Reset the styler. Clear any inner saved states.
    virtual void Reset() = 0;

    void StyleText(bool full_document = false);

    /// Line by line styling: style `text`, which starts at the start of a line and holds whole lines, as UTF-8 bytes.
    /// `start_state` is the state at the end of the line before the text (0 for the first line of the document).
    /// `on_line_end(line_index, state)` is called with the state at the end of each line of `text`.
    /// Returns the styles, one for each byte of `text`. It does not use the control.
    std::vector<char> StyleLines(std::string_view text,
                                 int doc_offset,
                                 int start_state,
                                 const std::function<void(size_t, int)>& on_line_end);

protected:
    /// Line by line styling, see EnableLineStyling().
    /// Set the state of the styler from a value that SaveLineState() returned.
    virtual void RestoreLineState(int state) { wxUnusedVar(state); }
    /// Style one line: the accessor holds only this line
    virtual void StyleLine(AccessorBase& accessor) { wxUnusedVar(accessor); }
    /// Called at the end of each line: return the state that the next line starts with. Scintilla keeps it for each
    /// line, so styling can restart at the start of any line.
    virtual int SaveLineState() { return 0; }

    /// Style the text when Scintilla asks for it (wxEVT_STC_STYLENEEDED), line by line, with the methods above.
    /// Only the lines from the first one that is not styled up to the requested position are styled. This is what
    /// makes the styler work while the user types, or while text is appended to the control.
    void EnableLineStyling();
    void OnStyleNeeded(wxStyledTextEvent& event);

    void OnThemChanged(wxCommandEvent& event);
    void InitInternal();

    wxStyledTextCtrl* m_ctrl{nullptr};
    std::function<void(AccessorBase& styles)> m_on_style_callback{nullptr};
    bool m_lineStylingEnabled{false};
};