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

#include <wx/tokenzr.h>
#include "pluginmanager.h"
#include "pp_include.h"
#include "pptable.h"
#include "ieditor.h"
#include "tags_options_dlg.h"
#include "ctags_manager.h"
#include "windowattrmanager.h"
#include "macros.h"
#include "wx/tokenzr.h"
#include "add_option_dialog.h"
#include "globals.h"
#include "includepathlocator.h"
#include "clang_code_completion.h"
#include <build_settings_config.h>
#include <compiler.h>
#include <ICompilerLocator.h>
#include <wx/msgdlg.h>
#include "clSingleChoiceDialog.h"

//---------------------------------------------------------

TagsOptionsDlg::TagsOptionsDlg(wxWindow* parent, const TagsOptionsData& data)
    : TagsOptionsBaseDlg(parent)
    , m_data(data)
{
    MSWSetNativeTheme(m_treebook2->GetTreeCtrl());
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgrColouring->GetGrid());
    Centre();
    GetSizer()->Fit(this);
    SetName("TagsOptionsDlg");
    WindowAttrManager::Load(this);

    // Set default values

    //------------------------------------------------------------------
    // Display and behavior
    //------------------------------------------------------------------
    m_checkDisplayFunctionTip->SetValue(m_data.GetFlags() & CC_DISP_FUNC_CALLTIP ? true : false);
    m_checkDisplayTypeInfo->SetValue(m_data.GetFlags() & CC_DISP_TYPE_INFO ? true : false);
    m_checkCppKeywordAssist->SetValue(m_data.GetFlags() & CC_CPP_KEYWORD_ASISST ? true : false);
    m_checkDisableParseOnSave->SetValue(m_data.GetFlags() & CC_DISABLE_AUTO_PARSING ? true : false);
    m_checkBoxretagWorkspaceOnStartup->SetValue(m_data.GetFlags() & CC_RETAG_WORKSPACE_ON_STARTUP ? true : false);
    m_checkBoxDeepUsingNamespaceResolving->SetValue(m_data.GetFlags() & CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING ? true :
                                                                                                                 false);
    m_checkBoxEnableCaseSensitiveCompletion->SetValue(m_data.GetFlags() & CC_IS_CASE_SENSITIVE ? true : false);
    m_checkBoxKeepFunctionSignature->SetValue(m_data.GetFlags() & CC_KEEP_FUNCTION_SIGNATURE_UNFORMATTED);
    m_spinCtrlNumberOfCCItems->ChangeValue(::wxIntToString(m_data.GetCcNumberOfDisplayItems()));

    //------------------------------------------------------------------
    // Colouring
    //------------------------------------------------------------------
    m_pgPropColourWorkspaceSymbols->SetValueFromInt(m_data.GetCcColourFlags());
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
    m_textFileSpec->SetValue(m_data.GetFileSpec());

//----------------------------------------------------
// Clang page
//----------------------------------------------------
#if HAS_LIBCLANG
    m_checkBoxEnableClangCC->SetValue(m_data.GetClangOptions() & CC_CLANG_ENABLED);
#else
    m_checkBoxEnableClangCC->SetValue(false);
    m_checkBoxEnableClangCC->Enable(false);
#endif

    m_checkBoxClangFirst->SetValue(m_data.GetClangOptions() & CC_CLANG_FIRST);
    m_textCtrlClangSearchPaths->SetValue(m_data.GetClangSearchPaths());
    m_choiceCachePolicy->Clear();

    // defaults
    m_choiceCachePolicy->Append(TagsOptionsData::CLANG_CACHE_LAZY);
    m_choiceCachePolicy->Append(TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD);
    m_choiceCachePolicy->Select(1);

    int where = m_choiceCachePolicy->FindString(m_data.GetClangCachePolicy());
    if(where != wxNOT_FOUND) {
        m_choiceCachePolicy->Select(where);
    }
}

TagsOptionsDlg::~TagsOptionsDlg() {}

void TagsOptionsDlg::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CopyData();
    EndModal(wxID_OK);
}

void TagsOptionsDlg::CopyData()
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
    m_data.SetFileSpec(m_textFileSpec->GetValue());
    m_data.SetTokens(m_textPrep->GetValue());
    m_data.SetTypes(m_textTypes->GetValue());
    m_data.SetMacrosFiles(m_textCtrlFilesList->GetValue());

    //----------------------------------------------------
    // Clang
    //----------------------------------------------------
    size_t options(0);
    if(m_checkBoxEnableClangCC->IsChecked()) options |= CC_CLANG_ENABLED;

    if(m_checkBoxClangFirst->IsChecked()) options |= CC_CLANG_FIRST;

    m_data.SetClangOptions(options);
    m_data.SetClangSearchPaths(m_textCtrlClangSearchPaths->GetValue());
    m_data.SetClangCachePolicy(m_choiceCachePolicy->GetStringSelection());
}

