#ifndef CLMARKDOWNRENDERER_HPP
#define CLMARKDOWNRENDERER_HPP

#include "codelite_exports.h"
#include "mdparser.hpp"

#include <wx/dc.h>

class WXDLLIMPEXP_SDK clMarkdownRenderer
{
private:
    void UpdateFont(wxDC& dc, const mdparser::Style& style);
    wxSize DoRender(wxWindow* win, wxDC& dc, const wxString& text, const wxRect& rect, bool do_draw);

public:
    clMarkdownRenderer();
    ~clMarkdownRenderer();

    /**
     * @brief do not render, just calculate the size required to draw the text
     */
    wxSize GetSize(wxWindow* win, wxDC& dc, const wxString& text);
    /**
     * @brief Render the tip and return the size required to do so
     */
    wxSize Render(wxWindow* win, wxDC& dc, const wxString& text, const wxRect& rect);
};

#endif // CLMARKDOWNRENDERER_HPP
