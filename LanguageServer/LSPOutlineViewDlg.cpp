#include "LSPOutlineViewDlg.h"
#include "ColoursAndFontsManager.h"
#include "globals.h"
#include "imanager.h"
#include <stack>
#include <wx/dataview.h>

namespace
{
const wxString FUNCTION_SYMBOL = wxT("\u2A10");
const wxString CLASS_SYMBOL = wxT("\u2394");
const wxString VARIABLE_SYMBOL = wxT("\u2027");
const wxString MODULE_SYMBOL = wxT("\u00BB");
} // namespace

void LSPOutlineViewDlg::DoSelectionActivate()
{
    auto selection = m_dvTreeCtrll->GetSelection();
    if(!selection.IsOk()) {
        return;
    }

    LSP::SymbolInformation* si = reinterpret_cast<LSP::SymbolInformation*>(m_dvTreeCtrll->GetItemData(selection));
    // open the editor and go to
    LSP::Location loc = si->GetLocation();
    wxString file = loc.GetPath();
    int line = loc.GetRange().GetStart().GetLine();
    clGetManager()->OpenFile(file, "", line);
    EndModal(wxID_OK);
}

LSPOutlineViewDlg::LSPOutlineViewDlg(wxWindow* parent, const vector<SymbolInformation>& symbols)
    : LSPOutlineViewDlgBase(parent)
    , m_symbols(symbols)
{
    m_dvTreeCtrll->SetScrollToBottom(false);

    // build the tree
    auto lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    wxColour class_colour = lexer->GetProperty(wxSTC_C_WORD2).GetFgColour();
    wxColour variable_colour = lexer->GetProperty(wxSTC_C_GLOBALCLASS).GetFgColour();

    vector<pair<wxString, int>> containers;
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

        wxString current_line;
        int curdepth = containers.empty() ? 1 : containers.back().second;
        // add indentation
        current_line << wxString(' ', curdepth * 4);

        // determine the symbol
        switch(si.GetKind()) {
        case kSK_File:
        case kSK_Module:
        case kSK_Package:
            current_line << MODULE_SYMBOL << " " << si.GetName();
            break;
        case kSK_Class:
        case kSK_Struct:
        case kSK_Interface:
            current_line << CLASS_SYMBOL << " " << si.GetName();
            break;
        case kSK_Method:
        case kSK_Function:
        case kSK_Constructor:
            current_line << FUNCTION_SYMBOL << " " << si.GetName() << "()";
            break;
        default:
            current_line << VARIABLE_SYMBOL << " " << si.GetName();
            break;
        }

        m_dvTreeCtrll->AddLine(current_line, false, (wxUIntPtr)&si);
    }
    clSetDialogBestSizeAndPosition(this);
    m_textCtrlFilter->CallAfter(&wxTextCtrl::SetFocus);
}

LSPOutlineViewDlg::~LSPOutlineViewDlg() {}

void LSPOutlineViewDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSelectionActivate();
}

void LSPOutlineViewDlg::OnTextUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString filter_text = m_textCtrlFilter->GetValue();
    auto match = m_dvTreeCtrll->FindNext({}, filter_text, 0, wxTR_SEARCH_DEFAULT);
    if(match.IsOk()) {
        m_dvTreeCtrll->Select(match);
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
    if(event.GetKeyCode() == WXK_ESCAPE) {
        EndModal(wxID_CANCEL);
    } else {
        event.Skip();
    }
}
