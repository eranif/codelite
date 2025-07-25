#include "clRowEntry.h"

#include "clCellValue.h"
#include "clHeaderBar.h"
#include "clHeaderItem.h"
#include "clSystemSettings.h"
#include "clTreeCtrl.h"
#include "drawingutils.h"
#include "file_logger.h"
#include "functional"
#include "globals.h"

#include <algorithm>
#include <wx/dataview.h>
#include <wx/dc.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/window.h>

#if !wxCHECK_VERSION(3, 1, 0)
#define wxCONTROL_NONE 0
#endif

#ifdef __WXGTK__
#define SELECTION_RECT_DEFLATE_X 1
#define SELECTION_RECT_DEFLATE_Y 0
#define SELECTION_RADIUS 0.0
#elif defined(__WXMSW__)
#define SELECTION_RECT_DEFLATE_X 1
#define SELECTION_RECT_DEFLATE_Y 0
#define SELECTION_RADIUS 0.0
#else
// macOS
#define SELECTION_RECT_DEFLATE_X 1
#define SELECTION_RECT_DEFLATE_Y 0
#define SELECTION_RADIUS 0.0
#endif

namespace
{
const wxString COLOUR_TEXT = " #FFFFFF ";

struct clClipperHelper {
    bool m_used = false;
    wxRect m_oldRect;
    wxDC& m_dc;

    clClipperHelper(wxDC& dc)
        : m_dc(dc)
    {
    }

    ~clClipperHelper() { Reset(); }

    void Clip(const wxRect& rect)
    {
        // Make sure we can call this method only once
        if (m_used) {
            return;
        }
        m_dc.GetClippingBox(m_oldRect);
        m_dc.SetClippingRegion(rect);
        m_used = true;
    }

    void Reset()
    {
        // Make sure we can call this method only once
        if (m_used) {
            m_dc.DestroyClippingRegion();
            if (!m_oldRect.IsEmpty()) {
                m_dc.SetClippingRegion(m_oldRect);
            }
            m_used = false;
        }
    }
};

wxString GetTextForRendering(const wxString& text)
{
    size_t pos = text.find('\n');
    if (pos != wxString::npos) {
        wxString fixed_text = text.Mid(0, pos - 1);
        if (fixed_text.length() != text.length()) {
            fixed_text << "...";
        }
        return fixed_text;
    } else {
        return text;
    }
}

namespace
{
    void draw_rectangle(wxDC& dc, const wxRect& rect, const wxColour& pen_colour, const wxColour& brush_colour,
                        double radius = 0)
    {
        wxBrush brush(brush_colour);
        wxPen pen(pen_colour);

        wxDCBrushChanger brush_changer(dc, brush);
        wxDCPenChanger pen_changer(dc, pen);

        dc.SetPen(pen);
        dc.SetBrush(brush);
        dc.DrawRoundedRectangle(rect, radius);
    }

    void draw_item_selected_rect(wxWindow* win, wxDC& dc, const wxRect& rect, const wxColour& pen_colour,
                                 const wxColour& brush_colour, double radius = 0)
    {
        wxUnusedVar(win);
        draw_rectangle(dc, rect, pen_colour, brush_colour, radius);
    }
} // namespace

void DoDrawSimpleSelection(wxWindow* win, wxDC& dc, const wxRect& rect, const clColours& colours)
{
    wxColour selectionBg = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    wxColour selectionPen = selectionBg;

#ifdef __WXMSW__
    if (!clSystemSettings::IsDark() && DrawingUtils::IsDark(selectionBg)) {
        selectionBg = selectionBg.ChangeLightness(150);
    }
#endif

    wxRect r = rect;
    r = r.CenterIn(rect);
    draw_item_selected_rect(win, dc, r, selectionPen, selectionBg, 0.0);
}

void DrawButton(wxWindow* win, wxDC& dc, const wxRect& button_rect, const clCellValue& cell)
{
    DrawingUtils::DrawButton(dc, win, button_rect, cell.GetButtonUnicodeSymbol(), wxNullBitmap, eButtonKind::kNormal,
                             cell.GetButtonState());
}
} // namespace

#ifdef __WXMSW__
int clRowEntry::X_SPACER = 4;
int clRowEntry::Y_SPACER = 2;
#elif defined(__WXOSX__)
int clRowEntry::X_SPACER = 4;
int clRowEntry::Y_SPACER = 4;
#else
int clRowEntry::X_SPACER = 5;
int clRowEntry::Y_SPACER = 2;
#endif

#ifdef __WXMSW__
#define PEN_STYLE wxPENSTYLE_SHORT_DASH
#define IS_MSW 1
#else
#define PEN_STYLE wxPENSTYLE_DOT
#define IS_MSW 0
#endif

#ifdef __WXOSX__
#define IS_OSX 1
#else
#define IS_OSX 0
#endif

#ifdef __WXGTK__
#define IS_GTK 1
#else
#define IS_GTK 0
#endif

