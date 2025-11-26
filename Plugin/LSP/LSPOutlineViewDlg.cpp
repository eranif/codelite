#include "LSPOutlineViewDlg.h"

#include "ColoursAndFontsManager.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "drawingutils.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "LSPSymbolStyle.h"

#include <stack>
#include <wx/dataview.h>

namespace {
    class wxIntClientData : public wxClientData {
        int m_value = 0;
    public: 
        wxIntClientData(int value) : wxClientData(), m_value(value) {}
        ~wxIntClientData() = default;
        
        int GetValue() const { return m_value; }
        void SetValue(int value) { m_value = value; }
    };
}

LSPOutlineViewDlg::LSPOutlineViewDlg(wxWindow* parent)
    : LSPOutlineViewDlgBase(parent)
{
    CreateToolbar();
    m_treeCtrl->SetSortFunction(nullptr);
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &LSPOutlineViewDlg::OnTreeItemActivated, this);    
    
    DoInitialiseEmpty();
    clSetDialogBestSizeAndPosition(this);       
}

void LSPOutlineViewDlg::CreateToolbar()
{
    int style = wxAUI_TB_PLAIN_BACKGROUND;
    m_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);
    
    auto images = clGetManager()->GetStdIcons();
    m_textCtrlFilter = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_textCtrlFilter->Bind(wxEVT_TEXT_ENTER, &LSPOutlineViewDlg::OnEnter, this);
    m_textCtrlFilter->Bind(wxEVT_TEXT, &LSPOutlineViewDlg::OnTextUpdated, this);
    m_textCtrlFilter->Bind(wxEVT_KEY_DOWN, &LSPOutlineViewDlg::OnKeyDown, this);
    auto* iFilter = m_toolbar->AddControl(m_textCtrlFilter, _("Search"));
    iFilter->SetProportion(1);     
    m_toolbar->AddSeparator();
    
    wxSize size = GetTextExtent("Type/Name (case sensitive)");
    m_sortOptions = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(), -1));
    m_sortOptions->Append(_("Type/Name"), new wxIntClientData(LSPSymbolSorter::SORT_KIND_NAME));
    m_sortOptions->Append(_("Name"), new wxIntClientData(LSPSymbolSorter::SORT_NAME));
    m_sortOptions->Append(_("Line"), new wxIntClientData(LSPSymbolSorter::SORT_LINE));
    m_sortOptions->Append(_("Type/Name (case sensitive)"), new wxIntClientData(LSPSymbolSorter::SORT_KIND_NAME_CASE_SENSITIVE));
    m_sortOptions->Append(_("Name (case sensitive)"), new wxIntClientData(LSPSymbolSorter::SORT_NAME_CASE_SENSITIVE));   
    // find default selection
    for (size_t i = 0; i < m_sortOptions->GetCount(); i++) {
        auto* data = static_cast<wxIntClientData*>(m_sortOptions->GetClientObject(i));
        if (data && data->GetValue() == (int)m_sortType) {            
            m_sortOptions->SetSelection(i);
            break;
        }
    }
    m_toolbar->AddLabel(wxID_ANY, _("Sort:"));
    m_toolbar->AddControl(m_sortOptions, _("Sort"));
    m_toolbar->AddSeparator();
    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_DETAILS, _("Show Details"), images->LoadBitmap("mime-xml"), "", wxITEM_CHECK);
    m_toolbar->ToggleTool(ID_TOOL_DETAILS, m_showDetails);
    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_KIND, _("Show Kind"), images->LoadBitmap("archive"), "", wxITEM_CHECK);
    m_toolbar->ToggleTool(ID_TOOL_KIND, m_showSymbolKind);

    m_toolbar->Realize();
    
    m_toolbar->Bind(wxEVT_TOOL, [&](wxCommandEvent& event) {
        if (m_mode != Mode::DOCUMENT_SYMBOL)
            return;
        switch (event.GetId()) {
            case ID_TOOL_DETAILS: {
                m_showDetails = event.IsChecked();
                DoInitialiseDocumentSymbol();
                break;
            }
            case ID_TOOL_KIND: {
                m_showSymbolKind = event.IsChecked();
                DoInitialiseDocumentSymbol();
                break;
            }
            default: {}
        }
    });
    
    m_sortOptions->Bind(wxEVT_CHOICE, [&](wxCommandEvent& event) {
        if (m_mode != Mode::DOCUMENT_SYMBOL)
            return;
            
        auto* data = static_cast<wxIntClientData*>(event.GetClientObject());
        if (!data)
            return;
        LSPSymbolSorter::SortType sort = (LSPSymbolSorter::SortType)data->GetValue();
        SortSymbols(sort);
        DoInitialiseDocumentSymbol();
    });
}

