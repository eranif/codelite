#ifndef CLCOLOURS_H
#define CLCOLOURS_H

#include "codelite_exports.h"
#include <wx/colour.h>

class WXDLLIMPEXP_SDK clColours
{
    wxColour hoverBgColour;       // Background colour of an hovered item
    wxColour itemTextColour;      // item text colour
    wxColour itemBgColour;        // item bg colour
    wxColour selItemTextColour;   // text colour for the selected item
    wxColour selItemBgColour;     // selected item background colour
    wxColour buttonColour;        // expand/collapse button colour
    wxColour bgColour;            // background colour for the control
    wxColour scrolBarButton;      // The scrollbar thumb button colour
    wxColour scrollBarBgColour;   // The scrollbar background colour
    wxColour alternateColourOdd;  // Colour to draw odd items background (wxTR_ROW_LINES)
    wxColour alternateColourEven; // Colour to draw even items background (wxTR_ROW_LINES)

public:
    clColours() { InitDefaults(); }
    virtual ~clColours() {}
    void InitDefaults();
    void InitDarkDefaults();
    void SetAlternateColourEven(const wxColour& alternateColourEven)
    {
        this->alternateColourEven = alternateColourEven;
    }
    void SetAlternateColourOdd(const wxColour& alternateColourOdd) { this->alternateColourOdd = alternateColourOdd; }
    void SetBgColour(const wxColour& bgColour) { this->bgColour = bgColour; }
    void SetButtonColour(const wxColour& buttonColour) { this->buttonColour = buttonColour; }
    void SetHoverBgColour(const wxColour& hoverBgColour) { this->hoverBgColour = hoverBgColour; }
    void SetItemBgColour(const wxColour& itemBgColour) { this->itemBgColour = itemBgColour; }
    void SetItemTextColour(const wxColour& itemTextColour) { this->itemTextColour = itemTextColour; }
    void SetScrolBarButton(const wxColour& scrolBarButton) { this->scrolBarButton = scrolBarButton; }
    void SetScrollBarBgColour(const wxColour& scrollBarBgColour) { this->scrollBarBgColour = scrollBarBgColour; }
    void SetSelItemBgColour(const wxColour& selItemBgColour) { this->selItemBgColour = selItemBgColour; }
    void SetSelItemTextColour(const wxColour& selItemTextColour) { this->selItemTextColour = selItemTextColour; }
    const wxColour& GetAlternateColourEven() const { return alternateColourEven; }
    const wxColour& GetAlternateColourOdd() const { return alternateColourOdd; }
    const wxColour& GetBgColour() const { return bgColour; }
    const wxColour& GetButtonColour() const { return buttonColour; }
    const wxColour& GetHoverBgColour() const { return hoverBgColour; }
    const wxColour& GetItemBgColour() const { return itemBgColour; }
    const wxColour& GetItemTextColour() const { return itemTextColour; }
    const wxColour& GetScrolBarButton() const { return scrolBarButton; }
    const wxColour& GetScrollBarBgColour() const { return scrollBarBgColour; }
    const wxColour& GetSelItemBgColour() const { return selItemBgColour; }
    const wxColour& GetSelItemTextColour() const { return selItemTextColour; }
};

#endif // CLCOLOURS_H
