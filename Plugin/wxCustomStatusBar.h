//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : wxCustomStatusBar.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef WXCUSTOMSTATUSBAR_H
#define WXCUSTOMSTATUSBAR_H

#include "clSystemSettings.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wxPNGAnimation.h"

#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/sharedptr.h>
#include <wx/statusbr.h>
#include <wx/timer.h>

class wxCustomStatusBar;
class WXDLLIMPEXP_SDK wxCustomStatusBarArt : public wxEvtHandler
{
protected:
    wxString m_name;

public:
    typedef wxSharedPtr<wxCustomStatusBarArt> Ptr_t;

public:
    wxCustomStatusBarArt(const wxString& name);
    virtual ~wxCustomStatusBarArt() {}

    virtual void DrawText(wxDC& dc, wxCoord x, wxCoord y, const wxString& text);
    virtual void DrawFieldSeparator(wxDC& dc, const wxRect& fieldRect);

    virtual wxColour GetBgColour() const;
    virtual wxColour GetPenColour() const;
    virtual wxColour GetTextColour() const;
    virtual wxColour GetSeparatorColour() const;
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
};

//================---------------
// Base field
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBarField : public wxEvtHandler
{
public:
    enum Flags {
        AUTO_WIDTH = (1 << 0),
    };

protected:
    wxRect m_rect;
    wxString m_tooltip;
    wxCustomStatusBar* m_parent;
    size_t m_flags = 0;
    size_t m_autoWidth = 0;

public:
    typedef wxSharedPtr<wxCustomStatusBarField> Ptr_t;
    typedef std::vector<wxCustomStatusBarField::Ptr_t> Vect_t;

    wxCustomStatusBarField(wxCustomStatusBar* parent)
        : m_parent(parent)
    {
    }
    virtual ~wxCustomStatusBarField() {}

    bool IsAutoWidth() const { return m_flags & AUTO_WIDTH; }
    void SetAutoWidth(bool b) { m_flags |= AUTO_WIDTH; }

    void SetAutoWidth(size_t w) { m_autoWidth = w; }
    size_t GetAutoWidth() const { return m_autoWidth; }

    /**
     * @brief render the field content
     * @param dc the device content
     * @param rect the field bounding rect
     */
    virtual void Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art) = 0;

    /**
     * @brief return the field length
     */
    virtual size_t GetWidth() const = 0;

    /**
     * @brief return true if this field requires frequest refreshes
     * e.g. animation field needs it
     */
    virtual bool IsRequireFrequentRefresh() const { return false; }

    /**
     * @brief return true if 'point' is inside the current field
     * @param point position in parent coordinates
     */
    bool HitTest(const wxPoint& point) const;

    virtual void SetTooltip(const wxString& tooltip) { this->m_tooltip = tooltip; }
    const wxString& GetTooltip() const { return m_tooltip; }
    const wxRect& GetRect() const { return m_rect; }
    void SetRect(const wxRect& rect) { this->m_rect = rect; }
    template <typename T> T* Cast() const { return dynamic_cast<T*>(const_cast<wxCustomStatusBarField*>(this)); }
};

//================---------------
// Text field
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBarFieldText : public wxCustomStatusBarField
{
    wxString m_text;
    size_t m_width;
    wxAlignment m_textAlign;

public:
    wxCustomStatusBarFieldText(wxCustomStatusBar* parent, size_t width)
        : wxCustomStatusBarField(parent)
        , m_width(width)
        , m_textAlign(wxALIGN_CENTER)
    {
    }
    virtual ~wxCustomStatusBarFieldText() {}
    virtual void Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art);
    void SetText(const wxString& text);
    const wxString& GetText() const { return m_text; }
    void SetWidth(size_t width) { this->m_width = width; }
    size_t GetWidth() const { return m_width; }
    void SetTextAlignment(wxAlignment align) { m_textAlign = align; }
};

//================---------------
// Spacer field
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBarSpacerField : public wxCustomStatusBarField
{
    wxString m_text;
    size_t m_width;

public:
    wxCustomStatusBarSpacerField(wxCustomStatusBar* parent, size_t width)
        : wxCustomStatusBarField(parent)
        , m_width(width)
    {
    }
    virtual ~wxCustomStatusBarSpacerField() {}
    virtual void Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art);
    void SetWidth(size_t width) { this->m_width = width; }
    size_t GetWidth() const { return m_width; }
};

//================---------------
// Bitmap field
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBarBitmapField : public wxCustomStatusBarField
{
    size_t m_width;
    wxBitmap m_bitmap;
    wxString m_label;

public:
    wxCustomStatusBarBitmapField(wxCustomStatusBar* parent, size_t width)
        : wxCustomStatusBarField(parent)
        , m_width(width)
    {
    }
    virtual ~wxCustomStatusBarBitmapField() {}
    virtual void Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art);
    void SetWidth(size_t width) { this->m_width = width; }
    size_t GetWidth() const { return m_width; }
    void SetBitmap(const wxBitmap& bitmap) { this->m_bitmap = bitmap; }
    const wxBitmap& GetBitmap() const { return m_bitmap; }
    void SetLabel(const wxString& label) { this->m_label = label; }
    const wxString& GetLabel() const { return m_label; }
};