void clRowEntry::DrawSimpleSelection(wxWindow* win, wxDC& dc, const wxRect& rect, const clColours& colours)
{
    DoDrawSimpleSelection(win, dc, rect, colours);
}

clRowEntry::clRowEntry(clTreeCtrl* tree, const wxString& label, int bitmapIndex, int bitmapSelectedIndex)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
{
    if (m_tree) {
        // Fill the verctor with items constructed using the _non_ default constructor
        // to makes sure that IsOk() returns TRUE
        m_cells.resize(m_tree->GetHeader()->empty() ? 1 : m_tree->GetHeader()->size(),
                       clCellValue("", -1, -1)); // at least one column
        clCellValue cv(label, bitmapIndex, bitmapSelectedIndex);
        m_cells[0] = cv;
    }
}

clRowEntry::clRowEntry(clTreeCtrl* tree, bool checked, const wxString& label, int bitmapIndex, int bitmapSelectedIndex)
    : m_tree(tree)
    , m_model(tree ? &tree->GetModel() : nullptr)
{
    // Fill the verctor with items constructed using the _non_ default constructor
    // to makes sure that IsOk() returns TRUE
    if (m_tree) {
        m_cells.resize(m_tree->GetHeader()->empty() ? 1 : m_tree->GetHeader()->size(),
                       clCellValue("", -1, -1)); // at least one column
        clCellValue cv(checked, label, bitmapIndex, bitmapSelectedIndex);
        m_cells[0] = cv;
    }
}

clRowEntry::~clRowEntry()
{
    // Delete all the node children
    DeleteAllChildren();
    wxDELETE(m_clientObject);

    for (auto& cell : m_cells) {
        if (cell.IsControl() && cell.GetControl()) {
            cell.GetControl()->Destroy();
            cell.SetControl(nullptr);
        }
    }

    // Notify the model that a selection is being deleted
    if (m_model) {
        m_model->NodeDeleted(this);
    }
}

void clRowEntry::ConnectNodes(clRowEntry* first, clRowEntry* second)
{
    if (first) {
        first->m_next = this;
    }
    this->m_prev = first;
    this->m_next = second;
    if (second) {
        second->m_prev = this;
    }
}

void clRowEntry::InsertChild(clRowEntry* child, clRowEntry* prev)
{
    child->SetParent(this);
    child->SetIndentsCount(GetIndentsCount() + 1);

    // We need the last item of this subtree (prev 'this' is the root)
    clRowEntry::Vec_t::iterator iterCur = {};
    if (prev == nullptr || prev == this) {
        // make it the first item
        iterCur = m_children.insert(m_children.begin(), child);
    } else {

        // optimization:
        // we often get here by calling AddChild(), so don't loop over the list, check if `prev`
        // is the last item
        if (!m_children.empty() && m_children.back() == prev) {
            iterCur = m_children.insert(m_children.end(), child);
        } else {
            // Insert the item in the parent children list
            clRowEntry::Vec_t::iterator iter = m_children.end();
            iter = std::find_if(m_children.begin(), m_children.end(), [&](clRowEntry* c) { return c == prev; });
            if (iter != m_children.end()) {
                ++iter;
            }
            // if iter is end(), than the is actually appending the item
            iterCur = m_children.insert(iter, child);
        }
    }

    // iterCur points to the newly added `child` element in the array
    clRowEntry* nodeBefore = nullptr;

    // Find the item before and after
    if (iterCur == m_children.begin()) {
        nodeBefore = child->GetParent(); // "this"
    } else {
        --iterCur;
        clRowEntry* prevSibling = (*iterCur);
        while (prevSibling && prevSibling->HasChildren()) {
            prevSibling = prevSibling->GetLastChild();
        }
        nodeBefore = prevSibling;
    }
    child->ConnectNodes(nodeBefore, nodeBefore->m_next);
}

void clRowEntry::AddChild(clRowEntry* child) { InsertChild(child, m_children.empty() ? nullptr : m_children.back()); }

void clRowEntry::SetParent(clRowEntry* parent)
{
    if (m_parent == parent) {
        return;
    }
    if (m_parent) {
        m_parent->DeleteChild(this);
    }
    m_parent = parent;
}

void clRowEntry::DeleteChild(clRowEntry* child)
{
    // first remove all of its children
    child->DeleteAllChildren();

    // Connect the list
    clRowEntry* prev = child->m_prev;
    clRowEntry* next = child->m_next;
    if (prev) {
        prev->m_next = next;
    }
    if (next) {
        next->m_prev = prev;
    }
    // Now disconnect this child from this node
    if (child == m_children.back()) { // Fast track for DeleteAllChildren().
        m_children.pop_back();
    } else {
        clRowEntry::Vec_t::iterator iter =
            std::find_if(m_children.begin(), m_children.end(), [&](clRowEntry* c) { return c == child; });
        if (iter != m_children.end()) {
            m_children.erase(iter);
        }
    }
    wxDELETE(child);
}

