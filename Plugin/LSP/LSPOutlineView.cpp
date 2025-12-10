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
    // initialize behaviour from config
    m_initialiseAllCollapsed = clConfig::Get().Read(kConfigOutlineStartAllCollapsed, false);
    m_initialiseClassesCollapsed = clConfig::Get().Read(kConfigOutlineStartClassesCollapsed, false);
    m_initialiseSubclassesCollapsed = clConfig::Get().Read(kConfigOutlineStartSubclassesCollapsed, true);
    m_initialiseEnumsCollapsed = clConfig::Get().Read(kConfigOutlineStartEnumsCollapsed, true);
    m_initialiseFunctionsCollapsed = clConfig::Get().Read(kConfigOutlineStartFunctionsCollapsed, true);
    m_initialiseContainersCollapsed = clConfig::Get().Read(kConfigOutlineStartContainersCollapsed, false);
    m_initialiseVariablesCollapsed = clConfig::Get().Read(kConfigOutlineStartVariablesCollapsed, true);
    m_keepNamespacesExpanded = clConfig::Get().Read(kConfigOutlineKeepNamespacesExpanded, true);
    m_showDetails = clConfig::Get().Read(kConfigOutlineShowSymbolDetails, true);
    m_showSymbolKind = clConfig::Get().Read(kConfigOutlineShowSymbolKinds, false);
    m_forceTree = clConfig::Get().Read(kConfigOutlineForceTree, true);
    m_sortType = (LSPSymbolParser::SortType) clConfig::Get().Read(kConfigOutlineSortType, (int)LSPSymbolParser::SortType::SORT_LINE);
    
    // load bitmaps
    auto images = clGetManager()->GetStdIcons();
    m_bitmaps.reserve((int)LSPSymbolStyle::BITMAP_MAX);    
    // order has to be same as LSPSymbolStyle::BITMAP enum
    m_bitmaps.push_back(images->LoadBitmap("symbol-module"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-enum"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-enummember"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-class"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-struct"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-function"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-constructor"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-variable"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-container"));
    m_bitmaps.push_back(images->LoadBitmap("symbol-namespace"));
    
    // create controls
    CreateUI();      
    // initialize content (empty)
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
    m_treeCtrl->SetBitmaps(&m_bitmaps);
    m_treeCtrl->SetSortFunction(nullptr);
    m_treeCtrl->AddTreeStyle(wxTR_HIDE_ROOT);
    s->Add(m_treeCtrl, 1, wxEXPAND);    
    
    m_terminalViewCtrl->Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &LSPOutlineView::OnItemActivated, this);
    m_terminalViewCtrl->Bind(wxEVT_KEY_DOWN, &LSPOutlineView::OnListKeyDown, this);
    m_treeCtrl->Bind(wxEVT_KEY_DOWN, &LSPOutlineView::OnListKeyDown, this);
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &LSPOutlineView::OnTreeItemActivated, this);  
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_EXPANDED, &LSPOutlineView::OnTreeItemExpanded, this);  
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_COLLAPSED, &LSPOutlineView::OnTreeItemCollapsed, this);  
    m_treeCtrl->Bind(wxEVT_TREE_SEL_CHANGED, &LSPOutlineView::OnTreeItemSelected, this);  
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
    m_sortOptions = new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(), -1));
    m_sortOptions->SetToolTip(_("Sort symbols"));
    m_sortOptions->Append(_("Kind/Name"), new wxIntClientData(LSPSymbolParser::SORT_KIND_NAME));
    m_sortOptions->Append(_("Name"), new wxIntClientData(LSPSymbolParser::SORT_NAME));
    m_sortOptions->Append(_("Line"), new wxIntClientData(LSPSymbolParser::SORT_LINE));
    m_sortOptions->Append(_("Kind/Name (case sensitive)"), new wxIntClientData(LSPSymbolParser::SORT_KIND_NAME_CASE_SENSITIVE));
    m_sortOptions->Append(_("Name (case sensitive)"), new wxIntClientData(LSPSymbolParser::SORT_NAME_CASE_SENSITIVE));   
    // find default selection
    for (size_t i = 0; i < m_sortOptions->GetCount(); i++) {
        auto* data = static_cast<wxIntClientData*>(m_sortOptions->GetClientObject(i));
        if (data && data->GetValue() == (int)m_sortType) {            
            m_sortOptions->SetSelection(i);
            break;
        }
    }
    m_toolbar->AddControl(m_sortOptions);
    m_toolbar->AddSeparator();
    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_EXPAND_ALL, _("Expand All"), images->LoadBitmap("expand"), "", wxITEM_NORMAL);    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_COLLAPSE_ALL, _("Collapse All"), images->LoadBitmap("collapse"), "", wxITEM_NORMAL);    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_DETAILS, _("Show Details"), images->LoadBitmap("detail"), "", wxITEM_CHECK);
    m_toolbar->ToggleTool(ID_TOOL_DETAILS, m_showDetails);    
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_KIND, _("Show Kind"), images->LoadBitmap("type"), "", wxITEM_CHECK);
    m_toolbar->ToggleTool(ID_TOOL_KIND, m_showSymbolKind);
    clAuiToolBarArt::AddTool(m_toolbar, ID_TOOL_MENU, _("Show Settings"), images->LoadBitmap("tools"), "", wxITEM_DROPDOWN);       

    m_toolbar->Realize();
    
    m_toolbar->Bind(wxEVT_TOOL, &LSPOutlineView::OnToolBar, this);    
    m_sortOptions->Bind(wxEVT_CHOICE, &LSPOutlineView::OnSortChanged, this);    
    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &LSPOutlineView::OnToolBarDropdown, this);
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
    auto images = clGetManager()->GetStdIcons();
    LSPSymbolStyle style(symbol.GetKind(), lexer);
    wxString name = symbol.CreateNameString("", false, m_showDetails, m_showSymbolKind);
        
    wxTreeItemData* data = new wxTreeItemData();
    data->SetId( wxTreeItemId((void*)&symbol) );
    
    // add symbol to tree
    auto node = m_treeCtrl->AppendItem(parent, name, style.bitmap, -1, data);
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
    
    // expand or collapse the tree according to config setting
    InitExpandedState();
    
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_EXPANDED, &LSPOutlineView::OnTreeItemExpanded, this);  
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_COLLAPSED, &LSPOutlineView::OnTreeItemCollapsed, this);  
    
    // items in m_treeState are DocumentSymbols wrapped in wxTreeItemId
    // the tree cannot directly use these, we have to find the corresponding row recursively first
    // easier to iterate the whole tree and check if each item is in our m_treeState
    RestoreTreeStateRec(m_treeCtrl->GetRootItem());
    
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_EXPANDED, &LSPOutlineView::OnTreeItemExpanded, this);  
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_COLLAPSED, &LSPOutlineView::OnTreeItemCollapsed, this);  
        
    Layout();
    if (m_textCtrlFilter)
        m_textCtrlFilter->CallAfter(&wxTextCtrl::SetFocus);
}

