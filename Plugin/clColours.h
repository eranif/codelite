#ifndef CLCOLOURS_H
#define CLCOLOURS_H

#include "codelite_exports.h"
#include <wx/colour.h>

class WXDLLIMPEXP_SDK clColours
{
    wxColour hoverBgColour;          // Background colour of an hovered item
    wxColour itemTextColour;         // item text colour
    wxColour itemBgColour;           // item bg colour
    wxColour selItemTextColour;      // text colour for the selected item
    wxColour selItemBgColour;        // selected item background colour
    wxColour selItemBgColourNoFocus; // selected item background colour
    wxColour buttonColour;           // expand/collapse button colour
    wxColour selbuttonColour;        // The colour of the button ("Expand") when on a selected row
    wxColour bgColour;               // background colour for the control
    wxColour scrolBarButton;         // The scrollbar thumb button colour
    wxColour alternateColourOdd;     // Colour to draw odd items background (wxTR_ROW_LINES)
    wxColour alternateColourEven;    // Colour to draw even items background (wxTR_ROW_LINES)
    wxColour headerBgColour;         // Header background colour
    wxColour headerHBorderColour;    // The line to use for drawing a horizontal header border
    wxColour headerVBorderColour;    // The line to use for drawing a vertical header border
    wxColour matchedItemText;        // Text colour for matched item (need the style wxTR_ENABLE_SEARCH)
    wxColour matchedItemBgText;      // Text bg colour for matched item (need the style wxTR_ENABLE_SEARCH)
    bool m_useNativeColours = false; // When possible, use the native colours

public:
    clColours();
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
    const wxColour& GetSelItemBgColour() const { return selItemBgColour; }
    const wxColour& GetSelItemTextColour() const { return selItemTextColour; }
    void SetHeaderBgColour(const wxColour& headerBgColour) { this->headerBgColour = headerBgColour; }
    const wxColour& GetHeaderBgColour() const { return headerBgColour; }
    void SetHeaderHBorderColour(const wxColour& headerHBorderColour)
    {
        this->headerHBorderColour = headerHBorderColour;
    }
    void SetHeaderVBorderColour(const wxColour& headerVBorderColour)
    {
        this->headerVBorderColour = headerVBorderColour;
    }
    const wxColour& GetHeaderHBorderColour() const { return headerHBorderColour; }
    const wxColour& GetHeaderVBorderColour() const { return headerVBorderColour; }
    void SetSelbuttonColour(const wxColour& selbuttonColour) { this->selbuttonColour = selbuttonColour; }
    const wxColour& GetSelbuttonColour() const { return selbuttonColour; }
    void SetSelItemBgColourNoFocus(const wxColour& selItemBgColourNoFocus)
    {
        this->selItemBgColourNoFocus = selItemBgColourNoFocus;
    }
    const wxColour& GetSelItemBgColourNoFocus() const { return selItemBgColourNoFocus; }
    void SetUseNativeColours(bool useNativeColours) { this->m_useNativeColours = useNativeColours; }
    bool IsUseNativeColours() const { return m_useNativeColours; }
    void SetMatchedItemBgText(const wxColour& matchedItemBgText) { this->matchedItemBgText = matchedItemBgText; }
    void SetMatchedItemText(const wxColour& matchedItemText) { this->matchedItemText = matchedItemText; }
    const wxColour& GetMatchedItemBgText() const { return matchedItemBgText; }
    const wxColour& GetMatchedItemText() const { return matchedItemText; }
};

#endif // CLCOLOURS_H
