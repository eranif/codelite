#include "CompilersModifiedDlg.h"
#include <build_settings_config.h>

#define SELECT_COMPILER "<Select A Compiler>"

CompilersModifiedDlg::CompilersModifiedDlg(wxWindow* parent, const wxStringSet_t& deletedCompilers)
    : CompilersModifiedDlgBase(parent)
{
    wxArrayString compilers;
    compilers = BuildSettingsConfigST::Get()->GetAllCompilers();
    compilers.Insert(SELECT_COMPILER, 0);
    
    m_dvListCtrl->AppendColumn( new wxDataViewColumn( _("Copy Settings From"), 
                                new wxDataViewChoiceRenderer( compilers, wxDATAVIEW_CELL_EDITABLE, wxDVR_DEFAULT_ALIGNMENT), 
                                m_dvListCtrl->GetColumnCount(), 
                                150, 
                                wxALIGN_LEFT));

    wxStringSet_t::const_iterator iter = deletedCompilers.begin();
    for(; iter != deletedCompilers.end(); ++iter ) {
        wxVector<wxVariant> cols;
        cols.push_back(*iter);
        cols.push_back( SELECT_COMPILER );
        m_dvListCtrl->AppendItem(cols);
    }
}

CompilersModifiedDlg::~CompilersModifiedDlg()
{
}

void CompilersModifiedDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(true);
    wxStringMap_t table = GetReplacementTable();
    wxStringMap_t::iterator iter = table.begin();
    for(; iter != table.end(); ++iter) {
        if ( iter->second == SELECT_COMPILER ) {
            event.Enable(false);
            return;
        }
    }
}

wxStringMap_t CompilersModifiedDlg::GetReplacementTable() const
{
    wxStringMap_t table;
    for(size_t i=0; i<m_dvListCtrl->GetItemCount(); ++i) {
        wxVariant value;
        wxString oldCompiler, newCompiler;
        m_dvListCtrl->GetValue(value, i, 0);
        oldCompiler = value.GetString();
        
        m_dvListCtrl->GetValue(value, i, 1);
        newCompiler = value.GetString();
        
        table[oldCompiler] = newCompiler;
    }
    return table;
}