void LSPOutlineView::RestoreTreeStateRec(wxTreeItemId parent)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrl->GetFirstChild(parent, cookie);
    while (child.IsOk()) {
        // get the DocumentSymbol
        auto* data = m_treeCtrl->GetItemData(child);
        
        if (data->GetId() == m_treeState.selectedNode) {
            m_treeCtrl->SelectItem(child); 
            m_treeCtrl->EnsureVisible(child);           
        }
        
        auto it2 = std::find(m_treeState.collapsedNodes.begin(), m_treeState.collapsedNodes.end(), data->GetId());
        if (it2 != m_treeState.collapsedNodes.end())
            m_treeCtrl->Collapse(child);
            
        auto it = std::find(m_treeState.expandedNodes.begin(), m_treeState.expandedNodes.end(), data->GetId());
        if (it != m_treeState.expandedNodes.end())
            m_treeCtrl->Expand(child);        
            
        if (m_treeCtrl->GetChildrenCount(child, false) > 0) {
            // recursively restore the state
            RestoreTreeStateRec(child);
        }
        
        child = m_treeCtrl->GetNextChild(parent, cookie);
    }
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
    if (m_textCtrlFilter)
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
        if (m_showSymbolKind) {            
            builder.Add(wxString::Format(" [%s]", SymbolKindToString(si.GetKind())), AnsiColours::NormalText());
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
    if (m_mode == Mode::DOCUMENT_SYMBOL || m_mode == Mode::SYMBOL_INFORMATION_PARSED) {     
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
    if (m_mode == Mode::DOCUMENT_SYMBOL || m_mode == Mode::SYMBOL_INFORMATION_PARSED) {
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

void LSPOutlineView::OnTreeItemSelected(wxTreeEvent& event) {
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);
    auto* data = m_treeCtrl->GetItemData(item);
    m_treeState.selectedNode = data->GetId();
    if (m_style & STYLE_ACTIVATE_ON_SELECTION)
        DoActivate(item);
}

void LSPOutlineView::OnTreeItemActivated(wxTreeEvent& event) {
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);
    auto* data = m_treeCtrl->GetItemData(item);
    m_treeState.selectedNode = data->GetId();
    if (m_treeCtrl->GetChildrenCount(item, false) > 0) {
        if (m_treeCtrl->IsExpanded(item))
            m_treeCtrl->Collapse(item);
        else
            m_treeCtrl->Expand(item);
    }
    else {
        DoActivate(item);
    }
}

void LSPOutlineView::OnTreeItemExpanded(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);
    auto* data = m_treeCtrl->GetItemData(item);
    auto symbol = data->GetId();
    auto it = std::find(m_treeState.expandedNodes.begin(), m_treeState.expandedNodes.end(), symbol);
    if (it == m_treeState.expandedNodes.end())
        m_treeState.expandedNodes.push_back(symbol);
        
    std::erase(m_treeState.collapsedNodes, symbol);
        
    event.Skip();
}

