#ifndef CLTOOLBARBUTTONBASE_H
#define CLTOOLBARBUTTONBASE_H

#include "clToolBar.h"
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/settings.h>
#include <wx/string.h>

#define CL_TOOL_BAR_X_MARGIN 5
#define CL_TOOL_BAR_Y_MARGIN 5
#ifdef __WXOSX__
#define CL_TOOL_BAR_DROPDOWN_ARROW_SIZE 5
#else
#define CL_TOOL_BAR_DROPDOWN_ARROW_SIZE 8
#endif
class WXDLLIMPEXP_SDK clToolBarButtonBase
{
protected:
    clToolBar* m_toolbar;
    wxWindowID m_id;
    wxBitmap m_bmp;
    wxString m_label;
    size_t m_flags;
    wxRect m_dropDownArrowRect;
    wxRect m_buttonRect;
    size_t m_renderFlags;

public:
    enum eFlags {
        kHasMenu = (1 << 0),
        kToggleButton = (1 << 1),
        kChecked = (1 << 2),
        kDisabled = (1 << 3),
        kSeparator = (1 << 4),
        kControl = (1 << 5),
    };

    enum eRenderFlags {
        kHover = (1 << 0),
        kPressed = (1 << 1),
    };

public:
    clToolBarButtonBase(
        clToolBar* parent, wxWindowID id, const wxBitmap& bmp, const wxString& label = "", size_t flags = 0);
    virtual ~clToolBarButtonBase();

public:
    /**
     * @brief calculate the size needed for drawing this button
     * @return
     */
    virtual wxSize CalculateSize(wxDC& dc) const = 0;

public:
    /**
     * @brief draw the button in a given wxRect
     */
    virtual void Render(wxDC& dc, const wxRect& rect);

protected:
    void EnableRenderFlag(clToolBarButtonBase::eRenderFlags f, bool b)
    {
        if(b) {
            m_renderFlags |= f;
        } else {
            m_renderFlags &= ~f;
        }
    }

    void EnableFlag(clToolBarButtonBase::eFlags f, bool b)
    {
        if(b) {
            m_flags |= f;
        } else {
            m_flags &= ~f;
        }
    }

public:
    static void FillMenuBarBgColour(wxDC& dc, const wxRect& rect);

    void SetBmp(const wxBitmap& bmp) { this->m_bmp = bmp; }
    void SetLabel(const wxString& label) { this->m_label = label; }
    const wxBitmap& GetBmp() const { return m_bmp; }
    const wxString& GetLabel() const { return m_label; }

    /**
     * @brief does this button has a menu?
     */
    bool HasMenu() const { return m_flags & kHasMenu; }
    wxWindowID GetId() const { return m_id; }
    clToolBar* GetToolbar() { return m_toolbar; }

    virtual bool Contains(const wxPoint& pt) const { return m_buttonRect.Contains(pt); }
    bool InsideMenuButton(const wxPoint& pt) const { return HasMenu() && (m_dropDownArrowRect.Contains(pt)); }

    void SetRenderFlags(size_t flags) { m_renderFlags = flags; }
    bool IsHover() const { return m_renderFlags & kHover; }
    bool IsPressed() const { return m_renderFlags & kPressed; }
    void SetHover(bool b)
    {
        ClearRenderFlags();
        EnableRenderFlag(kHover, b);
    }
    void SetPressed(bool b)
    {
        ClearRenderFlags();
        EnableRenderFlag(kPressed, b);
    }
    void ClearRenderFlags() { m_renderFlags = 0; }
    const wxRect& GetButtonRect() const { return m_buttonRect; }
    bool IsChecked() const { return (m_flags & kChecked); }
    void Check(bool b) { EnableFlag(kChecked, b); }
    bool IsToggle() const { return (m_flags & kToggleButton); }
    bool IsSeparator() const { return m_flags & kSeparator; }
    bool IsEnabled() const { return !(m_flags & kDisabled); }
    bool IsControl() const { return m_flags & kControl; }
    void Enable(bool b) { EnableFlag(kDisabled, !b); }
    template <typename T>
    T* Cast()
    {
        return dynamic_cast<T*>(this);
    }
};

#endif // CLTOOLBARBUTTONBASE_H
