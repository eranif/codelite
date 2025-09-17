#pragma once

#include "codelite_exports.h"

#include <wx/aui/aui.h>
#include <wx/aui/auibook.h>
#include <wx/aui/tabart.h>
#include <wx/colour.h>
#include <wx/dc.h>
#include <wx/version.h>

#if wxCHECK_VERSION(3, 3, 0)

// This tab art provider draws flat tabs with a thin border.
class WXDLLIMPEXP_SDK clAuiFlatTabArt : public wxAuiTabArtBase
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
#else
using clAuiFlatTabArt = wxAuiDefaultTabArt;
#endif // wxCHECK_VERSION(3, 3, 0)
