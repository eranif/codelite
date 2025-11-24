#include "outline_tab.h"

#include "ColoursAndFontsManager.h"
#include "LSP/LSPManager.hpp"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clSideBarCtrl.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"

#include <unordered_set>
#include <wx/colour.h>
#include <wx/stc/stc.h>

using namespace LSP;

OutlineTab::OutlineTab(wxWindow* parent)
    : OutlineTabBaseClass(parent)
{
    EventNotifier::Get()->Bind(wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW, &OutlineTab::OnOutlineSymbols, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &OutlineTab::OnAllEditorsClosed, this);
    EventNotifier::Get()->Bind(wxEVT_SIDEBAR_SELECTION_CHANGED, &OutlineTab::OnSideBarPageChanged, this);
    
    // tree node double clicked
    m_treeCtrl->Bind(wxEVT_TREE_ITEM_ACTIVATED, &OutlineTab::OnTreeItemSelected, this);
    // context menu items selected
    Bind(wxEVT_COMMAND_MENU_SELECTED, &OutlineTab::OnMenu, this);
    
    // hide everything until we receive data
    m_treeCtrl->Hide();
    m_terminalViewCtrl->Hide();
    m_messagePanel->Hide();
}

OutlineTab::~OutlineTab()
{
    EventNotifier::Get()->Unbind(wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW, &OutlineTab::OnOutlineSymbols, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &OutlineTab::OnAllEditorsClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_SIDEBAR_SELECTION_CHANGED, &OutlineTab::OnSideBarPageChanged, this);
}

void OutlineTab::OnOutlineSymbols(LSPEvent& event)
{
    event.Skip();
    if (!IsShown()) {
        return;
    }
    // depending on what data we get, show a different view
    if (!event.GetDocumentSymbols().empty()) {        
        RenderSymbols(event.GetDocumentSymbols(), event.GetFileName());
    }
    else if (!event.GetSymbolsInformation().empty()) {
        RenderSymbols(event.GetSymbolsInformation(), event.GetFileName());        
    }
    else {
        ShowMessage("Waiting for language server...", true);
    }
}

bool OutlineTab::CheckAndRequest(const wxString& filename) {
    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET_FALSE(editor);

    wxString remote_path;
    if (editor->IsRemoteFile()) {
        remote_path = editor->GetRemotePath();
    }
    wxString local_path = editor->GetFileName().GetFullPath();
    
    if (local_path != filename && remote_path != filename) {
        // the symbols do not match the active editor
        // Request symbols for the current editor.
        
        LSP_DEBUG() << "OutlineTab::CheckAndRequest: Requesting symbols" << endl;
        LSP::Manager::GetInstance().RequestSymbolsForEditor(editor, nullptr);
        return false;
    }
    
    return true;
}

void OutlineTab::RenderSymbols(const std::vector<LSP::DocumentSymbol>& symbols, const wxString& filename) 
{        
    m_mode = Mode::DOCUMENT_SYMBOL;
    HideMessage();
    m_terminalViewCtrl->Hide();
    m_treeCtrl->Show();
    m_headerPanel->Show();
        
    CHECK_COND_RET(CheckAndRequest(filename));
    
    // Clear the tree. Not using ClearView() because we don't want to delete our symbols
    // in case a re-render
    m_treeCtrl->DeleteAllItems();
    
    m_currentSymbolsFileName = filename;
    m_documentSymbols = symbols;
        
    if (symbols.empty()) {
        ShowMessage(_("No Symbols received from language server."), false);
        return;
    }

    auto lexer = ColoursAndFontsManager::Get().GetLexer("python");

    // reduce the outline font size
    wxFont font = lexer->GetFontForStyle(0, m_treeCtrl);
    font.SetFractionalPointSize(static_cast<double>(font.GetPointSize()) * 0.8);
    m_treeCtrl->SetDefaultFont(font);
    
    // build the tree
    m_treeCtrl->Begin();
    
    clColours colours;
    colours.FromLexer(lexer);
    m_treeCtrl->SetColours(colours);
    m_treeCtrl->AddTreeStyle(wxTR_HIDE_ROOT);
        
    // invisible root node
    wxTreeItemId node = m_treeCtrl->AddRoot("Root");
    // level-one nodes
    for (const DocumentSymbol& ds : m_documentSymbols) {
        AddDocumentSymbolRec(node, ds, lexer.get());
    }
    
    m_treeCtrl->Commit();
    m_treeCtrl->ExpandAll();    
    Layout();
}