void LSPOutlineView::OnTreeItemCollapsed(wxTreeEvent& event)
{   
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);
    auto* data = m_treeCtrl->GetItemData(item);
    auto symbol = data->GetId();
    auto it = std::find(m_treeState.collapsedNodes.begin(), m_treeState.collapsedNodes.end(), symbol);
    if (it == m_treeState.collapsedNodes.end())
        m_treeState.collapsedNodes.push_back(symbol);
    std::erase(m_treeState.expandedNodes, symbol);
    event.Skip();
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
    if (m_mode == Mode::DOCUMENT_SYMBOL || m_mode == Mode::SYMBOL_INFORMATION_PARSED) {
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
    if (m_mode == Mode::DOCUMENT_SYMBOL || m_mode == Mode::SYMBOL_INFORMATION_PARSED) {
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
    m_symbolsInformation = symbols;    
    m_toolbar->EnableTool(ID_TOOL_DETAILS, false);
    m_toolbar->EnableTool(ID_TOOL_KIND, true);    
    m_treeState = TreeState();
    if (m_forceTree) {
        m_documentSymbols = LSPSymbolParser::Parse(symbols);
        m_mode = Mode::SYMBOL_INFORMATION_PARSED;
        DoInitialiseDocumentSymbol();
    }
    else {        
        m_mode = Mode::SYMBOL_INFORMATION;
        DoInitialiseSymbolInformation();
    }
    
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
    m_treeState = TreeState();
    
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

    LSP::DocumentSymbol* symbol = static_cast<LSP::DocumentSymbol*>(data->GetId().m_pItem);
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


void LSPOutlineView::SortSymbols(LSPSymbolParser::SortType sort) 
{
    using vec_t = std::vector<LSP::DocumentSymbol>;
    // sorting the symbols vector invalidates our TreeState, which uses pointers to the symbols
    // thus we need to copy the current collapsed/expanded/selected state first and restore it after sorting
    
    // map tree items to incomplete shallow copies of their referenced symbols
    auto mapShallow = [](const std::vector<wxTreeItemId>& items) -> vec_t {        
        vec_t target;
        target.reserve(items.size());
        for (const auto& item : items) {
            DocumentSymbol* s = static_cast<DocumentSymbol*>(item.GetID());
            auto it = target.insert(target.end(), LSP::DocumentSymbol());
            it->SetName(s->GetName());
            it->SetRange(s->GetRange());
            it->SetKind(s->GetKind());
        }
        return target;
    };
    
    // compare if two symbols are the same based on name, kind and range
    auto isSame = [](const DocumentSymbol& a, const DocumentSymbol& b) -> bool {
        return  a.GetName() == b.GetName() && 
                a.GetKind() == b.GetKind() &&
                a.GetRange().GetStart() == b.GetRange().GetStart() &&
                a.GetRange().GetEnd() == b.GetRange().GetEnd();
    };
    
    // recursive function to set the expanded state again after the tree was rebuild
    std::function<void(DocumentSymbol&, const vec_t&, const vec_t&, const DocumentSymbol&)> rebuildStateRec = 
        [&](DocumentSymbol& symbol, const vec_t& expanded, const vec_t& collapsed, const DocumentSymbol& selected) 
    {
        // selected previously selected node again
        if (isSame(symbol, selected)) {
            m_treeState.selectedNode = wxTreeItemId(&symbol);
        }
                
        if (symbol.GetChildren().size() > 0) {            
            // if the node was previously expanded, do it again
            for (const auto& other : expanded) {
                if (isSame(symbol, other)) {
                    m_treeState.expandedNodes.push_back(wxTreeItemId(&symbol));
                    break;
                }
            }
            // if the node was previously collapsed, do it again
            for (const auto& other : collapsed) {
                if (isSame(symbol, other)) {
                    m_treeState.collapsedNodes.push_back(wxTreeItemId(&symbol));
                    break;
                }
            }
            // recursion
            for (auto& child : symbol.GetChildren()) {
                rebuildStateRec(child, expanded, collapsed, selected);
            }
        }
    };
    
    // copy existing state
    vec_t expanded = mapShallow(m_treeState.expandedNodes);
    vec_t collapsed = mapShallow(m_treeState.collapsedNodes);
    DocumentSymbol selected;
    if (m_treeState.selectedNode.IsOk()) {
        auto* selectedPtr = static_cast<DocumentSymbol*>(m_treeState.selectedNode.GetID());
        selected = *selectedPtr;
    }    
    
    // clear old state
    m_treeState.expandedNodes.clear();
    m_treeState.collapsedNodes.clear();
    
    // perform sort
    switch (m_mode) {
        case Mode::DOCUMENT_SYMBOL: wxFALLTHROUGH;
        case Mode::SYMBOL_INFORMATION_PARSED:
            LSPSymbolParser::Sort(m_documentSymbols, sort);
            break;
        case Mode::SYMBOL_INFORMATION:
            LSPSymbolParser::Sort(m_symbolsInformation, sort);
            break;
    }
    
    // restore state
    for (auto& symbol : m_documentSymbols) {
        rebuildStateRec(symbol, expanded, collapsed, selected);
    }
}


void LSPOutlineView::CollapseTree()
{ 
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_COLLAPSED, &LSPOutlineView::OnTreeItemCollapsed, this);  
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_EXPANDED, &LSPOutlineView::OnTreeItemExpanded, this);  
    m_treeCtrl->CollapseAll();
    if (m_keepNamespacesExpanded) {
        // expand namespaces in base level
        wxTreeItemIdValue cookie;
        auto item = m_treeCtrl->GetFirstChild(m_treeCtrl->GetRootItem(), cookie);
        while (item.IsOk()) {        
            auto* data = m_treeCtrl->GetItemData(item);
            auto* symbol = static_cast<DocumentSymbol*>(data->GetId().m_pItem);
            if (symbol->GetKind() == eSymbolKind::kSK_Namespace)
                m_treeCtrl->Expand(item);
            item = m_treeCtrl->GetNextChild(m_treeCtrl->GetRootItem(), cookie);
        }
    }  
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_EXPANDED, &LSPOutlineView::OnTreeItemExpanded, this);  
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_COLLAPSED, &LSPOutlineView::OnTreeItemCollapsed, this);   
}

