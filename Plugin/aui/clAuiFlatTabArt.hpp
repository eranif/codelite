#pragma once

#include "codelite_exports.h"

#include <wx/aui/aui.h>
#include <wx/aui/auibook.h>
#include <wx/aui/tabart.h>
#include <wx/dc.h>

// Base, still abstract, class for the concrete tab art classes below.
class WXDLLIMPEXP_SDK clAuiTabArtBase : public wxAuiTabArt
{
public:
    void SetFlags(unsigned int flags) override;

    void SetSizingInfo(const wxSize& tabCtrlSize, size_t tabCount, wxWindow* wnd = nullptr) override;

    void SetNormalFont(const wxFont& font) override;
    void SetSelectedFont(const wxFont& font) override;
    void SetMeasuringFont(const wxFont& font) override;

    wxFont GetNormalFont() const override;
    wxFont GetSelectedFont() const override;

    int GetButtonRect(wxReadOnlyDC& dc,
                      wxWindow* wnd,
                      const wxRect& inRect,
                      int bitmapId,
                      int buttonState,
                      int orientation,
                      wxRect* outRect) override;

    void DrawButton(wxDC& dc,
                    wxWindow* wnd,
                    const wxRect& inRect,
                    int bitmapId,
                    int buttonState,
                    int orientation,
                    wxRect* outRect) override;

    int ShowDropDown(wxWindow* wnd, const wxAuiNotebookPageArray& items, int activeIdx) override;

    int GetBorderWidth(wxWindow* wnd) override;

    int GetAdditionalBorderSpace(wxWindow* wnd) override;

    void DrawBorder(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;

protected:
    // Ctor is protected, this class is only used as a base class.
    //
    // Remember to call InitBitmaps() after setting up the colours in the
    // derived class ctor.
    clAuiTabArtBase();

    // Initialize the bitmaps using the colours returned by GetButtonColour().
    void InitBitmaps();

    // Return pointer to our bitmap bundle corresponding to the button ID and
    // state or null if we don't support this button or if it is hidden.
    const wxBitmapBundle* GetButtonBitmapBundle(const wxAuiTabContainerButton& button) const;

    // Helper function for DrawButton() and GetButtonRect().
    bool DoGetButtonRectAndBitmap(wxWindow* wnd,
                                  const wxRect& inRect,
                                  int bitmapId,
                                  int buttonState,
                                  int orientation,
                                  wxRect* outRect,
                                  wxBitmap* outBitmap = nullptr);

    // Note: all these fields are protected for compatibility reasons, but
    // shouldn't be accessed directly.
    wxFont m_normalFont;
    wxFont m_selectedFont;
    wxFont m_measuringFont;

    wxBitmapBundle m_activeCloseBmp;
    wxBitmapBundle m_disabledCloseBmp;
    wxBitmapBundle m_activeLeftBmp;
    wxBitmapBundle m_disabledLeftBmp;
    wxBitmapBundle m_activeRightBmp;
    wxBitmapBundle m_disabledRightBmp;
    wxBitmapBundle m_activeWindowListBmp;
    wxBitmapBundle m_disabledWindowListBmp;
    wxBitmapBundle m_activePinBmp;
    wxBitmapBundle m_disabledPinBmp;
    wxBitmapBundle m_activeUnpinBmp;
    wxBitmapBundle m_disabledUnpinBmp;

    int m_fixedTabWidth;
    int m_tabCtrlHeight; // Unused, kept only for compatibility.
    unsigned int m_flags{0};

private:
    // This is called by InitBitmaps().
    //
    // The state parameter is currently always either wxAUI_BUTTON_STATE_NORMAL
    // or wxAUI_BUTTON_STATE_DISABLED, but the function could be called with
    // other values in the future.
    virtual wxColour GetButtonColour(wxAuiButtonId button, wxAuiPaneButtonState state) const = 0;

    // This is called by DrawButton().
    //
    // By default just draws the bitmap using wxDC::DrawBitmap().
    virtual void DrawButtonBitmap(wxDC& dc, const wxRect& rect, const wxBitmap& bmp, int buttonState);
};

// This tab art provider draws flat tabs with a thin border.
class WXDLLIMPEXP_SDK clAuiFlatTabArt : public clAuiTabArtBase
{
public:
    clAuiFlatTabArt();
    virtual ~clAuiFlatTabArt();

    // Objects of this class are supposed to be used polymorphically, so
    // copying them is not allowed, use Clone() instead.
    clAuiFlatTabArt(const clAuiFlatTabArt&) = delete;
    clAuiFlatTabArt& operator=(const clAuiFlatTabArt&) = delete;

    wxAuiTabArt* Clone() override;

    void SetColour(const wxColour& colour) override;
    void SetActiveColour(const wxColour& colour) override;
    void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;
    int DrawPageTab(wxDC& dc, wxWindow* wnd, wxAuiNotebookPage& page, const wxRect& rect) override;
    int GetIndentSize() override;

    wxSize
    GetPageTabSize(wxReadOnlyDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, int* xExtent = nullptr) override;
    int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages, const wxSize& requiredBmpSize) override;
    void UpdateColoursFromSystem() override;

private:
    // Private pseudo-copy ctor used by Clone().
    explicit clAuiFlatTabArt(clAuiFlatTabArt* other);

    virtual wxColour GetButtonColour(wxAuiButtonId button, wxAuiPaneButtonState state) const override;

    struct Data;
    Data* const m_data{nullptr};
};
