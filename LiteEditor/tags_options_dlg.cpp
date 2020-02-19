//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_dlg.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "AddOptionsDialog.h"
#include "ColoursAndFontsManager.h"
#include "clSingleChoiceDialog.h"
#include "cl_config.h"
#include "ctags_manager.h"
#include "globals.h"
#include "ieditor.h"
#include "includepathlocator.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "pluginmanager.h"
#include "pp_include.h"
#include "pptable.h"
#include "tags_options_dlg.h"
#include "windowattrmanager.h"
#include "wx/tokenzr.h"
#include <ICompilerLocator.h>
#include <build_settings_config.h>
#include <compiler.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
//---------------------------------------------------------

CodeCompletionSettingsDialog::CodeCompletionSettingsDialog(wxWindow* parent, const TagsOptionsData& data)
    : TagsOptionsBaseDlg(parent)
    , m_data(data)
{
    DoSetEditEventsHandler(this);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_textCtrlCtagsExcludePaths);
        lexer->Apply(m_textCtrlCtagsSearchPaths);
        lexer->Apply(m_textPrep);
        lexer->Apply(m_textTypes);
    }

    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrColouring->GetGrid());
    Centre();
    GetSizer()->Fit(this);
    SetName("CodeCompletionSettingsDialog");
    WindowAttrManager::Load(this);

    // Set default values
    bool genJsonFile = clConfig::Get().Read("GenerateCompileCommands", false);

    //------------------------------------------------------------------
    // Display and behavior
    //------------------------------------------------------------------
    m_checkDisplayFunctionTip->SetValue(m_data.GetFlags() & CC_DISP_FUNC_CALLTIP ? true : false);
    m_checkDisplayTypeInfo->SetValue(m_data.GetFlags() & CC_DISP_TYPE_INFO ? true : false);
    m_checkCppKeywordAssist->SetValue(m_data.GetFlags() & CC_CPP_KEYWORD_ASISST ? true : false);
    m_checkDisableParseOnSave->SetValue(m_data.GetFlags() & CC_DISABLE_AUTO_PARSING ? true : false);
    m_checkBoxretagWorkspaceOnStartup->SetValue(m_data.GetFlags() & CC_RETAG_WORKSPACE_ON_STARTUP ? true : false);
    m_checkBoxDeepUsingNamespaceResolving->SetValue(m_data.GetFlags() & CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING ? true
                                                                                                               : false);
    m_checkBoxEnableCaseSensitiveCompletion->SetValue(m_data.GetFlags() & CC_IS_CASE_SENSITIVE ? true : false);
    m_checkBoxKeepFunctionSignature->SetValue(m_data.GetFlags() & CC_KEEP_FUNCTION_SIGNATURE_UNFORMATTED);
    m_spinCtrlNumberOfCCItems->ChangeValue(::wxIntToString(m_data.GetCcNumberOfDisplayItems()));
    m_textCtrlFileSpec->ChangeValue(m_data.GetFileSpec());
    this->m_checkBoxGenCompileCommandsJSON->SetValue(genJsonFile);

    //------------------------------------------------------------------
    // Colouring
    //------------------------------------------------------------------
    m_pgPropColourLocalVariables->SetValue(m_data.GetFlags() & CC_COLOUR_VARS ? true : false);
    m_pgPropTrackPreProcessors->SetValue(m_data.GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS ? true : false);

    //----------------------------------------------------
    // Triggering
    //----------------------------------------------------
    m_checkWordAssist->SetValue(m_data.GetFlags() & CC_WORD_ASSIST ? true : false);
    m_checkAutoInsertSingleChoice->SetValue(m_data.GetFlags() & CC_AUTO_INSERT_SINGLE_CHOICE ? true : false);
    m_sliderMinWordLen->SetValue(m_data.GetMinWordLen());

    //----------------------------------------------------
    // CTags search paths
    //----------------------------------------------------
    m_textCtrlCtagsSearchPaths->SetValue(wxImplode(m_data.GetParserSearchPaths(), wxT("\n")));
    m_textCtrlCtagsExcludePaths->SetValue(wxImplode(m_data.GetParserExcludePaths(), wxT("\n")));

    //----------------------------------------------------
    // CTags advanced page
    //----------------------------------------------------
    m_textPrep->SetValue(m_data.GetTokens());
    m_textTypes->SetValue(m_data.GetTypes());
    m_textCtrlFilesList->SetValue(m_data.GetMacrosFiles());
    ::clSetDialogBestSizeAndPosition(this);
}

CodeCompletionSettingsDialog::~CodeCompletionSettingsDialog() {}

void CodeCompletionSettingsDialog::OnButtonOk(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CopyData();
    EndModal(wxID_OK);
}

