#include "LSPOutlineView.h"

#include "LSP/UI.h"
#include "LSP/LSPManager.hpp"
#include "LSPSymbolStyle.h"
#include "ColoursAndFontsManager.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "globals.h"
#include "imanager.h"
#include "macros.h"

wxDEFINE_EVENT(wxEVT_LSP_OUTLINE_SYMBOL_ACTIVATED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_LSP_OUTLINE_VIEW_DISMISS, wxCommandEvent);

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

LSPOutlineView::LSPOutlineView(wxWindow* parent, wxWindowID id, long style)
    : wxPanel(parent, id), m_style(style)
{
    m_initialiseExpanded = clConfig::Get().Read(kConfigOutlineStartExpanded, true);
    m_keepNamespacesExpanded = clConfig::Get().Read(kConfigOutlineKeepNamespacesExpanded, true);
    m_showDetails = clConfig::Get().Read(kConfigOutlineShowSymbolDetails, true);
    m_showSymbolKind = clConfig::Get().Read(kConfigOutlineShowSymbolKinds, false);
    m_sortType = (LSPSymbolSorter::SortType) clConfig::Get().Read(kConfigOutlineSortType, (int)LSPSymbolSorter::SortType::SORT_LINE);
    CreateUI();      
    DoInitialiseEmpty();     
}

void LSPOutlineView::CreateUI()
{
    wxSizer* s = new wxBoxSizer(wxVERTICAL);
    SetSizer(s);
    CreateToolbar();    
    s->Add(m_toolbar, 0, wxEXPAND);
    
    m_msgPanel = new wxPanel(this, wxID_ANY);
    wxSizer* sMsg = new wxBoxSizer(wxVERTICAL);
    m_msgPanel->SetSizer(sMsg);
    m_msgIndicator = new wxActivityIndicator(m_msgPanel, wxID_ANY);    
    sMsg->Add(m_msgIndicator, 0, wxALL | wxALIGN_CENTER, WXC_FROM_DIP(5));    
    m_msgText = new wxStaticText(m_msgPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);    
    sMsg->Add(m_msgText, 1, wxALL | wxALIGN_CENTER, WXC_FROM_DIP(5));
    s->Add(m_msgPanel, 1, wxEXPAND);
        
    m_terminalViewCtrl = new clTerminalViewCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER|wxDV_ROW_LINES|wxDV_SINGLE);
    s->Add(m_terminalViewCtrl, 1, wxEXPAND);
    
    m_treeCtrl = new clTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE);    
    m_treeCtrl->SetSortFunction(nullptr);
    m_treeCtrl->AddTreeStyle(wxTR_HIDE_ROOT);
    s->Add(m_treeCtrl, 1, wxEXPAND);    
    
    m_terminalViewCtrl->Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &LSPOutlineView::OnItemActivated, this);
    m_terminalViewCtrl->Bind(wxEVT_KEY_DOWN, &LSPOutlineView::OnListKeyDown, this);
    m_treeCtrl->Bind(wxEVT_KEY_DOWN, &LSPOutlineView::OnListKeyDown, this);
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &LSPOutlineView::OnTreeItemActivated, this);  
    Bind(wxEVT_COMMAND_MENU_SELECTED, &LSPOutlineView::OnMenu, this);
}
    