void LSPOutlineViewDlg::DoInitialiseEmpty()
{   
    m_treeCtrl->Hide();
    m_terminalViewCtrl->Hide();
    m_msgPanel->Show();
    m_staticText->SetLabel(_("Language Server is still not ready...\n(hit ESCAPE key to dismiss)"));
    m_activityCtrl->Start();
    Layout();
   
    return;
}

void LSPOutlineViewDlg::AddDocumentSymbolRec(wxTreeItemId parent, const LSP::DocumentSymbol& symbol, LexerConf* lexer) {
    LSPSymbolStyle style(symbol.GetKind(), lexer);
    wxString name = symbol.CreateNameString(style.icon, false, m_showDetails, m_showSymbolKind);
        
    wxTreeItemData* data = new wxTreeItemData();
    data->SetId( wxTreeItemId((void*)&symbol) );
    
    // add symbol to tree
    auto node = m_treeCtrl->AppendItem(parent, name, -1, -1, data);
    m_treeCtrl->SetItemBold(node, style.bold);
    m_treeCtrl->SetItemTextColour(node, style.colour);
    
    // recursively add children
    for(const auto& child : symbol.GetChildren()) {
        AddDocumentSymbolRec(node, child, lexer);
    }    
}

void LSPOutlineViewDlg::DoInitialiseDocumentSymbol()
{   
    m_msgPanel->Hide();
    m_activityCtrl->Stop();
    m_terminalViewCtrl->Hide();
    m_treeCtrl->Show();
    
    auto lexer = ColoursAndFontsManager::Get().GetLexer("python");
    m_treeCtrl->DeleteAllItems();
    m_textCtrlFilter->Clear();

    m_treeCtrl->Begin();
    clColours colours;
    colours.FromLexer(lexer);
    m_treeCtrl->SetColours(colours);
    m_treeCtrl->AddTreeStyle(wxTR_HIDE_ROOT);
    
    wxFont font = lexer->GetFontForStyle(0, m_treeCtrl);
    m_treeCtrl->SetDefaultFont(font);
    
    wxTreeItemId node = m_treeCtrl->AddRoot("Root");
    for (const DocumentSymbol& ds : m_documentSymbols) {
        AddDocumentSymbolRec(node, ds, lexer.get());
    }
    
    m_treeCtrl->Commit();
    m_treeCtrl->ExpandAll();
    Layout();
    m_textCtrlFilter->CallAfter(&wxTextCtrl::SetFocus);
}

