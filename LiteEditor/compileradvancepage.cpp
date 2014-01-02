#include <wx/tokenzr.h>
#include "compileradvancepage.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "EditDlg.h"
#include "event_notifier.h"

CompilerAdvancePage::CompilerAdvancePage( wxWindow* parent, const wxString &cmpname )
    : CompilerAdvanceBase( parent )
    , m_cmpname(cmpname)
{
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(m_cmpname);
    m_textObjectExtension->SetValue(cmp->GetObjectSuffix());
    m_textDependExtension->SetValue(cmp->GetDependSuffix());
    m_textPreprocessExtension->SetValue(cmp->GetPreprocessSuffix());
    m_checkBoxGenerateDependenciesFiles->SetValue(cmp->GetGenerateDependeciesFile());
    m_textCtrlGlobalIncludePath->SetValue(cmp->GetGlobalIncludePath());
    m_textCtrlGlobalLibPath->SetValue(cmp->GetGlobalLibPath());
    m_checkBoxReadObjectsFromFile->SetValue(cmp->GetReadObjectFilesFromList());
    m_checkBoxObjectNameSameAsFileName->SetValue(cmp->GetObjectNameIdenticalToFileName());

}

void CompilerAdvancePage::Save(CompilerPtr cmp)
{
    cmp->SetGenerateDependeciesFile(m_checkBoxGenerateDependenciesFiles->IsChecked());
    cmp->SetGlobalIncludePath(m_textCtrlGlobalIncludePath->GetValue());
    cmp->SetGlobalLibPath(m_textCtrlGlobalLibPath->GetValue());
    cmp->SetObjectSuffix(m_textObjectExtension->GetValue());
    cmp->SetDependSuffix(m_textDependExtension->GetValue());
    cmp->SetPreprocessSuffix(m_textPreprocessExtension->GetValue());
    cmp->SetReadObjectFilesFromList(m_checkBoxReadObjectsFromFile->IsChecked());
    cmp->SetObjectNameIdenticalToFileName(m_checkBoxObjectNameSameAsFileName->IsChecked());
}

void CompilerAdvancePage::OnEditIncludePaths(wxCommandEvent& event)
{
    wxString curIncludePath = m_textCtrlGlobalIncludePath->GetValue();
    curIncludePath = wxJoin( ::wxStringTokenize(curIncludePath, ";", wxTOKEN_STRTOK), '\n', '\0' );
    wxString newIncludePath = ::clGetTextFromUser(curIncludePath, EventNotifier::Get()->TopFrame());
    newIncludePath.Trim().Trim(false);
    if ( !newIncludePath.IsEmpty() ) {
        newIncludePath = wxJoin( ::wxStringTokenize(newIncludePath, "\n\r", wxTOKEN_STRTOK), ';', '\0' );
        m_textCtrlGlobalIncludePath->ChangeValue( newIncludePath );
    }
}

void CompilerAdvancePage::OnEditLibraryPaths(wxCommandEvent& event)
{
    wxString curLibPath = m_textCtrlGlobalLibPath->GetValue();
    curLibPath = wxJoin( ::wxStringTokenize(curLibPath, ";", wxTOKEN_STRTOK), '\n', '\0' );
    wxString newLibPath = ::clGetTextFromUser(curLibPath, EventNotifier::Get()->TopFrame());
    newLibPath.Trim().Trim(false);
    if ( !newLibPath.IsEmpty() ) {
        newLibPath = wxJoin( ::wxStringTokenize(newLibPath, "\n\r", wxTOKEN_STRTOK), ';', '\0' );
        m_textCtrlGlobalLibPath->ChangeValue( newLibPath );
    }
}