void LSPOutlineView::ExpandTree()
{
    m_treeCtrl->Unbind(wxEVT_TREE_ITEM_EXPANDED, &LSPOutlineView::OnTreeItemExpanded, this); 
    m_treeCtrl->ExpandAll();    
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_EXPANDED, &LSPOutlineView::OnTreeItemExpanded, this);  
}

void LSPOutlineView::ClearView()
{
    m_currentSymbolsFileName.clear();
    m_terminalViewCtrl->DeleteAllItems();
    m_treeCtrl->DeleteAllItems();
    m_documentSymbols.clear();
    m_symbolsInformation.clear();
    m_treeState = TreeState();
}

void LSPOutlineView::ShowMessage(const wxString& message, bool showLoadingIndicator) {
    m_treeCtrl->Hide();
    m_terminalViewCtrl->Hide();
    m_msgText->SetLabel(message);
    m_msgIndicator->Show(showLoadingIndicator);
    if (showLoadingIndicator) 
        m_msgIndicator->Start();
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


void LSPOutlineView::InitExpandedState() {
    wxTreeItemIdValue cookie;
    wxTreeItemId root = m_treeCtrl->GetRootItem();
    if (m_treeState.manuallyCollapsed || (m_initialiseAllCollapsed && !m_treeState.manuallyExpanded)) {
        CollapseTree();
        return;
    }
    else if (m_treeState.manuallyExpanded) {
        ExpandTree();
        return;
    }
    
    std::function<void(const wxTreeItemId&, const DocumentSymbol*)> initStateRec = [&](const wxTreeItemId& item, const DocumentSymbol* parent) {
        auto* data = m_treeCtrl->GetItemData(item);
        auto* symbol = static_cast<DocumentSymbol*>(data->GetId().GetID());
        switch (symbol->GetKind()) {
            case eSymbolKind::kSK_Enum:
                if (m_initialiseEnumsCollapsed)
                    m_treeCtrl->Collapse(item);
                else
                    m_treeCtrl->Expand(item);
                break;
            case LSP::eSymbolKind::kSK_Class: wxFALLTHROUGH;
            case LSP::eSymbolKind::kSK_Struct:
                if (m_initialiseClassesCollapsed)
                    m_treeCtrl->Collapse(item);
                else if (m_initialiseSubclassesCollapsed && parent != nullptr) {
                    eSymbolKind parentKind = parent->GetKind();
                    if (parentKind == eSymbolKind::kSK_Class 
                        || parentKind == eSymbolKind::kSK_Struct) 
                    {                        
                        m_treeCtrl->Collapse(item);
                    }
                    else {
                        m_treeCtrl->Expand(item);
                    }
                }
                else
                    m_treeCtrl->Expand(item);
                break;
            case eSymbolKind::kSK_Method: wxFALLTHROUGH;
            case eSymbolKind::kSK_Constructor: wxFALLTHROUGH;
            case eSymbolKind::kSK_Function:
                if (m_initialiseFunctionsCollapsed)
                    m_treeCtrl->Collapse(item);
                else
                    m_treeCtrl->Expand(item);
                break;
            case eSymbolKind::kSK_Container:
                if (m_initialiseContainersCollapsed)
                    m_treeCtrl->Collapse(item);
                else
                    m_treeCtrl->Expand(item);
                break;
            case eSymbolKind::kSK_Variable: wxFALLTHROUGH;
            case eSymbolKind::kSK_Field: wxFALLTHROUGH;
            case eSymbolKind::kSK_String: 
                if (m_initialiseVariablesCollapsed)
                    m_treeCtrl->Collapse(item);
                else
                    m_treeCtrl->Expand(item);
                break;
            default: 
                m_treeCtrl->Expand(item);
        }  
        
        if (m_treeCtrl->GetChildrenCount(item) > 0) {    
            wxTreeItemIdValue _cookie;
            wxTreeItemId child = m_treeCtrl->GetFirstChild(item, _cookie);
            while (child.IsOk()) {
                initStateRec(child, symbol);
                child = m_treeCtrl->GetNextChild(item, _cookie);
            }
        }
    };
    wxTreeItemId item = m_treeCtrl->GetFirstChild(root, cookie);
    while (item.IsOk()) {        
        initStateRec(item, nullptr);
        item = m_treeCtrl->GetNextChild(root, cookie);
    }
}
    
void LSPOutlineView::OnMenu(wxCommandEvent& event)
{
    switch(event.GetId()) {
        case ID_MENU_INITIALISE_ALL_COLLAPSED: {
            m_initialiseAllCollapsed = event.IsChecked();
            if (m_initialiseAllCollapsed)
               CollapseTree();
            else   
                ExpandTree();
                
            clConfig::Get().Write(kConfigOutlineStartAllCollapsed, m_initialiseAllCollapsed);
            break;
        }
        case ID_MENU_INITIALISE_ENUMS_COLLAPSED: {
            m_initialiseEnumsCollapsed = event.IsChecked();
            InitExpandedState();                
            clConfig::Get().Write(kConfigOutlineStartEnumsCollapsed, m_initialiseEnumsCollapsed);
            break;
        }
        case ID_MENU_INITIALISE_CLASSES_COLLAPSED: {
            m_initialiseClassesCollapsed = event.IsChecked();
            InitExpandedState();                
            clConfig::Get().Write(kConfigOutlineStartClassesCollapsed, m_initialiseClassesCollapsed);
            break;
        }
        case ID_MENU_INITIALISE_SUBCLASSES_COLLAPSED: {
            m_initialiseSubclassesCollapsed = event.IsChecked();
            InitExpandedState();                
            clConfig::Get().Write(kConfigOutlineStartSubclassesCollapsed, m_initialiseSubclassesCollapsed);
            break;
        }
        case ID_MENU_INITIALISE_FUNCTIONS_COLLAPSED: {
            m_initialiseFunctionsCollapsed = event.IsChecked();
            InitExpandedState();                
            clConfig::Get().Write(kConfigOutlineStartFunctionsCollapsed, m_initialiseFunctionsCollapsed);
            break;
        }
        case ID_MENU_INITIALISE_CONTAINERS_COLLAPSED: {
            m_initialiseContainersCollapsed = event.IsChecked();
            InitExpandedState();                
            clConfig::Get().Write(kConfigOutlineStartContainersCollapsed, m_initialiseContainersCollapsed);
            break;
        }
        case ID_MENU_INITIALISE_VARIABLES_COLLAPSED: {
            m_initialiseVariablesCollapsed = event.IsChecked();
            InitExpandedState();                
            clConfig::Get().Write(kConfigOutlineStartVariablesCollapsed, m_initialiseVariablesCollapsed);
            break;
        }
        case ID_MENU_KEEP_NAMESPACE_EXPANDED: {
            m_keepNamespacesExpanded = event.IsChecked();
            clConfig::Get().Write(kConfigOutlineKeepNamespacesExpanded, m_keepNamespacesExpanded);
            break;
        }
        case ID_MENU_FORCE_TREE: {
            m_forceTree = event.IsChecked();
            clConfig::Get().Write(kConfigOutlineForceTree, m_forceTree);
            if (m_mode == Mode::SYMBOL_INFORMATION || m_mode == Mode::SYMBOL_INFORMATION_PARSED) {
                // re-parse our symbols with the changed tree setting
                SetSymbols(m_symbolsInformation, m_currentSymbolsFileName);
            }
            break;
        }
    }
}

void LSPOutlineView::OnToolBar(wxCommandEvent& event)
{
    switch (event.GetId()) {
        case ID_TOOL_DETAILS: {
            if (m_mode == Mode::SYMBOL_INFORMATION)
                return;
            m_showDetails = event.IsChecked();                
            clConfig::Get().Write(kConfigOutlineShowSymbolDetails, m_showDetails);
            DoInitialiseDocumentSymbol();
            break;
        }
        case ID_TOOL_KIND: {
            m_showSymbolKind = event.IsChecked();
            clConfig::Get().Write(kConfigOutlineShowSymbolKinds, m_showSymbolKind);
            if (m_mode == Mode::DOCUMENT_SYMBOL || m_mode == Mode::SYMBOL_INFORMATION_PARSED)
                DoInitialiseDocumentSymbol();
            else
                DoInitialiseSymbolInformation();
            break;
        }
        case ID_TOOL_COLLAPSE_ALL: {
            if (m_mode == Mode::SYMBOL_INFORMATION)
                return;
            CollapseTree();
            m_treeState.manuallyCollapsed = true;                
            m_treeState.manuallyExpanded = false;
            m_treeState.expandedNodes.clear();
            break;
        }
        case ID_TOOL_EXPAND_ALL: {
            if (m_mode == Mode::SYMBOL_INFORMATION)
                return;
            ExpandTree();                
            m_treeState.manuallyExpanded = true;
            m_treeState.manuallyCollapsed = false; 
            m_treeState.collapsedNodes.clear();
            break;
        }
        default: {}
    }
}
void LSPOutlineView::OnToolBarDropdown(wxAuiToolBarEvent& event)
{
    // show the settings menu
    if (event.GetToolId() == ID_TOOL_MENU) {
        auto images = clGetManager()->GetStdIcons();
        // submenu for initially expanded/collapsed items
        wxMenu* submenu = new wxMenu();        
        wxMenuItem* item = submenu->Append(ID_MENU_INITIALISE_ALL_COLLAPSED, _("Start with collapsed tree"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_initialiseAllCollapsed);        
        item = submenu->Append(ID_MENU_INITIALISE_ENUMS_COLLAPSED, _("Start with collapsed enums"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_initialiseEnumsCollapsed);        
        item = submenu->Append(ID_MENU_INITIALISE_SUBCLASSES_COLLAPSED, _("Start with collapsed subclasses"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_initialiseSubclassesCollapsed);        
        item = submenu->Append(ID_MENU_INITIALISE_FUNCTIONS_COLLAPSED, _("Start with collapsed functions"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_initialiseFunctionsCollapsed);        
        item = submenu->Append(ID_MENU_INITIALISE_CLASSES_COLLAPSED, _("Start with collapsed classes"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_initialiseClassesCollapsed);           
        item = submenu->Append(ID_MENU_INITIALISE_CONTAINERS_COLLAPSED, _("Start with collapsed containers"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_initialiseContainersCollapsed);          
        item = submenu->Append(ID_MENU_INITIALISE_VARIABLES_COLLAPSED, _("Start with collapsed variables"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_initialiseVariablesCollapsed);        
        
        // dropdown menu
        wxMenu menu;
        item = new wxMenuItem(&menu, wxID_ANY, _("Initial display"), _("Change the display state of the symbol hierarchy after loading"), wxITEM_NORMAL, submenu);
        item->SetBitmap(images->LoadBitmap("check-all"));
        menu.Append(item);
        
        item = menu.Append(ID_MENU_KEEP_NAMESPACE_EXPANDED, _("Keep namespaces expanded"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_keepNamespacesExpanded);
        
        item = menu.Append(ID_MENU_FORCE_TREE, _("Always show hierarchy (experimental)"), wxEmptyString, wxITEM_CHECK);
        item->Check(m_forceTree);
        
        PopupMenu(&menu, event.GetItemRect().GetBottomLeft());
    }
    else {
        event.Skip();
    }
}
void LSPOutlineView::OnSortChanged(wxCommandEvent& event)
{
    auto* data = static_cast<wxIntClientData*>(event.GetClientObject());
    CHECK_PTR_RET(data);
    LSPSymbolParser::SortType sort = (LSPSymbolParser::SortType)data->GetValue();        
    clConfig::Get().Write(kConfigOutlineSortType, (int)sort);
    SortSymbols(sort);
    if (m_mode == Mode::DOCUMENT_SYMBOL || m_mode == Mode::SYMBOL_INFORMATION_PARSED)
        DoInitialiseDocumentSymbol();
    else
        DoInitialiseSymbolInformation();
}