void LSPOutlineViewDlg::DoInitialiseSymbolInformation()
{
    m_msgPanel->Hide();
    m_activityCtrl->Stop();
    m_terminalViewCtrl->Show();
    m_treeCtrl->Hide();
    
    auto lexer = ColoursAndFontsManager::Get().GetLexer("python");
    m_terminalViewCtrl->DeleteAllItems();
    m_terminalViewCtrl->SetSortFunction(nullptr);
    m_textCtrlFilter->Clear();

    m_terminalViewCtrl->Begin();
    m_terminalViewCtrl->SetScrollToBottom(false);

    clColours colours;
    colours.FromLexer(lexer);
    m_terminalViewCtrl->SetColours(colours);

    // build the tree
    wxColour operator_colour = lexer->GetProperty(wxSTC_P_OPERATOR).GetFgColour();
    std::vector<std::pair<wxString, int>> containers;

    clAnsiEscapeCodeColourBuilder builder;
    for (const SymbolInformation& si : m_symbolsInformation) {
        const wxString& symbol_container = si.GetContainerName();
        if (symbol_container.empty()) {
            containers.push_back({ si.GetName(), 1 });
        } else {
            int parent_depth = 0;
            while (!containers.empty()) {
                if (containers.back().first == symbol_container) {
                    parent_depth = containers.back().second;
                    break;
                }
                containers.pop_back();
            }
            containers.push_back({ si.GetName(), parent_depth + 1 });
        }

        builder.Clear();
   
        LSPSymbolStyle style(si.GetKind(), lexer.get());        
        switch (si.GetKind()) {
            case kSK_File:
            case kSK_Module:
            case kSK_Package:
                builder.Add(style.icon + " ", AnsiColours::NormalText());
                builder.Add(si.GetName(), style.colour);
                break;
            case kSK_Class:
            case kSK_Struct:
            case kSK_Interface:
            case kSK_Object:
            case kSK_Enum:
                builder.Add(style.icon + " ", AnsiColours::NormalText());
                builder.Add(si.GetName(), style.colour, true);
                break;
            case kSK_Method:
            case kSK_Function:
            case kSK_Constructor:
                builder.Add(style.icon + " ", AnsiColours::NormalText());
                if (si.GetName().Contains("(") && si.GetName().Contains(")")) {
                    // the name also has the signature
                    wxString signature = si.GetName().AfterFirst('(');
                    signature = signature.BeforeLast(')');
                    wxString name_only = si.GetName().BeforeFirst('(');
                    builder.Add(name_only, style.colour);
                    builder.Add("(", operator_colour);
                    builder.Add(signature, AnsiColours::NormalText());
                    builder.Add(")", operator_colour);
                } else {
                    builder.Add(si.GetName(), style.colour);
                    builder.Add("()", operator_colour);
                }
                break;
            case kSK_TypeParameter: // define
            case kSK_EnumMember:
                builder.Add(style.icon + " ", AnsiColours::NormalText());
                builder.Add(si.GetName(), AnsiColours::NormalText());
                break;
            default:
                builder.Add(style.icon + " ", AnsiColours::NormalText());
                builder.Add(si.GetName(), style.colour);
                break;
        }
        m_terminalViewCtrl->AddLine(builder.GetString(), false, (wxUIntPtr)&si);
    }
    if (!m_terminalViewCtrl->IsEmpty()) {
        m_terminalViewCtrl->SelectRow(0);
    }
    m_terminalViewCtrl->Commit();
    Layout();
    m_textCtrlFilter->CallAfter(&wxTextCtrl::SetFocus);
}

void LSPOutlineViewDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);    
    if (m_mode == Mode::DOCUMENT_SYMBOL) {     
        auto selection = m_treeCtrl->GetSelection();
        CHECK_ITEM_RET(selection);
        DoActivate(selection);        
    }
    else {        
        auto selection = m_terminalViewCtrl->GetSelection();
        CHECK_ITEM_RET(selection);
        DoActivate(selection);
    }
}

void LSPOutlineViewDlg::OnTextUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString filter_text = m_textCtrlFilter->GetValue();
    if (m_mode == Mode::DOCUMENT_SYMBOL) {
        m_treeCtrl->ClearAllHighlights();
        wxTreeItemId starting_item = m_treeCtrl->GetSelection().IsOk() ? m_treeCtrl->GetSelection() : wxTreeItemId( nullptr );
        auto match = m_treeCtrl->FindNext(starting_item, filter_text, 0, wxTR_SEARCH_DEFAULT);
        if (match.IsOk()) {
            m_treeCtrl->SelectItem(match);
            m_treeCtrl->HighlightText(match, true);
            m_treeCtrl->EnsureVisible(match);
        }
    }
    else {
        m_terminalViewCtrl->ClearAllHighlights();
        wxDataViewItem starting_item =
            m_terminalViewCtrl->GetSelection().IsOk() ? m_terminalViewCtrl->GetSelection() : wxDataViewItem{ nullptr };
        auto match = m_terminalViewCtrl->FindNext(starting_item, filter_text, 0, wxTR_SEARCH_DEFAULT);
        if (match.IsOk()) {
            m_terminalViewCtrl->Select(match);
            m_terminalViewCtrl->HighlightText(match, true);
            m_terminalViewCtrl->EnsureVisible(match);
        }
    }    
}