int clRowEntry::GetExpandedLines() const
{
    clRowEntry* node = const_cast<clRowEntry*>(this);
    int counter = 0;
    while (node) {
        if (node->IsVisible()) {
            ++counter;
        }
        node = node->m_next;
    }
    return counter;
}

void clRowEntry::GetNextItems(int count, clRowEntry::Vec_t& items, bool selfIncluded)
{
    if (count <= 0) {
        return;
    }
    items.reserve(count);
    if (!this->IsHidden() && selfIncluded) {
        items.push_back(this);
    }
    clRowEntry* next = GetNext();
    while (next) {
        if (next->IsVisible() && !next->IsHidden()) {
            items.push_back(next);
        }
        if ((int)items.size() == count) {
            return;
        }
        next = next->GetNext();
    }
}

void clRowEntry::GetPrevItems(int count, clRowEntry::Vec_t& items, bool selfIncluded)
{
    if (count <= 0) {
        return;
    }
    items.reserve(count);
    if (!this->IsHidden() && selfIncluded) {
        items.insert(items.begin(), this);
    }
    clRowEntry* prev = GetPrev();
    while (prev) {
        if (prev->IsVisible() && !prev->IsHidden()) {
            items.insert(items.begin(), prev);
        }
        if ((int)items.size() == count) {
            return;
        }
        prev = prev->GetPrev();
    }
}

clRowEntry* clRowEntry::GetVisibleItem(int index)
{
    clRowEntry::Vec_t items;
    GetNextItems(index, items);
    if ((int)items.size() != index) {
        return nullptr;
    }
    return items.back();
}

void clRowEntry::UnselectAll()
{
    clRowEntry* item = const_cast<clRowEntry*>(this);
    while (item) {
        item->SetSelected(false);
        item = item->GetNext();
    }
}

bool clRowEntry::SetExpanded(bool b)
{
    if (!m_model) {
        return false;
    }
    if (IsHidden() && !b) {
        // Hidden root can not be hidden
        return false;
    }

    if (IsHidden()) {
        // Hidden node do not fire events
        SetFlag(kNF_Expanded, b);
        return true;
    }

    // Already expanded?
    if (b && IsExpanded()) {
        return true;
    }

    // Already collapsed?
    if (!b && !IsExpanded()) {
        return true;
    }
    if (!m_model->NodeExpanding(this, b)) {
        return false;
    }

    SetFlag(kNF_Expanded, b);
    m_model->NodeExpanded(this, b);
    return true;
}

void clRowEntry::ClearRects()
{
    m_buttonRect = wxRect();
    m_rowRect = wxRect();
}

std::vector<size_t> clRowEntry::GetColumnWidths(wxWindow* win, wxDC& dc)
{
    wxDCFontChanger font_changer(dc);

    std::vector<size_t> v;
    wxRect rowRect = GetItemRect();
    int itemIndent = IsListItem() ? clHeaderItem::X_SPACER : (GetIndentsCount() * m_tree->GetIndent());

    // set default font
    const wxFont default_font = m_tree->GetDefaultFont();
    v.reserve(m_cells.size());

    int COLOUR_TEXT_LEN = wxNOT_FOUND;
    COLOUR_TEXT_LEN = wxNOT_FOUND;

    for (size_t i = 0; i < m_cells.size(); ++i) {
        auto& cell = m_cells[i];

        // use the tree font to calculate the width
        dc.SetFont(default_font);

        // unless cell font is valid
        if (cell.GetFont().IsOk()) {
            // cell
            dc.SetFont(cell.GetFont());
        }

        v.emplace_back();
        size_t& width = v.back();

        if ((i == 0) && !IsListItem()) {
            // space for the button
            width += rowRect.GetHeight();
        }

        if (cell.IsColour()) {
            if (COLOUR_TEXT_LEN == wxNOT_FOUND) {
                COLOUR_TEXT_LEN = dc.GetTextExtent(COLOUR_TEXT).GetWidth();
            }
            width += COLOUR_TEXT_LEN;
        }

        if (cell.IsBool()) {
            // Render the checkbox
            width += X_SPACER;
            width += GetCheckBoxWidth(win);
            width += X_SPACER;
        }

        int bitmapIndex = cell.GetBitmapIndex();
        if (IsExpanded() && HasChildren() && cell.GetBitmapSelectedIndex() != wxNOT_FOUND) {
            bitmapIndex = cell.GetBitmapSelectedIndex();
        }

        if (bitmapIndex != wxNOT_FOUND) {
            const wxBitmap& bmp = m_tree->GetBitmap(bitmapIndex);
            if (bmp.IsOk()) {
                width += IsListItem() ? 0 : X_SPACER;
                width += bmp.GetScaledWidth();
                width += X_SPACER;
            }
        }

        // do we have text to redner?
        // if we dont and we have a header, use the header's label
        wxString label = cell.GetValueString();
        if (label.empty() && m_tree->GetHeader() && m_tree->GetHeader()->size() >= i) {
            label = m_tree->GetHeader()->Item(i).GetLabel();
        }

        if (!label.empty()) {
            wxString text_to_render = GetTextForRendering(label);
            width += (i == 0 ? itemIndent : clHeaderItem::X_SPACER);
            width += dc.GetTextExtent(text_to_render).GetWidth();
            width += X_SPACER;
        }

        if (cell.HasButton()) {
            width += X_SPACER;
            width += GetCheckBoxWidth(win);
            width += X_SPACER;
        }
    }
    return v;
}

