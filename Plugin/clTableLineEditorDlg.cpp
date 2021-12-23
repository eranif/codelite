#include "clTableLineEditorDlg.h"

#include "ColoursAndFontsManager.h"

clTableLineEditorDlg::clTableLineEditorDlg(wxWindow* parent, const wxArrayString& columns, const wxArrayString& data)
    : clTableLineEditorBaseDlg(parent)
    , m_columns(columns)
    , m_data(data)
{
    if(!m_columns.IsEmpty()) {
        m_listBoxColumns->Append(m_columns);
        m_listBoxColumns->SetSelection(0);
        DoItemSelected(0);
    }
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_stc);
    m_stc->SetReadOnly(true);
    CenterOnParent();
}

clTableLineEditorDlg::~clTableLineEditorDlg() {}

void clTableLineEditorDlg::OnColumnSelected(wxCommandEvent& event) { DoItemSelected(event.GetSelection()); }

void clTableLineEditorDlg::DoItemSelected(int index)
{
    if((index != wxNOT_FOUND) && (index < (int)m_data.size())) {
        m_stc->SetReadOnly(false);
        m_stc->SetText(m_data[index]);
        m_stc->SetReadOnly(true);
    }
}