void LSPOutlineViewDlg::OnItemActivated(wxDataViewEvent& event)
{
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);  
    DoActivate(item);
}

void LSPOutlineViewDlg::OnTreeItemActivated(wxTreeEvent& event) {
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);
    DoActivate(item);
}

void LSPOutlineViewDlg::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode()) {
    case WXK_ESCAPE:
        Hide();
        break;
    case WXK_DOWN:
        DoFindNext();
        break;
    case WXK_UP:
        DoFindPrev();
        break;
    default: {
        int modifier_key = event.GetModifiers();
        wxChar ch = event.GetUnicodeKey();
        if (modifier_key == wxMOD_CONTROL && ch == 'U') {
            m_terminalViewCtrl->PageUp();
            DoFindNext();
        } else if (modifier_key == wxMOD_CONTROL && ch == 'D') {
            m_terminalViewCtrl->PageDown();
            DoFindPrev();
        } else if (modifier_key == wxMOD_CONTROL && (ch == 'J' || ch == 'N')) {
            DoFindNext();
        } else if (modifier_key == wxMOD_CONTROL && (ch == 'K' || ch == 'P')) {
            DoFindPrev();
        } else {
            event.Skip();
        }
        break;
    }
    }
}

void LSPOutlineViewDlg::DoFindNext()
{
    if (m_mode == Mode::DOCUMENT_SYMBOL) {
        m_treeCtrl->ClearAllHighlights();

        wxTreeItemId sel_item = m_treeCtrl->GetSelection();
        wxTreeItemId next_item = m_treeCtrl->GetNextItem(sel_item);
        wxString find_what = m_textCtrlFilter->GetValue();
        if (find_what.empty()) {
            m_treeCtrl->SelectItem(next_item);
            m_treeCtrl->EnsureVisible(next_item);
        } else {
            wxTreeItemId item = m_treeCtrl->FindNext(next_item, find_what, 0, wxTR_SEARCH_DEFAULT);
            CHECK_ITEM_RET(item);
            m_treeCtrl->SelectItem(item);
            m_treeCtrl->EnsureVisible(item);
        }
    }
    else {        
        m_terminalViewCtrl->ClearAllHighlights();

        std::size_t sel_row = m_terminalViewCtrl->GetSelectedRow();
        if ((sel_row + 1) >= m_terminalViewCtrl->GetItemCount()) {
            return;
        }

        wxDataViewItem next_item = m_terminalViewCtrl->RowToItem(sel_row + 1);
        wxString find_what = m_textCtrlFilter->GetValue();
        if (find_what.empty()) {
            m_terminalViewCtrl->Select(next_item);
            m_terminalViewCtrl->EnsureVisible(next_item);
        } else {
            wxDataViewItem item = m_terminalViewCtrl->FindNext(next_item, find_what, 0, wxTR_SEARCH_DEFAULT);
            CHECK_ITEM_RET(item);
            m_terminalViewCtrl->Select(item);
            m_terminalViewCtrl->EnsureVisible(item);
        }
    }
}

void LSPOutlineViewDlg::DoFindPrev()
{
    if (m_mode == Mode::DOCUMENT_SYMBOL) {
        m_treeCtrl->ClearAllHighlights();

        wxTreeItemId sel_item = m_treeCtrl->GetSelection();
        wxTreeItemId prev_item = m_treeCtrl->GetPrevItem(sel_item);
        wxString find_what = m_textCtrlFilter->GetValue();
        if (find_what.empty()) {
            m_treeCtrl->SelectItem(prev_item);
            m_treeCtrl->EnsureVisible(prev_item);
        } else {
            wxTreeItemId item = m_treeCtrl->FindPrev(prev_item, find_what, 0, wxTR_SEARCH_DEFAULT);
            CHECK_ITEM_RET(item);
            m_treeCtrl->SelectItem(item);
            m_treeCtrl->EnsureVisible(item);
        }
    }
    else {   
        m_terminalViewCtrl->ClearAllHighlights();

        int sel_row = m_terminalViewCtrl->GetSelectedRow();
        if (sel_row < 1) {
            return;
        }

        wxDataViewItem prev_item = m_terminalViewCtrl->RowToItem(sel_row - 1);
        wxString find_what = m_textCtrlFilter->GetValue();
        if (find_what.empty()) {
            m_terminalViewCtrl->Select(prev_item);
            m_terminalViewCtrl->EnsureVisible(prev_item);
        } else {
            wxDataViewItem item = m_terminalViewCtrl->FindPrev(prev_item, find_what, 0, wxTR_SEARCH_DEFAULT);
            CHECK_ITEM_RET(item);
            m_terminalViewCtrl->Select(item);
            m_terminalViewCtrl->EnsureVisible(item);
        }
    }
}