void TagsOptionsDlg::SetFlag(CodeCompletionOpts flag, bool set)
{
    if(set) {
        m_data.SetFlags(m_data.GetFlags() | flag);
    } else {
        m_data.SetFlags(m_data.GetFlags() & ~(flag));
    }
}

void TagsOptionsDlg::SetColouringFlag(CodeCompletionColourOpts flag, bool set)
{
    if(set) {
        m_data.SetCcColourFlags(m_data.GetCcColourFlags() | flag);
    } else {
        m_data.SetCcColourFlags(m_data.GetCcColourFlags() & ~(flag));
    }
}

void TagsOptionsDlg::Parse()
{
    // Prepate list of files to work on
    wxArrayString files = wxStringTokenize(m_textCtrlFilesList->GetValue(), wxT(" \t"), wxTOKEN_STRTOK);
    wxArrayString searchPaths = GetCTagsSearchPaths();
    wxArrayString fullpathsArr;

    for(size_t i = 0; i < files.size(); i++) {
        wxString file = files[i].Trim().Trim(false);
        if(file.IsEmpty()) continue;

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
    for(size_t i = 0; i < fullpathsArr.size(); i++) PPScan(fullpathsArr.Item(i), true);

    // Open an editor and print out the results
    IEditor* editor = PluginManager::Get()->NewEditor();
    if(editor) {
        editor->AppendText(PPTable::Instance()->Export());
        CopyData();
        EndModal(wxID_OK);
    }
}

void TagsOptionsDlg::OnAddExcludePath(wxCommandEvent& event)
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

void TagsOptionsDlg::OnAddSearchPath(wxCommandEvent& event)
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

void TagsOptionsDlg::OnAutoShowWordAssitUI(wxUpdateUIEvent& event) { event.Enable(m_checkWordAssist->IsChecked()); }

void TagsOptionsDlg::OnClangCCEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxEnableClangCC->IsChecked()); }

void TagsOptionsDlg::OnClearClangCache(wxCommandEvent& event)
{
    wxBusyCursor cursor;
#if HAS_LIBCLANG
    ClangCodeCompletion::Instance()->ClearCache();
#endif
}

void TagsOptionsDlg::OnClearClangCacheUI(wxUpdateUIEvent& event)
{
#if HAS_LIBCLANG
    event.Enable(m_checkBoxEnableClangCC->IsChecked() && !ClangCodeCompletion::Instance()->IsCacheEmpty());
#else
    event.Enable(false);
#endif
}

void TagsOptionsDlg::OnFileSelectedUI(wxUpdateUIEvent& event)
{
    event.Enable(m_textCtrlFilesList->GetValue().IsEmpty() == false);
}

void TagsOptionsDlg::OnParse(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Parse();
}

void TagsOptionsDlg::OnSuggestSearchPaths(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSuggest(m_textCtrlClangSearchPaths);
}

wxArrayString TagsOptionsDlg::GetCTagsSearchPaths() const
{
    return wxStringTokenize(m_textCtrlCtagsSearchPaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK);
}

void TagsOptionsDlg::OnSuggestCtags(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSuggest(m_textCtrlCtagsSearchPaths);
}

void TagsOptionsDlg::DoSuggest(wxTextCtrl* textCtrl)
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

        if(dlg.ShowModal() != wxID_OK) return;
        selection = dlg.GetSelection();
    } else if(compilerNames.size() == 1) {
        selection = compilerNames.Item(0);
    }

    if(selection.IsEmpty()) return;

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
                          "CodeLite",
                          wxYES_NO | wxYES_DEFAULT | wxCANCEL | wxICON_QUESTION) != wxYES) {
            return;
        }
        textCtrl->Clear();
        textCtrl->ChangeValue(suggestedPaths);
    }
}
void TagsOptionsDlg::OnColouringPropertyValueChanged(wxPropertyGridEvent& event)
{
    // Enable the local variables colouring feature
    SetFlag(CC_COLOUR_VARS, m_pgPropColourLocalVariables->GetValue().GetBool());

    // if any of the types is selected, enable this feature
    size_t ccColourFlags(0);
    ccColourFlags = m_pgPropColourWorkspaceSymbols->GetValue().GetInteger();
    m_data.SetCcColourFlags(ccColourFlags);
    SetFlag(CC_COLOUR_WORKSPACE_TAGS, (ccColourFlags > 0));

    // Enable pre processor tracking (must come after we set the flags above)
    SetColouringFlag(CC_COLOUR_MACRO_BLOCKS, m_pgPropTrackPreProcessors->GetValue().GetBool());
}
