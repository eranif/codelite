#ifndef CLTOOLBARBUTTONBASE_H
#define CLTOOLBARBUTTONBASE_H

#include "clToolBar.h"
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/toolbar.h>

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
    wxMenu* m_menu;

public:
    enum eFlags {
        kHasMenu = (1 << 0),
        kToggleButton = (1 << 1),
        kChecked = (1 << 2),
        kDisabled = (1 << 3),
        kSeparator = (1 << 4),
        kControl = (1 << 5),
        kHidden = (1 << 6),
        kSpacer = (1 << 7),
        kStretchalbeSpace = (1 << 8),
    };

    enum eRenderFlags {
        kHover = (1 << 0),
        kPressed = (1 << 1),
    };

public:
    clToolBarButtonBase(clToolBar* parent, wxWindowID id, const wxBitmap& bmp, const wxString& label = "",
                        size_t flags = 0);
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
    void SetBmp(const wxBitmap& bmp) { this->m_bmp = bmp; }
    void SetLabel(const wxString& label) { this->m_label = label; }
    const wxBitmap& GetBmp() const { return m_bmp; }
    const wxString& GetLabel() const { return m_label; }

    /**
     * @brief does this button has a menu?
     */
    bool IsMenuButton() const { return m_flags & kHasMenu; }
    wxWindowID GetId() const { return m_id; }
    clToolBar* GetToolbar() { return m_toolbar; }
    void SetMenu(wxMenu* menu);
    wxMenu* GetMenu() const { return m_menu; }
    virtual bool Contains(const wxPoint& pt) const { return m_buttonRect.Contains(pt); }
    bool InsideMenuButton(const wxPoint& pt) const { return IsMenuButton() && (m_dropDownArrowRect.Contains(pt)); }

    void SetRenderFlags(size_t flags) { m_renderFlags = flags; }
    bool IsHover() const { return m_renderFlags & kHover; }
    bool IsPressed() const { return m_renderFlags & kPressed; }
    void SetHover(bool b)
    {
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
    void Check(bool b)
    {
        EnableFlag(kChecked, b);
        SetPressed(b);
    }
    bool IsToggle() const { return (m_flags & kToggleButton); }
    bool IsSeparator() const { return m_flags & kSeparator; }
    bool IsSpacer() const { return m_flags & kSpacer; }
    bool IsStretchableSpace() const { return m_flags & kStretchalbeSpace; }
    bool IsEnabled() const { return !(m_flags & kDisabled); }
    bool IsControl() const { return m_flags & kControl; }
    bool Enable(bool b)
    {
        EnableFlag(kDisabled, !b);
        return true;
    }
    bool IsHidden() const { return m_flags & kHidden; }
    bool IsShown() const { return !IsHidden(); }
    void Show(bool b) { EnableFlag(kHidden, !b); }

    template <typename T> T* Cast() { return dynamic_cast<T*>(this); }
};

#endif // CLTOOLBARBUTTONBASE_H