void LSPOutlineView::CreateToolbar()
{
    int tbStyle = wxAUI_TB_PLAIN_BACKGROUND;
    m_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tbStyle);
    
    auto images = clGetManager()->GetStdIcons();
    
    if (!(m_style & STYLE_NO_SEARCHBAR)) {
        m_textCtrlFilter = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        m_textCtrlFilter->Bind(wxEVT_TEXT_ENTER, &LSPOutlineView::OnEnter, this);
        m_textCtrlFilter->Bind(wxEVT_TEXT, &LSPOutlineView::OnTextUpdated, this);
        m_textCtrlFilter->Bind(wxEVT_KEY_DOWN, &LSPOutlineView::OnKeyDown, this);
        auto* iFilter = m_toolbar->AddControl(m_textCtrlFilter, _("Search"));
        iFilter->SetProportion(1);     
        m_toolbar->AddSeparator();
    }
    else {
        m_toolbar->AddStretchSpacer();
    }
    
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
    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_COLLAPSE_ALL, _("Collapse All"), images->LoadBitmap("step_out"), "", wxITEM_NORMAL);    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_EXPAND_ALL, _("Expand All"), images->LoadBitmap("step_in"), "", wxITEM_NORMAL);    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_DETAILS, _("Show Details"), images->LoadBitmap("mime-xml"), "", wxITEM_CHECK);
    m_toolbar->ToggleTool(ID_TOOL_DETAILS, m_showDetails);    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_KIND, _("Show Kind"), images->LoadBitmap("archive"), "", wxITEM_CHECK);
    m_toolbar->ToggleTool(ID_TOOL_KIND, m_showSymbolKind);
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_MENU, _("Show Settings"), images->LoadBitmap("tools"), "", wxITEM_DROPDOWN);       

    m_toolbar->Realize();
    
    m_toolbar->Bind(wxEVT_TOOL, [&](wxCommandEvent& event) {
        if (m_mode != Mode::DOCUMENT_SYMBOL)
            return;
        switch (event.GetId()) {
            case ID_TOOL_DETAILS: {
                m_showDetails = event.IsChecked();                
                clConfig::Get().Write(kConfigOutlineShowSymbolDetails, m_showDetails);
                DoInitialiseDocumentSymbol();
                break;
            }
            case ID_TOOL_KIND: {
                m_showSymbolKind = event.IsChecked();
                clConfig::Get().Write(kConfigOutlineShowSymbolKinds, m_showSymbolKind);
                DoInitialiseDocumentSymbol();
                break;
            }
            case ID_TOOL_COLLAPSE_ALL: {
                CollapseTree();
                break;
            }
            case ID_TOOL_EXPAND_ALL: {
                ExpandTree();
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
        clConfig::Get().Write(kConfigOutlineSortType, (int)sort);
        SortSymbols(sort);
        DoInitialiseDocumentSymbol();
    });
    
    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, [&](wxAuiToolBarEvent& event) {
        if (event.GetToolId() == ID_TOOL_MENU) {
            wxMenu menu;
            wxMenuItem* item = menu.Append(ID_MENU_INITIALISE_EXPANDED, _("Start with expanded tree"), wxEmptyString, wxITEM_CHECK);
            item->Check(m_initialiseExpanded);
            
            item = menu.Append(ID_MENU_KEEP_NAMESPACE_EXPANDED, _("Keep namespaces expanded"), wxEmptyString, wxITEM_CHECK);
            item->Check(m_keepNamespacesExpanded);
            
            PopupMenu(&menu, event.GetItemRect().GetBottomLeft());
        }
        else {
            event.Skip();
        }
    });
}

void LSPOutlineView::DoInitialiseEmpty()
{   
    m_treeCtrl->Hide();
    m_terminalViewCtrl->Hide();
    m_msgPanel->Show();
    m_msgText->SetLabel(m_loadingMessage);
    m_msgIndicator->Start();
    Layout();
   
    return;
}