void CodeCompletionSettingsDialog::CopyData()
{
    //----------------------------------------------------
    // Display and behavior
    //----------------------------------------------------
    SetFlag(CC_DISP_FUNC_CALLTIP, m_checkDisplayFunctionTip->IsChecked());
    SetFlag(CC_DISP_TYPE_INFO, m_checkDisplayTypeInfo->IsChecked());
    SetFlag(CC_CPP_KEYWORD_ASISST, m_checkCppKeywordAssist->IsChecked());
    SetFlag(CC_RETAG_WORKSPACE_ON_STARTUP, m_checkBoxretagWorkspaceOnStartup->IsChecked());
    SetFlag(CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING, m_checkBoxDeepUsingNamespaceResolving->IsChecked());
    SetFlag(CC_DISABLE_AUTO_PARSING, m_checkDisableParseOnSave->IsChecked());
    SetFlag(CC_IS_CASE_SENSITIVE, m_checkBoxEnableCaseSensitiveCompletion->IsChecked());
    SetFlag(CC_KEEP_FUNCTION_SIGNATURE_UNFORMATTED, m_checkBoxKeepFunctionSignature->IsChecked());
    m_data.SetCcNumberOfDisplayItems(::wxStringToInt(m_spinCtrlNumberOfCCItems->GetValue(), 100));
    m_data.SetFileSpec(m_textCtrlFileSpec->GetValue());
    clConfig::Get().Write("GenerateCompileCommands", this->m_checkBoxGenCompileCommandsJSON->IsChecked());

    //----------------------------------------------------
    // Colouring
    //----------------------------------------------------
    // colouring is updated on the fly. See OnColouringPropertyValueChanged() function below

    //----------------------------------------------------
    // Triggering
    //----------------------------------------------------
    SetFlag(CC_AUTO_INSERT_SINGLE_CHOICE, m_checkAutoInsertSingleChoice->IsChecked());
    SetFlag(CC_WORD_ASSIST, m_checkWordAssist->IsChecked());
    m_data.SetMinWordLen(m_sliderMinWordLen->GetValue());

    //----------------------------------------------------
    // CTags search paths
    //----------------------------------------------------
    m_data.SetParserSearchPaths(
        ::wxStringTokenize(m_textCtrlCtagsSearchPaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK));
    m_data.SetParserExcludePaths(
        ::wxStringTokenize(m_textCtrlCtagsExcludePaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK));

    //----------------------------------------------------
    // CTags advanced paths
    //----------------------------------------------------
    m_data.SetTokens(m_textPrep->GetValue());
    m_data.SetTypes(m_textTypes->GetValue());
    m_data.SetMacrosFiles(m_textCtrlFilesList->GetValue());
}

void CodeCompletionSettingsDialog::SetFlag(CodeCompletionOpts flag, bool set)
{
    if(set) {
        m_data.SetFlags(m_data.GetFlags() | flag);
    } else {
        m_data.SetFlags(m_data.GetFlags() & ~(flag));
    }
}

void CodeCompletionSettingsDialog::SetColouringFlag(CodeCompletionColourOpts flag, bool set)
{
    if(set) {
        m_data.SetCcColourFlags(m_data.GetCcColourFlags() | flag);
    } else {
        m_data.SetCcColourFlags(m_data.GetCcColourFlags() & ~(flag));
    }
}

void CodeCompletionSettingsDialog::Parse()
{
    // Prepate list of files to work on
    wxArrayString files = wxStringTokenize(m_textCtrlFilesList->GetValue(), wxT(" \t"), wxTOKEN_STRTOK);
    wxArrayString searchPaths = GetCTagsSearchPaths();
    wxArrayString fullpathsArr;

    for(size_t i = 0; i < files.size(); i++) {
        wxString file = files[i].Trim().Trim(false);
        if(file.IsEmpty())
            continue;

        for(size_t xx = 0; xx < searchPaths.size(); xx++) {
            wxString fullpath;
            fullpath << searchPaths.Item(xx) << wxFileName::GetPathSeparator() << file;
            wxFileName fn(fullpath);
            if(fn.FileExists()) {
                fullpathsArr.Add(fn.GetFullPath());
                break;
            }
        }
    }

    // Clear the PreProcessor table
    PPTable::Instance()->Clear();
    for(size_t i = 0; i < fullpathsArr.size(); i++)
        PPScan(fullpathsArr.Item(i), true);

    // Open an editor and print out the results
    IEditor* editor = PluginManager::Get()->NewEditor();
    if(editor) {
        editor->AppendText(PPTable::Instance()->Export());
        CopyData();
        EndModal(wxID_OK);
    }
}

