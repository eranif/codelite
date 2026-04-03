#pragma once

#include "StringUtils.h"
#include "codelite_exports.h"
#include "file_logger.h"

#include <functional>
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
    virtual bool StartsWith(std::vector<int> chars) const = 0;
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
     * @brief Returns whether the current position matches the location of the given character sequence.
     *
     * Compares the result of Contains(chars) with the current position and returns true only when they are equal.
     *
     * @param chars std::vector<int> The character sequence to compare against.
     * @return bool True if Contains(chars) equals GetPosition(), otherwise false.
     */
    bool StartsWith(std::vector<int> chars) const override { return Contains(chars) == GetPosition(); }

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

    bool CanNext() const override { return m_ctrl->GetEndStyled() != m_ctrl->GetLastPosition(); }
    bool CanNextFromPos(size_t pos) const override { return static_cast<int>(pos) < m_ctrl->GetLastPosition(); }

    /// Check if we can peek ahead 'count' characters from the current position
    bool CanPeek(size_t count) const override
    {
        return (m_ctrl->GetEndStyled() + static_cast<int>(count)) <= m_ctrl->GetLastPosition();
    }

    bool HasReachedEnd() const override { return m_ctrl->GetEndStyled() >= m_ctrl->GetLastPosition(); }

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

class WXDLLIMPEXP_SDK clSTCContainerStylerBase : public wxEvtHandler
{
public:
    clSTCContainerStylerBase(wxStyledTextCtrl* stc);
    virtual ~clSTCContainerStylerBase();

    void SetStyleCallback(std::function<void(clSTCAccessor&)> cb) { m_on_style_callback = std::move(cb); }

    /// Initialise the subclass colours & styles.
    virtual void InitStyles() = 0;

    /// Reset the styler. Clear any inner saved states.
    virtual void Reset() = 0;

    void StyleText(bool full_document = false);

protected:
    void OnThemChanged(wxCommandEvent& event);
    void InitInternal();

    wxStyledTextCtrl* m_ctrl{nullptr};
    std::function<void(clSTCAccessor& styles)> m_on_style_callback{nullptr};
};