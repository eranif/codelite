#pragma once

#include "StringUtils.h"
#include "codelite_exports.h"
#include "file_logger.h"

#include <functional>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/stc/stc.h>

#define CHECK_COND_RET_VAL(cond, what) \
    if (!(cond)) {                     \
        return what;                   \
    }

class WXDLLIMPEXP_SDK clSTCAccessor
{
public:
    clSTCAccessor(wxStyledTextCtrl* ctrl, int start_pos, int end_pos)
        : m_ctrl(ctrl)
        , m_start_pos(start_pos)
        , m_end_pos(end_pos)
    {
        m_buffer = m_ctrl->GetTextRange(m_start_pos, m_end_pos);
    }

    inline wxStringView GetSubstr(size_t count) const
    {
        CHECK_COND_RET_VAL((m_curpos + count) < m_buffer.length(), {});
        return wxStringView{m_buffer.data() + m_curpos, count};
    }

    /// Return the current character pointed by the this object.
    inline wxChar GetCurrentChar() const
    {
        CHECK_COND_RET_VAL(m_curpos < m_buffer.length(), 0);
        return m_buffer[m_curpos];
    }

    /// Return character from the current position and a given offset.
    /// This is basically like returning `m_buffer[m_curpos + at]`
    inline wxChar GetCharAt(size_t at) const
    {
        CHECK_COND_RET_VAL((m_curpos + at < m_buffer.length()), 0);
        return m_buffer[m_curpos + at];
    }

    inline bool IsAtStartOfLine() const
    {
        if (m_curpos == 0) {
            return true;
        }

        // Walk backward until we find the first non whitespace char
        int index = static_cast<int>(m_curpos - 1);
        while (index >= 0) {
            wxChar ch = m_buffer[index];
            if (ch == ' ' || ch == '\t') {
                --index;
                continue;
            } else if (ch == '\n') {
                return true;
            } else {
                return false;
            }
        }

        // if we reached here, it means we could not find any char which is not a whitespcae or newline.
        return true;
    }

    /// Apply style to the next `count` bytes. This also moves the last styled position.
    inline void SetStyle(int style, size_t count, size_t utf8_len = std::numeric_limits<size_t>::max())
    {
        if (utf8_len == std::numeric_limits<size_t>::max()) {
            utf8_len = count;
        }
        CHECK_COND_RET_VAL(m_curpos < m_buffer.length(), );
        if (m_curpos + count > m_buffer.length()) {
            count = m_buffer.length() - m_curpos;
        }
        m_ctrl->SetStyling(utf8_len, style);
        m_curpos += count;
    }

    /// Apply style until end of line reached or end of buffer.
    inline void SetStyleUntilEndOfLine(int style)
    {
        CHECK_COND_RET_VAL(m_curpos < m_buffer.length(), );
        auto where = m_buffer.find('\n', m_curpos);
        if (where == wxString::npos) {
            size_t count = m_buffer.length() - m_curpos;
            m_ctrl->SetStyling(count, style);
            m_curpos = m_buffer.size();
        } else {
            where += 1; // include the "\n"
            size_t count = where - m_curpos;
            m_ctrl->SetStyling(count, style);
            m_curpos = where;
        }
    }

    /// Check if `sv` exists in the current line.
    inline bool CurrentLineContains(size_t from, const char* str)
    {
        wxString sv{str, strlen(str)};
        wxStringView substr = GetSubStringUntilNewLine(from);
        if (substr.empty()) {
            return false;
        }
        return substr.find(sv) != wxStringView::npos;
    }

    inline bool CanNext() const { return m_curpos < m_buffer.length(); }

    /// Return the number of chars styled.
    inline size_t SizeStyled() const { return m_curpos; }

private:
    wxStringView GetSubStringUntilNewLine(size_t from)
    {
        CHECK_COND_RET_VAL(m_curpos + from < m_buffer.length(), {});
        size_t offset = m_curpos + from;
        size_t len{0};
        while (offset < m_buffer.length()) {
            if (m_buffer[offset] == '\n') {
                break;
            }
            ++len;
            ++offset;
        }
        return wxStringView{m_buffer.data() + m_curpos + from, len};
    }

    wxStyledTextCtrl* m_ctrl{nullptr};
    wxString m_buffer;
    size_t m_curpos{0};
    int m_start_pos{wxNOT_FOUND};
    int m_end_pos{wxNOT_FOUND};
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

    void StyleText();

protected:
    void OnThemChanged(wxCommandEvent& event);
    void InitInternal();

    wxStyledTextCtrl* m_ctrl{nullptr};
    std::function<void(clSTCAccessor& styles)> m_on_style_callback{nullptr};
};