void LSPOutlineView::AddDocumentSymbolRec(wxTreeItemId parent, const LSP::DocumentSymbol& symbol, LexerConf* lexer) {
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


void LSPOutlineView::DoInitialiseDocumentSymbol()
{   
    m_msgPanel->Hide();
    m_msgIndicator->Stop();
    m_terminalViewCtrl->Hide();
    m_treeCtrl->Show();
    
    auto lexer = ColoursAndFontsManager::Get().GetLexer("python");
    m_treeCtrl->DeleteAllItems();
    if (m_textCtrlFilter)
        m_textCtrlFilter->Clear();

    m_treeCtrl->Begin();
    clColours colours;
    colours.FromLexer(lexer);
    m_treeCtrl->SetColours(colours);
    
    wxFont font = lexer->GetFontForStyle(0, m_treeCtrl);
    if(m_style & STYLE_SMALL_FONT)
        font.Scale(0.8f);
    m_treeCtrl->SetDefaultFont(font);
    
    wxTreeItemId node = m_treeCtrl->AddRoot("Root");
    for (const DocumentSymbol& ds : m_documentSymbols) {
        AddDocumentSymbolRec(node, ds, lexer.get());
    }
    
    m_treeCtrl->Commit();
    if (m_initialiseExpanded)
        m_treeCtrl->ExpandAll();
    else
        CollapseTree();
        
    Layout();
    if (m_textCtrlFilter)
        m_textCtrlFilter->CallAfter(&wxTextCtrl::SetFocus);
}

void LSPOutlineView::DoInitialiseSymbolInformation()
{
    m_msgPanel->Hide();
    m_msgIndicator->Stop();
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
    if (m_textCtrlFilter)
        m_textCtrlFilter->CallAfter(&wxTextCtrl::SetFocus);
}

void LSPOutlineView::OnEnter(wxCommandEvent& event)
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

void LSPOutlineView::OnTextUpdated(wxCommandEvent& event)
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

void LSPOutlineView::OnItemActivated(wxDataViewEvent& event)
{
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);  
    DoActivate(item);
}

void LSPOutlineView::OnTreeItemActivated(wxTreeEvent& event) {
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);
    DoActivate(item);
}

void LSPOutlineView::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode()) {
    case WXK_ESCAPE: {
        auto* evt = new wxCommandEvent(wxEVT_LSP_OUTLINE_VIEW_DISMISS, GetId());
        GetEventHandler()->QueueEvent(evt);
        break;        
    }
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

void LSPOutlineView::DoFindNext()
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

void LSPOutlineView::DoFindPrev()
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

void LSPOutlineView::OnListKeyDown(wxKeyEvent& event)
{
    // key was pressed while tree/terminal view had focus.
    // if it was escape, send a dismiss signal. Otherwise, let the tree handle it.
    if (event.GetKeyCode() == WXK_ESCAPE) {
        auto* evt = new wxCommandEvent(wxEVT_LSP_OUTLINE_VIEW_DISMISS, GetId());
        GetEventHandler()->QueueEvent(evt);
    } else {
        event.Skip();
    }
}

void LSPOutlineView::SetSymbols(const std::vector<SymbolInformation>& symbols, const wxString& filename)
{    
    if (!CheckAndRequest(filename)) {
        ShowMessage(m_loadingMessage, true);
        return;
    }
    m_currentSymbolsFileName = filename;
    m_mode = Mode::SYMBOL_INFORMATION;
    m_symbolsInformation = symbols;
    m_toolbar->EnableTool(ID_TOOL_DETAILS, false);
    m_toolbar->EnableTool(ID_TOOL_KIND, false);
    DoInitialiseSymbolInformation();
}

void LSPOutlineView::SetSymbols(const std::vector<DocumentSymbol>& symbols, const wxString& filename)
{        
    // Clear the tree. Not using ClearView() because we don't want to delete our symbols
    // in case a re-render
    m_treeCtrl->DeleteAllItems();
    
    if (!CheckAndRequest(filename)) {
        ShowMessage(m_loadingMessage, true);
        return;
    }
    
    m_currentSymbolsFileName = filename;
    m_documentSymbols = symbols;
        
    if (symbols.empty()) {
        ShowMessage(_("No Symbols received from language server."), false);
        return;
    }
    
    m_currentSymbolsFileName = filename;
    m_mode = Mode::DOCUMENT_SYMBOL;
    m_documentSymbols = symbols;    
    m_toolbar->EnableTool(ID_TOOL_DETAILS, true);
    m_toolbar->EnableTool(ID_TOOL_KIND, true);
    SortSymbols(m_sortType);
    DoInitialiseDocumentSymbol();
}

void LSPOutlineView::SetEmptySymbols()
{
    m_documentSymbols.clear();
    m_symbolsInformation.clear();
    m_toolbar->EnableTool(ID_TOOL_DETAILS, false);
    m_toolbar->EnableTool(ID_TOOL_KIND, false);
    DoInitialiseEmpty();
}

void LSPOutlineView::DoActivate(const wxDataViewItem& item)
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
    
    auto* evt = new wxCommandEvent(wxEVT_LSP_OUTLINE_SYMBOL_ACTIVATED, GetId());
    evt->SetClientData(static_cast<void*>(si));
    evt->SetInt((int)m_mode);
    evt->SetString(m_currentSymbolsFileName);
    GetEventHandler()->QueueEvent(evt);
}