void OutlineTab::AddDocumentSymbolRec(wxTreeItemId parent, const LSP::DocumentSymbol& symbol, LexerConf* lexer) {
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

void OutlineTab::RenderSymbols(const std::vector<LSP::SymbolInformation>& symbols, const wxString& filename)
{
    m_mode = Mode::SYMBOL_INFORMATION;
    HideMessage();
    m_treeCtrl->Hide();
    m_terminalViewCtrl->Show();
    m_headerPanel->Hide();
    
    ClearView();

    CHECK_COND_RET(CheckAndRequest(filename));

    m_currentSymbolsFileName = filename;
    m_symbolsInformation = symbols;

    auto lexer = ColoursAndFontsManager::Get().GetLexer("python");
    if (symbols.empty()) {
        ShowMessage(_("Language Server is still not ready... "), true);
        return;
    }

    m_terminalViewCtrl->Begin();
    m_terminalViewCtrl->SetScrollToBottom(false);

    // reduce the outline font size
    wxFont font = lexer->GetFontForStyle(0, m_terminalViewCtrl);
    font.SetFractionalPointSize(static_cast<double>(font.GetPointSize()) * 0.8);
    m_terminalViewCtrl->SetDefaultFont(font);

    // build the tree
    wxColour class_colour = lexer->GetProperty(wxSTC_P_WORD2).GetFgColour();
    wxColour variable_colour = lexer->GetProperty(wxSTC_P_IDENTIFIER).GetFgColour();
    wxColour module_colour = lexer->GetProperty(wxSTC_P_STRING).GetFgColour();
    wxColour function_colour = lexer->GetProperty(wxSTC_P_DEFNAME).GetFgColour();
    wxColour operator_colour = lexer->GetProperty(wxSTC_P_OPERATOR).GetFgColour();

    constexpr int INITIAL_DEPTH = 0;
    constexpr int DEPTH_WIDTH = 2;

    std::unordered_set<wxString> containers;
    clAnsiEscapeCodeColourBuilder builder;
    for (const SymbolInformation& si : m_symbolsInformation) {
        builder.Clear();
        LSPSymbolStyle style(si.GetKind(), lexer.get());
        
        if (!si.GetContainerName().empty() && containers.count(si.GetContainerName()) == 0) {
            // probably a fake container
            containers.insert(si.GetContainerName());
            builder.Add(style.icon + " ", AnsiColours::NormalText());
            builder.Add(si.GetContainerName(), class_colour, true);
            m_terminalViewCtrl->AddLine(builder.GetString(), false, (wxUIntPtr)&si);
            builder.Clear();
        }

        size_t indent_level = si.GetContainerName().empty() ? INITIAL_DEPTH : DEPTH_WIDTH;
        builder.Add(wxString(' ', indent_level), AnsiColours::NormalText());

        // determine the symbol
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
            builder.Add(si.GetName(), variable_colour);
            break;
        }
        m_terminalViewCtrl->AddLine(builder.GetString(), false, (wxUIntPtr)&si);
    }
    if (!m_terminalViewCtrl->IsEmpty()) {
        m_terminalViewCtrl->SelectRow(0);
    }
    m_terminalViewCtrl->Commit();
    Layout();
}

void OutlineTab::ShowMessage(const wxString& message, bool showLoadingIndicator) {
    m_treeCtrl->Hide();
    m_terminalViewCtrl->Hide();
    m_staticTextMessage->SetLabel(message);
    m_activityCtrl->Show(showLoadingIndicator);
    if(showLoadingIndicator) m_activityCtrl->Start();
    m_messagePanel->Show();
}

void OutlineTab::HideMessage() {
    m_activityCtrl->Stop();
    m_activityCtrl->Hide();
    m_messagePanel->Hide();
}

