#include "compilerlinkeroptionspage.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "globals.h"

CompilerLinkerOptionsPage::CompilerLinkerOptionsPage( wxWindow* parent, const wxString &cmpname )
    : CompilerLinkerOptionsBase( parent )
    , m_cmpname(cmpname)
    , m_selectedLnkOption(wxNOT_FOUND)
{
    m_listLinkerOptions->InsertColumn(0, _("Switch"));
    m_listLinkerOptions->InsertColumn(1, _("Help"));

    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
    const Compiler::CmpCmdLineOptions& lnkOptions = cmp->GetLinkerOptions();
    Compiler::CmpCmdLineOptions::const_iterator itLnkOption = lnkOptions.begin();
    for ( ; itLnkOption != lnkOptions.end(); ++itLnkOption) {
        const Compiler::CmpCmdLineOption& lnkOption = itLnkOption->second;
        long idx = m_listLinkerOptions->InsertItem(m_listLinkerOptions->GetItemCount(), lnkOption.name);
        m_listLinkerOptions->SetItem(idx, 1, lnkOption.help);
    }
    m_listLinkerOptions->SetColumnWidth(0, 100);
    m_listLinkerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void CompilerLinkerOptionsPage::Save(CompilerPtr cmp)
{
    Compiler::CmpCmdLineOptions lnkOptions;
    for (int idx = 0; idx < m_listLinkerOptions->GetItemCount(); ++idx) {
        Compiler::CmpCmdLineOption lnkOption;
        lnkOption.name = m_listLinkerOptions->GetItemText(idx);
        lnkOption.help = GetColumnText(m_listLinkerOptions, idx, 1);

        lnkOptions[lnkOption.name] = lnkOption;
    }
    cmp->SetLinkerOptions(lnkOptions);
}

void CompilerLinkerOptionsPage::OnLinkerOptionActivated( wxListEvent& event )
{
    if (m_selectedLnkOption == wxNOT_FOUND) {
        return;
    }

    wxString name = m_listLinkerOptions->GetItemText(m_selectedLnkOption);
    wxString help = GetColumnText(m_listLinkerOptions, m_selectedLnkOption, 1);
    CompilerLinkerOptionDialog dlg(this, name, help);
    if (dlg.ShowModal() == wxID_OK) {
        SetColumnText(m_listLinkerOptions, m_selectedLnkOption, 0, dlg.GetName());
        SetColumnText(m_listLinkerOptions, m_selectedLnkOption, 1, dlg.GetHelp());
        m_listLinkerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
    }
}

void CompilerLinkerOptionsPage::OnNewLinkerOption( wxCommandEvent& event )
{
    CompilerLinkerOptionDialog dlg(this, wxEmptyString, wxEmptyString);
    if (dlg.ShowModal() == wxID_OK) {
        long idx = m_listLinkerOptions->InsertItem(m_listLinkerOptions->GetItemCount(), dlg.GetName());
        m_listLinkerOptions->SetItem(idx, 1, dlg.GetHelp());
        m_listLinkerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
    }
}

void CompilerLinkerOptionsPage::OnDeleteLinkerOption( wxCommandEvent& event )
{
    if (m_selectedLnkOption != wxNOT_FOUND) {
        if (wxMessageBox(_("Are you sure you want to delete this linker option?"), _("CodeLite"), wxYES_NO|wxCANCEL) == wxYES) {
            m_listLinkerOptions->DeleteItem(m_selectedLnkOption);
            m_listLinkerOptions->SetColumnWidth(1, wxLIST_AUTOSIZE);
            m_selectedLnkOption = wxNOT_FOUND;
        }
    }
}

void CompilerLinkerOptionsPage::OnLinkerOptionDeSelected(wxListEvent& event)
{
#ifndef __WXMAC__
    m_selectedLnkOption = wxNOT_FOUND;
#endif
    event.Skip();
}

void CompilerLinkerOptionsPage::OnLinkerOptionSelected(wxListEvent& event)
{
    m_selectedLnkOption = event.m_itemIndex;
    event.Skip();
}
