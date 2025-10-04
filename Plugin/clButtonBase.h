#ifndef CLBUTTONBASE_H
#define CLBUTTONBASE_H

#include "clColours.h"
#include "codelite_exports.h"
#include "drawingutils.h"
#include "wxCustomControls.hpp"

#include <wx/bitmap.h>
#include <wx/control.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/validate.h>

#if wxUSE_NATIVE_BUTTON
#include <wx/button.h>
#define BUTTON_BASE wxButton
#else
#define BUTTON_BASE wxControl
#endif

class WXDLLIMPEXP_SDK clButtonBase : public BUTTON_BASE
{
protected:
    bool m_hasDropDownMenu = false;
    wxString m_subText;
#if !wxUSE_NATIVE_BUTTON
protected:
    size_t m_buttonStyle = 0;
    clColours m_colours;
    wxString m_text;
    eButtonState m_state = eButtonState::kNormal;

    enum eDrawingFlags{
        kDrawingFlagEnabled = (1 << 0),
        kDrawingFlagChecked = (1 << 1),
    };
    size_t m_lastPaintFlags = 0;
    wxBitmap m_bitmap;

protected:
    void BindEvents();
    void UnBindEvents();
    void Initialise();
    wxSize GetBestSize() const;
    void PostClickEvent();
    size_t GetDrawingFlags() const;

protected:
    // Event handlers
    void OnPaint(wxPaintEvent& event);
    void OnErasebg(wxEraseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnEnter(wxMouseEvent& event);
    void OnLeave(wxMouseEvent& event);
    void OnFocus(wxFocusEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnSize(wxSizeEvent& event);

    // Overridable
    virtual void Render(wxDC& dc);

    bool IsPressed() const { return m_state == eButtonState::kPressed; }
    bool IsHover() const { return m_state == eButtonState::kHover; }
    bool IsNormal() const { return m_state == eButtonState::kNormal; }
    void SetPressed() { m_state = eButtonState::kPressed; }
    void SetNormal() { m_state = eButtonState::kNormal; }
    void SetHover() { m_state = eButtonState::kHover; }
#endif

public:
    clButtonBase() = default;
    clButtonBase(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                 const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
    virtual ~clButtonBase();

#if !wxUSE_NATIVE_BUTTON
    void SetColours(const clColours& colours);
    const clColours& GetColours() const { return m_colours; }
    const wxString& GetText() const { return m_text; }
    void SetDefault();
    void SetBitmap(const wxBitmap& bmp);
    const wxBitmap& GetBitmap() const;
#else
    void SetColours(const clColours& WXUNUSED(colour)) {}
    void SetPressed() {}
    void SetNormal() {}
    void SetHover() {}
#endif

    void SetSubText(const wxString& subText);
    const wxString& GetSubText() const { return m_subText; }
    void SetText(const wxString& text);
    void SetHasDropDownMenu(bool hasDropDownMenu);
    bool HasDropDownMenu() const { return m_hasDropDownMenu; }
    /**
     * @brief display a menu for the user aligned to the button
     * @param menu
     */
    void ShowMenu(wxMenu& menu, wxPoint* point = nullptr);
};

#endif // CLBUTTONBASE_H