void clRowEntry::RenderBackground(wxDC& dc, long tree_style, const clColours& c, int row_index)
{
    wxRect rowRect = GetItemRect();
    bool zebraColouring = (tree_style & wxTR_ROW_LINES) || (tree_style & wxDV_ROW_LINES);
    bool even_row = ((row_index % 2) == 0);

    clColours colours = c;
    if (zebraColouring) {
        // Set Zebra colouring, only if no user colour was provided for the given line
        colours.SetItemBgColour(even_row ? c.GetAlternateColour() : c.GetBgColour());
    }

    // Override default item bg colour with the user's one
    if (GetBgColour().IsOk()) {
        colours.SetItemBgColour(GetBgColour());
    }

    wxRect selectionRect = rowRect;
    wxPoint deviceOrigin = dc.GetDeviceOrigin();
    selectionRect.SetX(-deviceOrigin.x);
    draw_rectangle(dc, selectionRect, colours.GetItemBgColour(), colours.GetItemBgColour(), 0.0);
}

void clRowEntry::Render(wxWindow* win, wxDC& dc, const clColours& c, int row_index, clSearchText* searcher)
{
    wxUnusedVar(searcher);
    bool isDark = clSystemSettings::IsDark();

    wxRect rowRect = GetItemRect();
    bool zebraColouring = (m_tree->HasStyle(wxTR_ROW_LINES) || m_tree->HasStyle(wxDV_ROW_LINES));
    bool even_row = ((row_index % 2) == 0);

    // Not cell related
    clColours colours = c;
    if (zebraColouring) {
        // Set Zebra colouring, only if no user colour was provided for the given line
        colours.SetItemBgColour(even_row ? c.GetAlternateColour() : c.GetBgColour());
    }

    // Override default item bg colour with the user's one
    if (GetBgColour().IsOk()) {
        colours.SetItemBgColour(GetBgColour());
    }

    wxRect selectionRect = rowRect;
    wxPoint deviceOrigin = dc.GetDeviceOrigin();
    selectionRect.SetX(-deviceOrigin.x);

    // set a base colour for the item
    dc.SetBrush(colours.GetItemBgColour());
    dc.SetPen(colours.GetItemBgColour());
    dc.DrawRectangle(rowRect);

    if (IsSelected() && !m_tree->IsDisabled()) {
        DrawSimpleSelection(win, dc, selectionRect, colours);
    } else if (IsHovered() && !m_tree->IsDisabled()) {
        draw_rectangle(dc, selectionRect, colours.GetHoverBgColour(), colours.GetHoverBgColour());
    } else if (colours.GetItemBgColour().IsOk()) {
        draw_rectangle(dc, selectionRect, colours.GetItemBgColour(), colours.GetItemBgColour(), 0.0);
    }

    // Per cell drawings
    for (size_t i = 0; i < m_cells.size(); ++i) {
        bool last_cell = (i == (m_cells.size() - 1));
        colours = c; // reset the colours
        clCellValue& cell = GetColumn(i);
        wxFont f = m_tree->GetDefaultFont();
        if (cell.GetFont().IsOk()) {
            f = cell.GetFont();
        }

        if (m_tree->IsDisabled()) {
            // if the tree is disabled, use a gray text to draw the text
            colours.SetItemTextColour(colours.GetGrayText());
            colours.SetSelItemTextColour(colours.GetGrayText());

        } else if (cell.GetTextColour().IsOk()) {
            colours.SetItemTextColour(cell.GetTextColour());
        }

        if (cell.GetBgColour().IsOk()) {
            colours.SetItemBgColour(cell.GetBgColour());
        }

        dc.SetFont(f);

        // Define the base colour for drawing the collapse / expand button
        wxColour buttonColour =
            IsSelected() ? colours.GetSelItemTextColour() : (isDark ? wxColour("GREY") : wxColour("LIGHT GREY"));
        wxRect cellRect = GetCellRect(i);

        int textXOffset = cellRect.GetX();
        if ((i == 0) && !IsListItem()) {
            // The expand button is only make sense for the first cell
            if (HasChildren()) {
                wxRect assignedRectForButton = GetButtonRect();
                wxRect buttonRect = assignedRectForButton;
                buttonRect.Deflate(1);
                textXOffset += buttonRect.GetWidth();

                buttonRect = GetButtonRect();
                if (textXOffset >= cellRect.GetWidth()) {
                    // if we cant draw the button (off screen etc)
                    SetRects(GetItemRect(), wxRect());
                    continue;
                }

                buttonRect.Deflate(buttonRect.GetWidth() / 3);
                if (buttonRect.GetWidth() % 2 == 0) {
                    buttonRect.width += 1;
                }

                if (buttonRect.GetHeight() % 2 == 0) {
                    buttonRect.height += 1;
                }

                wxRect tribtn = buttonRect;
                const int PEN_WIDTH = 3;

                if (IsExpanded()) {
                    if (!IsSelected()) {
                        buttonColour = isDark ? *wxWHITE : *wxBLACK;
                    }

                    wxDCPenChanger penChanger(dc, wxPen(buttonColour, PEN_WIDTH));
                    tribtn.SetHeight(tribtn.GetHeight() - tribtn.GetHeight() / 2);
                    tribtn = tribtn.CenterIn(assignedRectForButton);
                    wxPoint middleLeft = wxPoint((tribtn.GetLeft() + tribtn.GetWidth() / 2), tribtn.GetBottom());
                    dc.DrawLine(tribtn.GetTopLeft(), middleLeft);
                    dc.DrawLine(tribtn.GetTopRight(), middleLeft);
                } else {
                    wxDCPenChanger penChanger(dc, wxPen(buttonColour, PEN_WIDTH));
                    tribtn.SetWidth(tribtn.GetWidth() - tribtn.GetWidth() / 2);
                    tribtn = tribtn.CenterIn(buttonRect);

                    wxPoint middleLeft = wxPoint(tribtn.GetRight(), (tribtn.GetY() + (tribtn.GetHeight() / 2)));
                    wxPoint p1 = tribtn.GetTopLeft();
                    wxPoint p2 = tribtn.GetBottomLeft();
                    dc.DrawLine(p1, middleLeft);
                    dc.DrawLine(middleLeft, p2);
                }

            } else {
                wxRect buttonRect(rowRect);
                buttonRect.SetWidth(rowRect.GetHeight());
                buttonRect.Deflate(1);
                textXOffset += buttonRect.GetWidth();
                if (textXOffset >= cellRect.GetWidth()) {
                    SetRects(GetItemRect(), wxRect());
                    continue;
                }
            }
        }
        int itemIndent = IsListItem() ? clHeaderItem::X_SPACER : (GetIndentsCount() * m_tree->GetIndent());
        int bitmapIndex = cell.GetBitmapIndex();
        if (IsExpanded() && HasChildren() && cell.GetBitmapSelectedIndex() != wxNOT_FOUND) {
            bitmapIndex = cell.GetBitmapSelectedIndex();
        }

        // Draw checkbox
        if (cell.IsBool()) {
            // Render the checkbox
            textXOffset += X_SPACER;
            int checkboxSize = GetCheckBoxWidth(win);
            wxRect checkboxRect = wxRect(textXOffset, rowRect.GetY(), checkboxSize, checkboxSize);
            checkboxRect = checkboxRect.CenterIn(rowRect, wxVERTICAL);
            dc.SetPen(colours.GetItemTextColour());
            RenderCheckBox(win, dc, colours, checkboxRect, cell.GetValueBool());
            cell.SetCheckboxRect(checkboxRect);
            textXOffset += checkboxRect.GetWidth();
            textXOffset += X_SPACER;
        } else {
            cell.SetCheckboxRect(wxRect()); // clear the checkbox rect
        }

        // Draw the bitmap
        if (bitmapIndex != wxNOT_FOUND) {
            const wxBitmap& bmp = m_tree->GetBitmap(bitmapIndex);
            if (bmp.IsOk()) {
                textXOffset += IsListItem() ? 0 : X_SPACER;
                int bitmapY = rowRect.GetY() + ((rowRect.GetHeight() - bmp.GetScaledHeight()) / 2);
                // if((textXOffset + bmp.GetScaledWidth()) >= cellRect.GetWidth()) { continue; }
                dc.DrawBitmap(bmp, itemIndent + textXOffset, bitmapY, true);
                textXOffset += bmp.GetScaledWidth();
                textXOffset += X_SPACER;
            }
        }

        // Draw the text
        if (!cell.GetValueString().empty()) {
            // clip the drawing region
            wxRect clip_rect = cellRect;
            if (cell.HasButton()) {
                clip_rect.SetWidth(clip_rect.GetWidth() - clip_rect.GetHeight() - 1);
            }
            clClipperHelper clipper(dc);
            clipper.Clip(clip_rect);

            wxString text_to_render = GetTextForRendering(cell.GetValueString());
            wxRect textRect(dc.GetTextExtent(text_to_render));
            textRect = textRect.CenterIn(rowRect, wxVERTICAL);
            int textY = textRect.GetY();
            int textX = (i == 0 ? itemIndent : clHeaderItem::X_SPACER) + textXOffset;
            RenderText(win, dc, colours, text_to_render, textX, textY, i);
            textXOffset += textRect.GetWidth();
            textXOffset += X_SPACER;
        }

        // By default, a cell has no action button
        cell.SetButtonRect(wxRect());
        if (cell.HasButton()) {
            // draw the drop down arrow. Make it aligned to the right
            textXOffset += X_SPACER;
            wxRect button_rect(textXOffset, rowRect.GetY(), rowRect.GetHeight(), rowRect.GetHeight());
            button_rect = button_rect.CenterIn(rowRect, wxVERTICAL);
            // Draw a button with the unicode symbol in it
            if (IsSelected()) {
                DrawButton(win, dc, button_rect, cell);
            }
            // Keep the rect to test clicks
            cell.SetButtonRect(button_rect);
            textXOffset += button_rect.GetWidth();
            textXOffset += X_SPACER;

        } else if (cell.IsButton()) {
            // the centire cell is a button
            wxRect button_rect = cellRect;
            button_rect.Deflate(1);
            button_rect = button_rect.CenterIn(cellRect);
            DrawingUtils::DrawButton(dc, win, button_rect, cell.GetValueString(), wxNullBitmap, eButtonKind::kNormal,
                                     cell.GetButtonState());
            cell.SetButtonRect(button_rect);
        } else if (cell.IsControl()) {
            // controls are only displayed if the line has focus
            auto ctrl = cell.GetControl();
            bool show_control = IsSelected() && !m_tree->IsDisabled();
            if (!show_control) {
                ctrl->Hide();
            } else {
                wxRect control_rect = cellRect;
                // set the control dimensions
                ctrl->SetSize(wxNOT_FOUND, control_rect.GetHeight());
                ctrl->Move(control_rect.GetTopLeft());
                ctrl->Show();
            }
        } else if (cell.IsColour()) {
            wxRect rr = cellRect;
            rr.Deflate(1);

            // since the method `DrawingUtils::DrawColourPicker` is not familiar with our spacing policy
            // move the drawing rectangle to the X_SPACER position
            // rr.SetX(rr.GetX() + X_SPACER);
            // rr.SetWidth(rr.GetWidth() - X_SPACER);
            wxRect button_rect =
                DrawingUtils::DrawColourPicker(win, dc, rr, cell.GetValueColour(), cell.GetButtonState());
            cell.SetButtonRect(button_rect);
        }

        if (!last_cell) {
            cellRect.SetHeight(rowRect.GetHeight());
            dc.SetPen(wxPen(colours.GetHeaderVBorderColour(), 1, PEN_STYLE));
            dc.DrawLine(cellRect.GetTopRight(), cellRect.GetBottomRight());
        }
    }
}