void CodeCompletionSettingsDialog::OnAddExcludePath(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString new_path =
        wxDirSelector(_("Add ctags Parser Exclude Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        wxString currPathsStr = m_textCtrlCtagsExcludePaths->GetValue();
        wxArrayString currPaths = wxStringTokenize(currPathsStr, wxT("\n\r"), wxTOKEN_STRTOK);
        if(currPaths.Index(new_path) == wxNOT_FOUND) {
            currPathsStr.Trim().Trim(false);
            if(currPathsStr.IsEmpty() == false) {
                currPathsStr << wxT("\n");
            }
            currPathsStr << new_path;
            m_textCtrlCtagsExcludePaths->ChangeValue(currPathsStr);
        }
    }
}

void CodeCompletionSettingsDialog::OnAddSearchPath(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString new_path =
        wxDirSelector(_("Add ctags Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        wxString currPathsStr = m_textCtrlCtagsSearchPaths->GetValue();
        wxArrayString currPaths = wxStringTokenize(currPathsStr, wxT("\n\r"), wxTOKEN_STRTOK);
        if(currPaths.Index(new_path) == wxNOT_FOUND) {

            currPathsStr.Trim().Trim(false);
            if(currPathsStr.IsEmpty() == false) {
                currPathsStr << wxT("\n");
            }
            currPathsStr << new_path;

            m_textCtrlCtagsSearchPaths->ChangeValue(currPathsStr);
        }
    }
}

void CodeCompletionSettingsDialog::OnAutoShowWordAssitUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkWordAssist->IsChecked());
}

void CodeCompletionSettingsDialog::OnFileSelectedUI(wxUpdateUIEvent& event)
{
    event.Enable(m_textCtrlFilesList->GetValue().IsEmpty() == false);
}

void CodeCompletionSettingsDialog::OnParse(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Parse();
}

wxArrayString CodeCompletionSettingsDialog::GetCTagsSearchPaths() const
{
    return wxStringTokenize(m_textCtrlCtagsSearchPaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK);
}

void CodeCompletionSettingsDialog::OnSuggestCtags(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSuggest(m_textCtrlCtagsSearchPaths);
}

void CodeCompletionSettingsDialog::DoSuggest(wxStyledTextCtrl* textCtrl)
{
    CompilerPtrVec_t allCompilers = BuildSettingsConfigST::Get()->GetAllCompilers();

    // We only support auto retrieval of compilers from the GCC family
    wxArrayString compilerNames;
    std::for_each(allCompilers.begin(), allCompilers.end(), [&](CompilerPtr c) {
        if(c->GetCompilerFamily() == COMPILER_FAMILY_CLANG || c->GetCompilerFamily() == COMPILER_FAMILY_MINGW ||
           c->GetCompilerFamily() == COMPILER_FAMILY_GCC) {
            compilerNames.Add(c->GetName());
        } else if(::clIsCygwinEnvironment() && c->GetCompilerFamily() == COMPILER_FAMILY_CYGWIN) {
            compilerNames.Add(c->GetName());
        }
    });

    wxString selection;
    if(compilerNames.size() > 1) {
        // we have more than one compiler defined, ask the user which one to use
        clSingleChoiceDialog dlg(this, compilerNames, 0);
        dlg.SetTitle(_("Select the compiler to use:"));

        if(dlg.ShowModal() != wxID_OK)
            return;
        selection = dlg.GetSelection();
    } else if(compilerNames.size() == 1) {
        selection = compilerNames.Item(0);
    }

    if(selection.IsEmpty())
        return;

    CompilerPtr comp = BuildSettingsConfigST::Get()->GetCompiler(selection);
    CHECK_PTR_RET(comp);

    wxArrayString paths;
    paths = comp->GetDefaultIncludePaths();

    wxString suggestedPaths;
    for(size_t i = 0; i < paths.GetCount(); i++) {
        suggestedPaths << paths.Item(i) << wxT("\n");
    }

    suggestedPaths.Trim().Trim(false);
    if(!suggestedPaths.IsEmpty()) {
        if(::wxMessageBox(_("Accepting this suggestion will replace your old search paths with these paths\nContinue?"),
                          "CodeLite", wxYES_NO | wxYES_DEFAULT | wxCANCEL | wxICON_QUESTION) != wxYES) {
            return;
        }
        textCtrl->Clear();
        textCtrl->ChangeValue(suggestedPaths);
    }
}
void CodeCompletionSettingsDialog::OnColouringPropertyValueChanged(wxPropertyGridEvent& event)
{
    // Enable the local variables colouring feature
    SetFlag(CC_COLOUR_VARS, m_pgPropColourLocalVariables->GetValue().GetBool());

    // Enable pre processor tracking (must come after we set the flags above)
    SetColouringFlag(CC_COLOUR_MACRO_BLOCKS, m_pgPropTrackPreProcessors->GetValue().GetBool());
}

void CodeCompletionSettingsDialog::OnButtonCancel(wxCommandEvent& event)
{
    event.Skip();
    EndModal(wxID_CANCEL);
}

void CodeCompletionSettingsDialog::DoSetEditEventsHandler(wxWindow* win)
{
    // wxTextCtrl needs some extra special handling
    if(dynamic_cast<wxStyledTextCtrl*>(win)) {
        clEditEventsHandler::Ptr_t handler(new clEditEventsHandler(dynamic_cast<wxStyledTextCtrl*>(win)));
        m_handlers.push_back(handler);
    }

    // Check the children
    wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    while(pclNode) {
        wxWindow* pclChild = pclNode->GetData();
        this->DoSetEditEventsHandler(pclChild);
        pclNode = pclNode->GetNext();
    }
}
