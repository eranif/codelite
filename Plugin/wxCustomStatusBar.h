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
protected:
    wxRect m_rect;
    wxString m_tooltip;
    wxCustomStatusBar* m_parent;

public:
    typedef wxSharedPtr<wxCustomStatusBarField> Ptr_t;
    typedef std::vector<wxCustomStatusBarField::Ptr_t> Vect_t;

    wxCustomStatusBarField(wxCustomStatusBar* parent)
        : m_parent(parent)
    {
    }
    virtual ~wxCustomStatusBarField() {}

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
// Bitmap field
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBarBitmapField : public wxCustomStatusBarField
{
    size_t m_width;
    wxBitmap m_bitmap;

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
// Custom status bar
//================---------------
class WXDLLIMPEXP_SDK wxCustomStatusBar : public wxStatusBar
{
    wxCustomStatusBarArt::Ptr_t m_art;
    wxCustomStatusBarField::Vect_t m_fields;
    wxString m_text;
    wxString m_lastArtNameUsedForPaint;
    wxCustomStatusBarField::Ptr_t m_mainText;
    wxTimer* m_timer;

protected:
    size_t DoGetFieldsWidth();

    // Event handlers
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackround(wxEraseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnTimer(wxTimerEvent& event);
    wxRect DoGetMainFieldRect();

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

public:
    wxCustomStatusBar(wxWindow* parent, wxWindowID id = wxID_ANY, long style = 0);
    virtual ~wxCustomStatusBar();

    void SetArt(wxCustomStatusBarArt::Ptr_t art);

    wxCustomStatusBarArt::Ptr_t GetArt() { return m_art; }

    void AddField(wxCustomStatusBarField::Ptr_t field) { m_fields.push_back(field); }

    /**
     * @brief set text in the main status bar text area
     */
    void SetText(const wxString& message, int secondsToLive = wxNOT_FOUND);

    /**
     * @brief return the main status bar area text message
     */
    const wxString& GetText() const { return m_text; }

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