void clRowEntry::RenderText(wxWindow* win, wxDC& dc, const clColours& colours, const wxString& text, int x, int y,
                            size_t col)
{
    RenderTextSimple(win, dc, colours, text, x, y, col);
}

void clRowEntry::RenderTextSimple(wxWindow* win, wxDC& dc, const clColours& colours, const wxString& text, int x, int y,
                                  size_t col)
{
    wxUnusedVar(win);
    wxDCTextColourChanger changer(dc);

    // fallback to the default
    // select the default text colour
    wxColour text_colour = GetTextColour(col);
    if (!text_colour.IsOk()) {
        if (IsSelected()) {
            text_colour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        } else {
            text_colour = colours.GetItemTextColour();
        }
    } else if (IsSelected()) {
        // when selected, override the user provided colour
        text_colour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    }

    dc.SetTextForeground(text_colour);
    dc.DrawText(text, x, y);
}

size_t clRowEntry::GetChildrenCount(bool recurse) const
{
    if (!recurse) {
        return m_children.size();
    } else {
        size_t count = m_children.size();
        // Loop over the children and add its children count
        for (size_t i = 0; i < m_children.size(); ++i) {
            count += m_children[i]->GetChildrenCount(recurse);
        }
        return count;
    }
}

bool clRowEntry::IsVisible() const
{
    if (IsHidden()) {
        return false;
    }
    clRowEntry* parent = GetParent();
    while (parent) {
        if (!parent->IsExpanded()) {
            return false;
        }
        parent = parent->GetParent();
    }
    return true;
}