//================---------------
// Animation field
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBarAnimationField : public wxCustomStatusBarField
{
    size_t m_width;
    wxPNGAnimation* m_animation;

protected:
    void OnAnimationClicked(wxMouseEvent& event);

public:
    /**
     * @brief construct animation field.
     */
    wxCustomStatusBarAnimationField(wxCustomStatusBar* parent, const wxBitmap& sprite, wxOrientation spriteOrientation,
                                    const wxSize& animSize);

    virtual ~wxCustomStatusBarAnimationField();
    virtual void Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art);
    void SetWidth(size_t width) { this->m_width = width; }
    size_t GetWidth() const { return m_width; }
    /**
     * @brief start the animation with a given refresh rate (milliseconds)
     */
    void Start(long refreshRate = 50);
    void Stop();
    /**
     * @brief is the animation running?
     */
    bool IsRunning() const { return m_animation->IsRunning(); }

    /**
     * @brief set the tooltip to the animation as well
     */
    virtual void SetTooltip(const wxString& tooltip)
    {
        this->m_tooltip = tooltip;
        if(m_animation) {
            m_animation->SetToolTip(tooltip);
        }
    }
};

//================---------------
// Custom control
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBarControlField : public wxCustomStatusBarField
{
    wxControl* m_control = nullptr;

public:
    /**
     * @brief construct animation field.
     */
    wxCustomStatusBarControlField(wxCustomStatusBar* parent, wxControl* control);

    virtual ~wxCustomStatusBarControlField();
    virtual void Render(wxDC& dc, const wxRect& rect, wxCustomStatusBarArt::Ptr_t art);

    void SetSize(const wxSize& size)
    {
        CHECK_PTR_RET(m_control);
        m_control->SetSizeHints(size);
        m_control->SetSize(size);
    }

    size_t GetWidth() const
    {
        if(m_control) {
            return m_control->GetSize().GetWidth();
        } else {
            return 0;
        }
    }

    /**
     * @brief set the tooltip for the control
     */
    virtual void SetTooltip(const wxString& tooltip)
    {
        CHECK_PTR_RET(m_control);
        m_control->SetToolTip(tooltip);
    }
};

//================---------------
// Custom status bar
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBar : public wxStatusBar
{
    wxCustomStatusBarArt::Ptr_t m_art;
    wxCustomStatusBarField::Vect_t m_fields;
    std::vector<std::pair<wxString, time_t>> m_text; // stack of messages:ttl to display
    wxString m_lastArtNameUsedForPaint;
    wxCustomStatusBarField::Ptr_t m_mainText;
    wxTimer* m_timer = nullptr;

protected:
    size_t DoGetFieldsWidth();

    // Event handlers
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnTimer(wxTimerEvent& event);
    wxRect DoGetMainFieldRect();
    void UpdateMainTextField();

public:
    /**
     * @brief animation control owned by 'field' was clicked
     */
    void AnimationClicked(wxCustomStatusBarField* field);

    void SetLastArtNameUsedForPaint(const wxString& lastArtNameUsedForPaint)
    {
        this->m_lastArtNameUsedForPaint = lastArtNameUsedForPaint;
    }
    const wxString& GetLastArtNameUsedForPaint() const { return m_lastArtNameUsedForPaint; }

    /// Add field to the status bar, returns its index
    size_t AddField(wxCustomStatusBarField::Ptr_t field)
    {
        size_t idx = m_fields.size();
        m_fields.push_back(field);
        return idx;
    }

    /// Add field to the status bar, returns its index
    size_t AddSpacer(size_t width)
    {
        size_t idx = m_fields.size();
        m_fields.push_back(wxCustomStatusBarField::Ptr_t(new wxCustomStatusBarSpacerField(this, width)));
        return idx;
    }

    /// Insert field at given position
    size_t InsertField(size_t index, wxCustomStatusBarField::Ptr_t field)
    {
        if(index >= m_fields.size()) {
            return AddField(field);
        }
        m_fields.insert(m_fields.begin() + index, field);
        return index;
    }

    /// Call this method after done adding fields
    /// The status bar uses this method to calculate the
    /// width of dynamic width fields
    void Finalize();

public:
    wxCustomStatusBar(wxWindow* parent, wxWindowID id = wxID_ANY, long style = 0);
    virtual ~wxCustomStatusBar();

    void SetArt(wxCustomStatusBarArt::Ptr_t art);

    wxCustomStatusBarArt::Ptr_t GetArt() { return m_art; }

    /**
     * @brief set text in the main status bar text area
     */
    void SetText(const wxString& message, int secondsToLive = wxNOT_FOUND);

    /**
     * @brief return the main status bar area text message
     */
    const wxString& GetText() const;

    /**
     * @brief clear the main text
     */
    void ClearText();

    /**
     * @brief return pointer to the field at given index
     * if index is out of bounds, return NULL
     */
    wxCustomStatusBarField::Ptr_t GetField(size_t index);

    /**
     * @brief remove status bar field by index
     */
    void RemoveField(size_t index);

    /**
     * @brief return the number of fields
     */
    size_t GetFieldsCount() const { return m_fields.size(); }
};
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_STATUSBAR_CLICKED, clCommandEvent);
#endif // WXCUSTOMSTATUSBAR_H
