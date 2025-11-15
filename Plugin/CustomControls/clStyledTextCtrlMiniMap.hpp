///////////////////////////////////////////////////////////////////////////////
// Name:        wx/stc/minimap.h
// Purpose:     Declaration of clStyledTextCtrlMiniMap class.
// Author:      Vadim Zeitlin
// Created:     2025-10-16
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/version.h>
#if wxCHECK_VERSION(3, 3, 2)
#define wxHAS_MINIMAP 1
#include "codelite_exports.h"

#include <wx/overlay.h>
#include <wx/stc/stc.h>

// ----------------------------------------------------------------------------
// clStyledTextCtrlMiniMap: a minimap used together with wxStyledTextCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_SDK clStyledTextCtrlMiniMap : public wxStyledTextCtrl
{
public:
    // Default constructor, use Create() later.
    clStyledTextCtrlMiniMap()
        : wxStyledTextCtrl()
    {
    }

    // Create the minimap with the given parent window and associated with the
    // given editor.
    clStyledTextCtrlMiniMap(wxWindow* parent,
                            wxStyledTextCtrl* edit,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0)
    {
        Create(parent, edit, id, pos, size, style);
    }

    bool Create(wxWindow* parent,
                wxStyledTextCtrl* edit,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    virtual ~clStyledTextCtrlMiniMap();

    void SetEdit(wxStyledTextCtrl* edit);

    // Set the colours used to draw the visible zone indicator in normal state
    // and when dragging.
    //
    // Note that alpha components of the colours are also used here.
    void SetThumbColours(const wxColour& colNormal, const wxColour& colDragging);

private:
    // Check if the cached information about visible and total number of lines
    // has changed and update it and return true if it did.
    bool UpdateLineInfo();

    // Get document line at the given mouse position in the map.
    int GetDocLineAtMapPoint(const wxPoint& pos) const { return LineFromPosition(PositionFromPoint(pos)); }

    // Set the editor first visible line without triggering the matching map
    // update.
    void SetEditFirstVisibleLine(int firstLine)
    {
        m_lastSetEditFirst = firstLine;

        m_edit->SetFirstVisibleLine(firstLine);
    }

    // Set the first visible line in the map without triggering the matching
    // editor update.
    void SetMapFirstVisibleLine(int firstLine)
    {
        m_lastSetMapFirst = firstLine;

        SetFirstVisibleLine(firstLine);
    }

    // Get the map first visible line corresponding to the given editor
    // first visible line.
    int GetMapFirstFromEditFirst(int editFirst) const;

    // Scroll the editor to correspond to the current position in the map.
    void SyncEditPosition();

    // Scroll the map to correspond to the current position in the editor.
    void SyncMapPosition();

    // Helper function returning the visible line of the map corresponding to
    // the given visible line in the main editor.
    int MapVisibleFromEditVisible(int editVisible) const
    {
        auto const docLine = m_edit->DocLineFromVisible(editVisible);
        return VisibleFromDocLine(docLine);
    }

    struct ThumbInfo {
        int pos = 0;    // Position of the top of the thumb in pixels.
        int height = 0; // Height of the thumb in pixels.
    };

    // Get the current thumb position and height.
    ThumbInfo GetThumbInfo() const;

    void DrawVisibleZone(wxDC& dc);

    // Event handlers, both for the map itself and for the associated editor.
    void OnEditDestroy(wxWindowDestroyEvent& event);

    void OnEditSize(wxSizeEvent& event);

    void OnEditPaint(wxPaintEvent& event);
    void OnMapPaint(wxPaintEvent& event);

    // Show the same selection in the map as in the editor.
    void SyncSelection()
    {
        SetSelectionStart(m_edit->GetSelectionStart());
        SetSelectionEnd(m_edit->GetSelectionEnd());
    }

    void OnEditUpdate(wxStyledTextEvent& event);
    void OnMapUpdate(wxStyledTextEvent& event);

    // Return true if the visible zone indicator is currently being dragged.
    bool IsDragging() const { return m_isDragging; }

    void HandleMouseClick(wxMouseEvent& event);

    // Called when the user starts dragging at the given point.
    void StartDragging(const wxPoint& pos);

    // Called while dragging the visible zone indicator.
    void DoDrag(wxPoint pos);

    // Called when we stop dragging, either because the mouse button was
    // released or because the mouse capture was lost.
    void DoStopDragging();

    // The associated main document control.
    wxStyledTextCtrl* m_edit = nullptr;

    // Colours used for the thumb in normal state and when dragging.
    wxColour m_thumbColNormal, m_thumbColDragging;

    // Overlay showing the visible zone indicator being dragged.
    wxOverlay m_overlay;

    // The constant vertical offset between the mouse pointer and the middle of
    // the thumb while dragging. Only valid if m_isDragging is true.
    int m_dragOffset = 0;

    // Height of a single line in the map: this value is cached when creating
    // the control.
    int m_mapLineHeight = 0;

    // The values in this struct change when the window size changes and, for
    // the display line count, may also change later when wrapping is enabled
    // as Scintilla re-wraps lines during idle time.
    struct LinesInfo {
        // Number of lines visible in the map.
        int mapVisible = 0;

        // Number of lines visible in the editor.
        int editVisible = 0;

        // Last valid value for the first map line.
        int mapMax = 0;

        // Last valid value for the first editor line.
        int editMax = 0;

        // Last valid value for the thumb position (in map line units).
        int thumbMax = 0;

        LinesInfo() = default;
        LinesInfo(const LinesInfo& other) = default;
        LinesInfo& operator=(const LinesInfo& other) = default;

        bool operator==(const LinesInfo& rhs) const
        {
            return mapVisible == rhs.mapVisible && editVisible == rhs.editVisible && mapMax == rhs.mapMax &&
                   editMax == rhs.editMax && thumbMax == rhs.thumbMax;
        }

        bool operator!=(const LinesInfo& rhs) const { return !(*this == rhs); }

        wxString Dump() const;
    } m_lines;

    // Last values set by this code: we can safely ignore notifications about
    // changes that still use these values.
    //
    // -1 here means "invalid" and is used because it's guaranteed to be
    // different from any valid first line value.
    int m_lastSetEditFirst = -1;
    int m_lastSetMapFirst = -1;

    // Flag set while dragging the thumb.
    bool m_isDragging = false;

    wxDECLARE_NO_COPY_CLASS(clStyledTextCtrlMiniMap);
};
#else
#define wxHAS_MINIMAP 0
#endif