void clRowEntry::DeleteAllChildren()
{
    while (!m_children.empty()) {
        clRowEntry* c = m_children.back();
        // DeleteChild will remove it from the array
        DeleteChild(c);
    }
}

clRowEntry* clRowEntry::GetLastChild() const
{
    if (m_children.empty()) {
        return nullptr;
    }
    return m_children.back();
}

clRowEntry* clRowEntry::GetFirstChild() const
{
    if (m_children.empty()) {
        return nullptr;
    }
    return m_children[0];
}

void clRowEntry::SetHidden(bool b)
{
    if (b && !IsRoot()) {
        return;
    }
    SetFlag(kNF_Hidden, b);
    if (b) {
        m_indentsCount = -1;
    } else {
        m_indentsCount = 0;
    }
}

int clRowEntry::CalcItemWidth(wxDC& dc, int rowHeight, size_t col)
{
    wxUnusedVar(col);
    if (col >= m_cells.size()) {
        return 0;
    }

    clCellValue& cell = GetColumn(col);

    int item_width = X_SPACER;
    // colour items occupying the entire cell
    if (cell.IsColour()) {
        dc.SetFont(cell.GetFont().IsOk() ? cell.GetFont() : m_tree->GetDefaultFont());
        item_width += dc.GetTextExtent(COLOUR_TEXT).GetWidth();
        item_width += X_SPACER;
        return item_width;
    }

    if (cell.IsBool()) {
        // add the checkbox size
        item_width += clGetSize(rowHeight, m_tree);
        item_width += X_SPACER;
    } else if (cell.HasButton()) {
        item_width += clGetSize(rowHeight, m_tree);
        item_width += X_SPACER;
    } else if (cell.IsControl()) {
        item_width += cell.GetControl()->GetSize().GetWidth();
        item_width += X_SPACER;
    }

    dc.SetFont(cell.GetFont().IsOk() ? cell.GetFont() : m_tree->GetDefaultFont());
    wxString text_to_render = GetTextForRendering(cell.GetValueString());
    wxSize textSize = dc.GetTextExtent(text_to_render);
    if ((col == 0) && !IsListItem()) {
        // always make room for the twist button
        item_width += clGetSize(rowHeight, m_tree);
    }
    int bitmapIndex = cell.GetBitmapIndex();
    if (IsExpanded() && HasChildren() && cell.GetBitmapSelectedIndex() != wxNOT_FOUND) {
        bitmapIndex = cell.GetBitmapSelectedIndex();
    }

    if (bitmapIndex != wxNOT_FOUND) {
        const wxBitmap& bmp = m_tree->GetBitmap(bitmapIndex);
        if (bmp.IsOk()) {
            item_width += clGetSize(X_SPACER, m_tree);
            item_width += bmp.GetScaledWidth();
            item_width += clGetSize(X_SPACER, m_tree);
        }
    }
    if ((col == 0) && !IsListItem()) {
        int itemIndent = (GetIndentsCount() * m_tree->GetIndent());
        item_width += clGetSize(itemIndent, m_tree);
    }
    item_width += textSize.GetWidth();
    item_width += clGetSize(clHeaderItem::X_SPACER, m_tree);
    return item_width;
}