void OutlineTab::OnAllEditorsClosed(wxCommandEvent& event)
{
    event.Skip();
    ClearView();
}

void OutlineTab::OnActiveEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    ClearView();
}

void OutlineTab::ClearView()
{
    // clear the view
    m_currentSymbolsFileName.clear();
    m_terminalViewCtrl->DeleteAllItems();
    m_treeCtrl->DeleteAllItems();
    m_documentSymbols.clear();
    m_symbolsInformation.clear();
}

void OutlineTab::OnItemSelected(wxDataViewEvent& event)
{
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);

    // select the line
    SymbolInformation* psi = reinterpret_cast<SymbolInformation*>(m_terminalViewCtrl->GetItemData(item));
    CHECK_PTR_RET(psi);

    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    int line_number = psi->GetLocation().GetRange().GetStart().GetLine();
    CHECK_EXPECTED_RETURN(line_number >= 0, true);

    // make sure this line is not folded
    editor->GetCtrl()->EnsureVisible(line_number);
    // center the editor around it
    editor->CenterLine(line_number);

    // set the focus to the editor
    editor->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void OutlineTab::OnTreeItemSelected(wxTreeEvent& event)
{
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);

    wxTreeItemData* data = m_treeCtrl->GetItemData(item);
    CHECK_PTR_RET(data);        

    LSP::DocumentSymbol* symbol = reinterpret_cast<LSP::DocumentSymbol*>(data->GetId().m_pItem);
    if (!symbol || symbol->GetKind() == LSP::eSymbolKind::kSK_Container) 
        return;
    
    // open the editor and go to
    IEditor* active_editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(active_editor);

    int sci_line = symbol->GetSelectionRange().GetStart().GetLine();
    int position = active_editor->PosFromLine(sci_line);  // start of line
    position += symbol->GetSelectionRange().GetStart().GetCharacter(); // add the column
    active_editor->SetCaretAt(position);
    active_editor->CenterLine(sci_line);
    
    // set the focus to the editor
    active_editor->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void OutlineTab::OnSideBarPageChanged(clCommandEvent& event)
{
    LSP_DEBUG() << "OutlineTab::OnSideBarPageChanged: Requesting Symbols" << endl;
    event.Skip();
    // Clear and request symbols for the current editor.
    ClearView();
    LSP::Manager::GetInstance().RequestSymbolsForEditor(clGetManager()->GetActiveEditor(), nullptr);
}

void OutlineTab::OnOptionsButton(wxCommandEvent& event)
{
    wxMenu menu;
    menu.Append(ID_SHOW_DETAILS, m_showDetails ? _("Hide details") : _("Show details"));
    menu.Append(ID_SHOW_KIND, m_showSymbolKind ? _("Hide symbol kind") : _("Show symbol kind"));
    PopupMenu(&menu);
}

void OutlineTab::OnSortButton(wxCommandEvent& event)
{
    wxMenu menu;
    menu.Append(ID_SORT_ALPHABETICALLY, _("Sort Alphabetically"));
    menu.Append(ID_SORT_LINE, _("Sort by line number"));
    PopupMenu(&menu);
}

void OutlineTab::OnMenu(wxCommandEvent& event) {
    switch (event.GetId()) {
        case ID_SHOW_DETAILS: {
            if (m_mode == Mode::SYMBOL_INFORMATION)
                return;
            m_showDetails = !m_showDetails;
            RenderSymbols(m_documentSymbols, m_currentSymbolsFileName);
            break;
        }
        case ID_SHOW_KIND: {
            if (m_mode == Mode::SYMBOL_INFORMATION)
                return;
            m_showSymbolKind = !m_showSymbolKind;
            RenderSymbols(m_documentSymbols, m_currentSymbolsFileName);
            break;
        }
        case ID_SORT_ALPHABETICALLY: {
            if (m_mode == Mode::SYMBOL_INFORMATION)
                return;
            break;
        }
        case ID_SORT_LINE: {
            if (m_mode == Mode::SYMBOL_INFORMATION)
                return;
            break;
        }
    }
}
