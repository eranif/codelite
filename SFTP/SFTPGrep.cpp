#include "SFTPGrep.h"
#include "cl_config.h"

SFTPGrep::SFTPGrep(wxWindow* parent)
    : SFTPGrepBase(parent)
{
    wxString find_what = clConfig::Get().Read("sftp/grep/finw_what", wxString());
    wxString searchIn = clConfig::Get().Read("sftp/grep/search_in", wxString("*"));
    bool ignoreCase = clConfig::Get().Read("sftp/grep/ignore_case", false);
    bool wholeWord = clConfig::Get().Read("sftp/grep/whole_word", true);

    m_checkBoxIgnoreCase->SetValue(ignoreCase);
    m_checkBoxWholeWord->SetValue(wholeWord);
    m_textCtrlFindWhat->ChangeValue(find_what);
    m_textCtrlSeachIn->ChangeValue(searchIn);
}

SFTPGrep::~SFTPGrep()
{
    clConfig::Get().Write("sftp/grep/finw_what", m_textCtrlFindWhat->GetValue());
    clConfig::Get().Write("sftp/grep/search_in", m_textCtrlSeachIn->GetValue());
    clConfig::Get().Write("sftp/grep/ignore_case", m_checkBoxIgnoreCase->IsChecked());
    clConfig::Get().Write("sftp/grep/whole_word", m_checkBoxWholeWord->IsChecked());
}

void SFTPGrep::OnOkUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFindWhat->IsEmpty()); }

GrepData SFTPGrep::GetData() const
{
    GrepData gd;
    gd.SetFindWhat(m_textCtrlFindWhat->GetValue());
    gd.SetSearchIn(m_textCtrlSeachIn->GetValue());
    gd.SetIgnoreCase(m_checkBoxIgnoreCase->IsChecked());
    gd.SetWholeWord(m_checkBoxWholeWord->IsChecked());
    return gd;
}

wxString GrepData::GetGrepCommand(const wxString& path) const
{
    wxString command;
    // use -n -H to include the file:line
    command << "find " << path << " -name \"" << GetSearchIn() << "\" | xargs grep -n -H ";
    if(IsIgnoreCase()) {
        command << " -i ";
    }
    if(IsWholeWord()) {
        command << " -w ";
    }
    command << " \"" << GetFindWhat() << "\"";
    return command;
}