void clRowEntry::SetBitmapIndex(int bitmapIndex, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetBitmapIndex(bitmapIndex);
}

int clRowEntry::GetBitmapIndex(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return wxNOT_FOUND;
    }
    return cell.GetBitmapIndex();
}

void clRowEntry::SetBitmapSelectedIndex(int bitmapIndex, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetBitmapSelectedIndex(bitmapIndex);
}

int clRowEntry::GetBitmapSelectedIndex(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return wxNOT_FOUND;
    }
    return cell.GetBitmapSelectedIndex();
}

void clRowEntry::SetLabel(const wxString& label, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetValue(label);
}

const wxString& clRowEntry::GetLabel(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        static wxString empty_string;
        return empty_string;
    }
    return cell.GetValueString();
}

void clRowEntry::SetChecked(bool checked, int bitmapIndex, const wxString& label, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetValue(checked);
    cell.SetValue(label);
    cell.SetBitmapIndex(bitmapIndex);
    // Mark this type as 'bool'
    cell.SetType(clCellValue::kTypeBool);
}

bool clRowEntry::IsChecked(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return false;
    }
    return cell.GetValueBool();
}

const clCellValue& clRowEntry::GetColumn(size_t col) const
{
    if (col >= m_cells.size()) {
        static clCellValue null_column;
        return null_column;
    }
    return m_cells[col];
}

clCellValue& clRowEntry::GetColumn(size_t col)
{
    if (col >= m_cells.size()) {
        static clCellValue null_column;
        return null_column;
    }
    return m_cells[col];
}

void clRowEntry::SetBgColour(const wxColour& bgColour, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetBgColour(bgColour);
}

void clRowEntry::SetFont(const wxFont& font, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetFont(font);
}

void clRowEntry::SetTextColour(const wxColour& textColour, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetTextColour(textColour);
}

const wxColour& clRowEntry::GetBgColour(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        static wxColour invalid_colour;
        return invalid_colour;
    }
    return cell.GetBgColour();
}

const wxFont& clRowEntry::GetFont(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        static wxFont invalid_font;
        return invalid_font;
    }
    return cell.GetFont();
}

