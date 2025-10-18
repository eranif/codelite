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
    clSTCAccessor(wxStyledTextCtrl* ctrl)
        : m_ctrl(ctrl)
    {
    }

    inline wxString GetSubstr(size_t count) const
    {
        int curpos = m_ctrl->GetEndStyled();
        int end = std::min(static_cast<int>(m_ctrl->GetLastPosition()), static_cast<int>(curpos + count));
        return m_ctrl->GetTextRange(curpos, end);
    }

    /// Return the current character pointed by the this object.
    inline wxChar GetCurrentChar() const
    {
        int curpos = m_ctrl->GetEndStyled();
        return m_ctrl->GetCharAt(curpos);
    }

    /// Return character from the current position and a given offset.
    /// This is basically like returning `m_buffer[m_curpos + at]`
    inline wxChar GetCharAt(size_t at) const
    {
        int curpos = m_ctrl->GetEndStyled();
        return m_ctrl->GetCharAt(m_ctrl->PositionRelative(curpos, at));
    }

    inline bool IsAtStartOfLine() const
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
    inline void SetStyle(int style, size_t count) { m_ctrl->SetStyling(count, style); }

    /// Apply style until end of line reached or end of buffer.
    inline void SetStyleUntilEndOfLine(int style)
    {
        int curpos = m_ctrl->GetEndStyled();
        int end_pos = m_ctrl->GetLineEndPosition(m_ctrl->LineFromPosition(curpos));
        m_ctrl->SetStyling(end_pos - curpos, style);
    }

    /// Check if `sv` exists in the current line.
    inline bool CurrentLineContains(size_t from, const char* str)
    {
        wxString sv{str, strlen(str)};
        wxString substr = GetSubStringUntilNewLine(from);
        if (substr.empty()) {
            return false;
        }
        return substr.find(sv) != wxString::npos;
    }

    inline bool CanNext() const { return m_ctrl->GetEndStyled() != m_ctrl->GetLastPosition(); }

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