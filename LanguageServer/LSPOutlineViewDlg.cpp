#include "LSPOutlineViewDlg.h"

#include "ColoursAndFontsManager.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "drawingutils.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"

#include <stack>
#include <wx/dataview.h>

namespace
{
const wxString FUNCTION_SYMBOL = wxT("\u2A10");
const wxString CLASS_SYMBOL = wxT("\u2394");
const wxString VARIABLE_SYMBOL = wxT("\u2027");
const wxString MODULE_SYMBOL = wxT("{}");
const wxString ENUMERATOR_SYMBOL = wxT("#");
} // namespace

LSPOutlineViewDlg::LSPOutlineViewDlg(wxWindow* parent)
    : LSPOutlineViewDlgBase(parent)
{
    clSetDialogBestSizeAndPosition(this);
    DoInitialise();
}

LSPOutlineViewDlg::~LSPOutlineViewDlg() {}

void LSPOutlineViewDlg::DoInitialise()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("python");
    m_dvTreeCtrll->DeleteAllItems();
    m_dvTreeCtrll->SetSortFunction(nullptr);
    m_textCtrlFilter->ClearAll();

    if(m_symbols.empty()) {
        clAnsiEscapeCodeColourBuilder builder;
        builder.SetTheme(lexer->IsDark() ? eAsciiTheme::DARK : eAsciiTheme::LIGHT);
        builder.Add(_("Language Server is still not ready... "), eAsciiColours::NORMAL_TEXT, false);
        builder.Add(_("(hit ESCAPE key to dismiss)"), eAsciiColours::GRAY, false);
        m_dvTreeCtrll->AddLine(builder.GetString(), false, (wxUIntPtr)0);
        m_textCtrlFilter->CallAfter(&wxTextCtrl::SetFocus);
        return;
    }

    m_dvTreeCtrll->Begin();
    m_dvTreeCtrll->SetScrollToBottom(false);

    // build the tree
    wxColour class_colour = lexer->GetProperty(wxSTC_P_WORD2).GetFgColour();
    wxColour variable_colour = lexer->GetProperty(wxSTC_P_IDENTIFIER).GetFgColour();
    wxColour module_colour = lexer->GetProperty(wxSTC_P_STRING).GetFgColour();
    wxColour function_colour = lexer->GetProperty(wxSTC_P_DEFNAME).GetFgColour();
    wxColour operator_colour = lexer->GetProperty(wxSTC_P_OPERATOR).GetFgColour();
    vector<pair<wxString, int>> containers;

    clAnsiEscapeCodeColourBuilder builder;
    for(const SymbolInformation& si : m_symbols) {
        const wxString& symbol_container = si.GetContainerName();
        if(symbol_container.empty()) {
            containers.push_back({ si.GetName(), 1 });
        } else {
            int parent_depth = 0;
            while(!containers.empty()) {
                if(containers.back().first == symbol_container) {
                    parent_depth = containers.back().second;
                    break;
                }
                containers.pop_back();
            }
            containers.push_back({ si.GetName(), parent_depth + 1 });
        }

        builder.Clear();
        int curdepth = containers.empty() ? 1 : containers.back().second;
        // add indentation
        builder.Add(wxString(' ', curdepth * 4), eAsciiColours::NORMAL_TEXT);

        // determine the symbol
        switch(si.GetKind()) {
        case kSK_File:
        case kSK_Module:
        case kSK_Package:
            builder.Add(MODULE_SYMBOL + " ", eAsciiColours::NORMAL_TEXT);
            builder.Add(si.GetName(), module_colour);
            break;
        case kSK_Class:
        case kSK_Struct:
        case kSK_Interface:
        case kSK_Object:
        case kSK_Enum:
            builder.Add(CLASS_SYMBOL + " ", eAsciiColours::NORMAL_TEXT);
            builder.Add(si.GetName(), class_colour, true);
            break;
        case kSK_Method:
        case kSK_Function:
        case kSK_Constructor:
            builder.Add(FUNCTION_SYMBOL + " ", eAsciiColours::NORMAL_TEXT);
            if(si.GetName().Contains("(") && si.GetName().Contains(")")) {
                // the name also has the signature
                wxString signature = si.GetName().AfterFirst('(');
                signature = signature.BeforeLast(')');
                wxString name_only = si.GetName().BeforeFirst('(');
                builder.Add(name_only, function_colour);
                builder.Add("(", operator_colour);
                builder.Add(signature, eAsciiColours::NORMAL_TEXT);
                builder.Add(")", operator_colour);
            } else {
                builder.Add(si.GetName(), function_colour);
                builder.Add("()", operator_colour);
            }
            break;
        case kSK_TypeParameter: // define
        case kSK_EnumMember:
            builder.Add(ENUMERATOR_SYMBOL + " ", eAsciiColours::NORMAL_TEXT);
            builder.Add(si.GetName(), eAsciiColours::NORMAL_TEXT);
            break;
        default:
            builder.Add(VARIABLE_SYMBOL + " ", eAsciiColours::NORMAL_TEXT);
            builder.Add(si.GetName(), variable_colour);
            break;
        }
        m_dvTreeCtrll->AddLine(builder.GetString(), false, (wxUIntPtr)&si);
    }
    if(!m_dvTreeCtrll->IsEmpty()) {
        m_dvTreeCtrll->SelectRow(0);
    }
    m_dvTreeCtrll->Commit();
    m_textCtrlFilter->CallAfter(&wxTextCtrl::SetFocus);
}

void LSPOutlineViewDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSelectionActivate();
}

void LSPOutlineViewDlg::OnTextUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dvTreeCtrll->ClearAllHighlights();

    wxString filter_text = m_textCtrlFilter->GetValue();
    wxDataViewItem starting_item =
        m_dvTreeCtrll->GetSelection().IsOk() ? m_dvTreeCtrll->GetSelection() : wxDataViewItem{ nullptr };
    auto match = m_dvTreeCtrll->FindNext(starting_item, filter_text, 0, wxTR_SEARCH_DEFAULT);
    if(match.IsOk()) {
        m_dvTreeCtrll->Select(match);
        m_dvTreeCtrll->HighlightText(match, true);
        m_dvTreeCtrll->EnsureVisible(match);
    }
}
void LSPOutlineViewDlg::OnItemActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    DoSelectionActivate();
}

void LSPOutlineViewDlg::OnKeyDown(wxKeyEvent& event)
{
    switch(event.GetKeyCode()) {
    case WXK_ESCAPE:
        Hide();
        break;
    case WXK_DOWN:
        DoFindNext();
        break;
    case WXK_UP:
        DoFindPrev();
        break;
    default:
        event.Skip();
        break;
    }
}

void LSPOutlineViewDlg::DoFindNext()
{
    m_dvTreeCtrll->ClearAllHighlights();

    int sel_row = m_dvTreeCtrll->GetSelectedRow();
    if((sel_row + 1) >= m_dvTreeCtrll->GetItemCount()) {
        return;
    }

    wxDataViewItem next_item = m_dvTreeCtrll->RowToItem(sel_row + 1);
    wxString find_what = m_textCtrlFilter->GetValue();
    if(find_what.empty()) {
        m_dvTreeCtrll->Select(next_item);
        m_dvTreeCtrll->EnsureVisible(next_item);
    } else {
        wxDataViewItem item = m_dvTreeCtrll->FindNext(next_item, find_what, 0, wxTR_SEARCH_DEFAULT);
        CHECK_ITEM_RET(item);
        m_dvTreeCtrll->Select(item);
        m_dvTreeCtrll->EnsureVisible(item);
    }
}

void LSPOutlineViewDlg::DoFindPrev()
{
    m_dvTreeCtrll->ClearAllHighlights();

    int sel_row = m_dvTreeCtrll->GetSelectedRow();
    if(sel_row < 1) {
        return;
    }

    wxDataViewItem prev_item = m_dvTreeCtrll->RowToItem(sel_row - 1);
    wxString find_what = m_textCtrlFilter->GetValue();
    if(find_what.empty()) {
        m_dvTreeCtrll->Select(prev_item);
        m_dvTreeCtrll->EnsureVisible(prev_item);
    } else {
        wxDataViewItem item = m_dvTreeCtrll->FindPrev(prev_item, find_what, 0, wxTR_SEARCH_DEFAULT);
        CHECK_ITEM_RET(item);
        m_dvTreeCtrll->Select(item);
        m_dvTreeCtrll->EnsureVisible(item);
    }
}

void LSPOutlineViewDlg::OnListKeyDown(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_ESCAPE) {
        Hide();
    } else {
        event.Skip();
    }
}

void LSPOutlineViewDlg::SetSymbols(const vector<SymbolInformation>& symbols)
{
    m_symbols = symbols;
    DoInitialise();
}

void LSPOutlineViewDlg::DoSelectionActivate()
{
    auto selection = m_dvTreeCtrll->GetSelection();
    if(!selection.IsOk()) {
        return;
    }

    LSP::SymbolInformation* si = reinterpret_cast<LSP::SymbolInformation*>(m_dvTreeCtrll->GetItemData(selection));
    CHECK_PTR_RET(si);

    // open the editor and go to
    LSP::Location loc = si->GetLocation();
    IEditor* active_editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(active_editor);

    int sci_line = loc.GetRange().GetStart().GetLine();
    if(loc.GetRange().GetStart().GetLine() != loc.GetRange().GetEnd().GetLine()) {
        // different lines, don't select the entire function
        // just place the caret at the begining of the function
        int position = active_editor->PosFromLine(sci_line);  // start of line
        position += loc.GetRange().GetStart().GetCharacter(); // add the column
        active_editor->SetCaretAt(position);
        active_editor->CenterLine(sci_line);
    } else {
        active_editor->SelectRange(loc.GetRange());
        active_editor->CenterLinePreserveSelection(sci_line);
    }
    Hide();
}