const wxColour& clRowEntry::GetTextColour(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        static wxColour invalid_colour;
        return invalid_colour;
    }
    const wxColour& colour = cell.GetTextColour();
    if (!colour.IsOk()) {
        // if one of our parent has a valid colour (or its parent.. or ...) __and__ our colour is invalid -> lets use
        // our parent's colour
        auto parent = GetParent();
        while (parent) {
            if (parent->GetColumn(col).IsOk() && parent->GetColumn(col).GetTextColour().IsOk()) {
                return parent->GetColumn(col).GetTextColour();
            }
            parent = parent->GetParent();
        }
    }
    // return the current cell colour
    return colour;
}

wxRect clRowEntry::GetCellRect(size_t col) const
{
    if (m_tree && m_tree->GetHeader() && (col < m_tree->GetHeader()->size())) {
        // Check which column was clicked
        wxRect cellRect = m_tree->GetHeader()->Item(col).GetRect();

        const wxRect& itemRect = GetItemRect();
        // Make sure that the cellRect has all the correct attributes of the row
        cellRect.SetY(itemRect.GetY());
        // If we got h-scrollbar, adjust the X coordinate
        // cellRect.SetX(cellRect.GetX() - m_tree->GetFirstColumn());
        cellRect.SetHeight(itemRect.GetHeight());
        return cellRect;
    } else {
        return GetItemRect();
    }
}

const wxRect& clRowEntry::GetCheckboxRect(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        static wxRect emptyRect;
        return emptyRect;
    }
    return cell.GetCheckboxRect();
}

const wxRect& clRowEntry::GetCellButtonRect(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        static wxRect emptyRect;
        return emptyRect;
    }
    return cell.GetButtonRect();
}

#ifdef __WXMSW__
namespace
{
wxColour MSWGetCheckBoxColour(bool checkmark, const clColours& colours, bool isSelected)
{
    bool is_light_theme = colours.IsLightTheme();
    if (checkmark) {
        return clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    } else {
        if (is_light_theme) {
            if (isSelected) {
                return clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT).ChangeLightness(120);
            } else {
                return clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
            }
        } else {
            return wxColour("SLATE BLUE");
        }
    }
}
} // namespace
#endif

void clRowEntry::RenderCheckBox(wxWindow* win, wxDC& dc, const clColours& colours, const wxRect& rect, bool checked)
{
    // on mac/linux, use native drawings
#ifndef __WXMSW__
    int flags = wxCONTROL_CURRENT;
    if (checked) {
        flags |= wxCONTROL_CHECKED;
    }
    wxRendererNative::Get().DrawCheckBox(win, dc, rect, flags);
#else
    wxColour border_colour = MSWGetCheckBoxColour(false, colours, IsSelected());
    wxDCPenChanger pen_changer(dc, border_colour);
    wxDCBrushChanger bursh_changer(dc, border_colour);
    dc.DrawRoundedRectangle(rect, 2.0);
    if (checked) {
        wxColour color = MSWGetCheckBoxColour(true, colours, IsSelected());
        wxDCPenChanger pen_changer(dc, color);
        wxDCTextColourChanger text_changer(dc, color);
        wxRect check_mark_rect = rect;
        check_mark_rect.Deflate(4);
        check_mark_rect = check_mark_rect.CenterIn(rect);
        dc.DrawCheckMark(check_mark_rect);
    }
#endif
}

int clRowEntry::GetCheckBoxWidth(wxWindow* win)
{
    static int width = wxNOT_FOUND;
    if (width == wxNOT_FOUND) {
        width = wxRendererNative::Get().GetCheckBoxSize(win).GetWidth();
        if (width <= 0) {
            // set default value
            width = 20;
        }
    }
    return width;
}

void clRowEntry::SetHasButton(eCellButtonType button_type, const wxString& unicode_symbol, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetType(clCellValue::kTypeButton);
    cell.SetButtonType(button_type, unicode_symbol);
}

bool clRowEntry::HasButton(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return false;
    }
    return cell.HasButton();
}

void clRowEntry::SetIsButton(const wxString& label, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetType(clCellValue::kTypeOnlyButton);
    cell.SetValue(label);
}

bool clRowEntry::IsButton(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return false;
    }
    return cell.IsButton();
}

void clRowEntry::SetColour(const wxColour& colour, size_t col)
{
    auto& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    cell.SetType(clCellValue::kTypeColour);
    cell.SetValue(colour);
}

bool clRowEntry::IsColour(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return false;
    }
    return cell.IsColour();
}

bool clRowEntry::IsControl(size_t col) const
{
    const clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return false;
    }
    return cell.IsControl();
}

void clRowEntry::SetIsControl(wxControl* ctrl, size_t col)
{
    clCellValue& cell = GetColumn(col);
    if (!cell.IsOk()) {
        return;
    }
    ctrl->Reparent(m_tree);
    cell.SetControl(ctrl); // this also marks the cell as `IsControl() == true`
}
