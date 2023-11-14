#include "outline_tab.h"

#include "ColoursAndFontsManager.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"

#include <unordered_set>
#include <wx/colour.h>
#include <wx/stc/stc.h>

namespace
{
const wxString FUNCTION_SYMBOL = wxT("\u2A10");
const wxString CLASS_SYMBOL = wxT("\u2394");
const wxString VARIABLE_SYMBOL = wxT("\u2027");
const wxString MODULE_SYMBOL = wxT("{}");
const wxString ENUMERATOR_SYMBOL = wxT("#");
} // namespace

using namespace LSP;

OutlineTab::OutlineTab(wxWindow* parent)
    : OutlineTabBaseClass(parent)
{
    EventNotifier::Get()->Bind(wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE, &OutlineTab::OnOutlineSymbols, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &OutlineTab::OnAllEditorsClosed, this);
}

OutlineTab::~OutlineTab()
{
    EventNotifier::Get()->Unbind(wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE, &OutlineTab::OnOutlineSymbols, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &OutlineTab::OnAllEditorsClosed, this);
}

void OutlineTab::OnOutlineSymbols(LSPEvent& event)
{
    event.Skip();
    if(!IsShown()) {
        return;
    }
    RenderSymbols(event.GetSymbolsInformation(), event.GetFileName());
}

void OutlineTab::RenderSymbols(const std::vector<LSP::SymbolInformation>& symbols, const wxString& filename)
{
    ClearView();

    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    wxString remote_path;
    if(editor->IsRemoteFile()) {
        remote_path = editor->GetRemotePath();
    }

    wxString local_path = editor->GetFileName().GetFullPath();
    if(local_path != filename && remote_path != filename) {
        // the symbols do not match the ative editor
        return;
    }

    m_currentSymbolsFileName = filename;
    m_symbols = symbols;

    auto lexer = ColoursAndFontsManager::Get().GetLexer("python");
    if(symbols.empty()) {
        clAnsiEscapeCodeColourBuilder builder;
        builder.SetTheme(lexer->IsDark() ? eColourTheme::DARK : eColourTheme::LIGHT);
        builder.Add(_("Language Server is still not ready... "), AnsiColours::NormalText(), false);
        builder.Add(_("(hit ESCAPE key to dismiss)"), AnsiColours::Gray(), false);
        m_dvListCtrl->AddLine(builder.GetString(), false, (wxUIntPtr)0);
        return;
    }

    m_dvListCtrl->Begin();
    m_dvListCtrl->SetScrollToBottom(false);

    // reduce the outline font size
    wxFont font = lexer->GetFontForStyle(0, m_dvListCtrl);
    font.SetFractionalPointSize(static_cast<double>(font.GetPointSize()) * 0.8);
    m_dvListCtrl->SetDefaultFont(font);

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
    for(const SymbolInformation& si : m_symbols) {
        builder.Clear();

        if(!si.GetContainerName().empty() && containers.count(si.GetContainerName()) == 0) {
            // probably a fake container
            containers.insert(si.GetContainerName());
            builder.Add(CLASS_SYMBOL + " ", AnsiColours::NormalText());
            builder.Add(si.GetContainerName(), class_colour, true);
            m_dvListCtrl->AddLine(builder.GetString(), false, (wxUIntPtr)&si);
            builder.Clear();
        }

        size_t indent_level = si.GetContainerName().empty() ? INITIAL_DEPTH : DEPTH_WIDTH;
        builder.Add(wxString(' ', indent_level), AnsiColours::NormalText());

        // determine the symbol
        switch(si.GetKind()) {
        case kSK_File:
        case kSK_Module:
        case kSK_Package:
            builder.Add(MODULE_SYMBOL + " ", AnsiColours::NormalText());
            builder.Add(si.GetName(), module_colour);
            break;
        case kSK_Class:
        case kSK_Struct:
        case kSK_Interface:
        case kSK_Object:
        case kSK_Enum:
            builder.Add(CLASS_SYMBOL + " ", AnsiColours::NormalText());
            builder.Add(si.GetName(), class_colour, true);
            break;
        case kSK_Method:
        case kSK_Function:
        case kSK_Constructor:
            builder.Add(FUNCTION_SYMBOL + " ", AnsiColours::NormalText());
            if(si.GetName().Contains("(") && si.GetName().Contains(")")) {
                // the name also has the signature
                wxString signature = si.GetName().AfterFirst('(');
                signature = signature.BeforeLast(')');
                wxString name_only = si.GetName().BeforeFirst('(');
                builder.Add(name_only, function_colour);
                builder.Add("(", operator_colour);
                builder.Add(signature, AnsiColours::NormalText());
                builder.Add(")", operator_colour);
            } else {
                builder.Add(si.GetName(), function_colour);
                builder.Add("()", operator_colour);
            }
            break;
        case kSK_TypeParameter: // define
        case kSK_EnumMember:
            builder.Add(ENUMERATOR_SYMBOL + " ", AnsiColours::NormalText());
            builder.Add(si.GetName(), AnsiColours::NormalText());
            break;
        default:
            builder.Add(VARIABLE_SYMBOL + " ", AnsiColours::NormalText());
            builder.Add(si.GetName(), variable_colour);
            break;
        }
        m_dvListCtrl->AddLine(builder.GetString(), false, (wxUIntPtr)&si);
    }
    if(!m_dvListCtrl->IsEmpty()) {
        m_dvListCtrl->SelectRow(0);
    }
    m_dvListCtrl->Commit();
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
    m_dvListCtrl->DeleteAllItems();
    m_symbols.clear();
}

void OutlineTab::OnItemSelected(wxDataViewEvent& event)
{
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);

    // select the line
    SymbolInformation* psi = reinterpret_cast<SymbolInformation*>(m_dvListCtrl->GetItemData(item));
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