void LSPOutlineViewDlg::OnListKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE) {
        Hide();
    } else {
        event.Skip();
    }
}

void LSPOutlineViewDlg::SetSymbols(const std::vector<SymbolInformation>& symbols)
{
    m_mode = Mode::SYMBOL_INFORMATION;
    m_symbolsInformation = symbols;
    m_toolbar->EnableTool(ID_TOOL_DETAILS, false);
    m_toolbar->EnableTool(ID_TOOL_KIND, false);
    DoInitialiseSymbolInformation();
}

void LSPOutlineViewDlg::SetSymbols(const std::vector<DocumentSymbol>& symbols)
{
    m_mode = Mode::DOCUMENT_SYMBOL;
    m_documentSymbols = symbols;    
    m_toolbar->EnableTool(ID_TOOL_DETAILS, true);
    m_toolbar->EnableTool(ID_TOOL_KIND, true);
    SortSymbols(m_sortType);
    DoInitialiseDocumentSymbol();
}

void LSPOutlineViewDlg::SetEmptySymbols()
{
    m_documentSymbols.clear();
    m_symbolsInformation.clear();
    m_toolbar->EnableTool(ID_TOOL_DETAILS, false);
    m_toolbar->EnableTool(ID_TOOL_KIND, false);
    DoInitialiseEmpty();
}

void LSPOutlineViewDlg::DoActivate(const wxDataViewItem& item)
{
    LSP::SymbolInformation* si = reinterpret_cast<LSP::SymbolInformation*>(m_terminalViewCtrl->GetItemData(item));
    CHECK_PTR_RET(si);

    // open the editor and go to
    LSP::Location loc = si->GetLocation();
    IEditor* active_editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(active_editor);

    int sci_line = loc.GetRange().GetStart().GetLine();
    if (loc.GetRange().GetStart().GetLine() != loc.GetRange().GetEnd().GetLine()) {
        // different lines, don't select the entire function
        // just place the caret at the beginning of the function
        int position = active_editor->PosFromLine(sci_line);  // start of line
        position += loc.GetRange().GetStart().GetCharacter(); // add the column
        active_editor->SetCaretAt(position);
        active_editor->CenterLine(sci_line);
    } else {
        active_editor->SelectRangeAfter(loc.GetRange());
        active_editor->CenterLinePreserveSelection(sci_line);
    }
    Hide();
}

void LSPOutlineViewDlg::DoActivate(const wxTreeItemId& item) {
    wxTreeItemData* data = m_treeCtrl->GetItemData(item);
    CHECK_PTR_RET(data);        

    LSP::DocumentSymbol* symbol = reinterpret_cast<LSP::DocumentSymbol*>(data->GetId().m_pItem);
    CHECK_PTR_RET(symbol);
    
    // open the editor and go to
    IEditor* active_editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(active_editor);

    int sci_line = symbol->GetRange().GetStart().GetLine();
    int position = active_editor->PosFromLine(sci_line);  // start of line
    position += symbol->GetRange().GetStart().GetCharacter(); // add the column
    active_editor->SetCaretAt(position);
    active_editor->CenterLine(sci_line);
    
    Hide();
}


void LSPOutlineViewDlg::SortSymbols(LSPSymbolSorter::SortType sort) 
{    
    LSPSymbolSorter::Sort(m_documentSymbols, sort);
}