void LSPOutlineView::DoActivate(const wxTreeItemId& item) {
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
    
    auto* evt = new wxCommandEvent(wxEVT_LSP_OUTLINE_SYMBOL_ACTIVATED, GetId());
    evt->SetClientData(static_cast<void*>(symbol));
    evt->SetInt((int)m_mode);
    evt->SetString(m_currentSymbolsFileName);
    GetEventHandler()->QueueEvent(evt);
}


void LSPOutlineView::SortSymbols(LSPSymbolSorter::SortType sort) 
{    
    LSPSymbolSorter::Sort(m_documentSymbols, sort);
}


void LSPOutlineView::CollapseTree()
{
    m_treeCtrl->CollapseAll();
    
    if (m_keepNamespacesExpanded) {
        // expand namespaces in base level
        wxTreeItemIdValue cookie;
        auto item = m_treeCtrl->GetFirstChild(m_treeCtrl->GetRootItem(), cookie);
        while (item.IsOk()) {        
            auto* data = m_treeCtrl->GetItemData(item);
            auto* symbol = reinterpret_cast<DocumentSymbol*>(data->GetId().m_pItem);
            if (symbol->GetKind() == eSymbolKind::kSK_Namespace)
                m_treeCtrl->Expand(item);
            item = m_treeCtrl->GetNextChild(m_treeCtrl->GetRootItem(), cookie);
        }
    }
}

void LSPOutlineView::ExpandTree()
{
    m_treeCtrl->ExpandAll();
}

void LSPOutlineView::ClearView()
{
    m_currentSymbolsFileName.clear();
    m_terminalViewCtrl->DeleteAllItems();
    m_treeCtrl->DeleteAllItems();
    m_documentSymbols.clear();
    m_symbolsInformation.clear();
}

void LSPOutlineView::ShowMessage(const wxString& message, bool showLoadingIndicator) {
    m_treeCtrl->Hide();
    m_terminalViewCtrl->Hide();
    m_msgText->SetLabel(message);
    m_msgIndicator->Show(showLoadingIndicator);
    if(showLoadingIndicator) m_msgIndicator->Start();
    m_msgPanel->Show();
}

void LSPOutlineView::HideMessage() {
    m_msgIndicator->Stop();
    m_msgPanel->Hide();
}


bool LSPOutlineView::CheckAndRequest(const wxString& filename) {
    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET_FALSE(editor);

    wxString remote_path;
    if (editor->IsRemoteFile()) {
        remote_path = editor->GetRemotePath();
    }
    wxString local_path = editor->GetFileName().GetFullPath();
    
    if (local_path != filename && remote_path != filename) {
        // The symbols do not match the active editor
        // Request symbols for the current editor.
        
        LSP_DEBUG() << "LSPOutlineView::CheckAndRequest: Requesting symbols" << endl;
        LSP::Manager::GetInstance().RequestSymbolsForEditor(editor, nullptr);
        return false;
    }
    
    return true;
}


void LSPOutlineView::OnMenu(wxCommandEvent& event)
{
    switch(event.GetId()) {
        case ID_MENU_INITIALISE_EXPANDED: {
            m_initialiseExpanded = event.IsChecked();
            if (m_initialiseExpanded)
               ExpandTree();
            else   
                CollapseTree();
                
            clConfig::Get().Write(kConfigOutlineStartExpanded, m_initialiseExpanded);
            break;
        }
        case ID_MENU_KEEP_NAMESPACE_EXPANDED: {
            m_keepNamespacesExpanded = event.IsChecked();
            clConfig::Get().Write(kConfigOutlineKeepNamespacesExpanded, m_keepNamespacesExpanded);
            break;
        }
    }
}