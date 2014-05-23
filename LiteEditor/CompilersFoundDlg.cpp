#include "CompilersFoundDlg.h"
#include <windowattrmanager.h>
#include <compiler.h>

class CompilersFoundDlgItemData : public wxClientData
{
    CompilerPtr m_compiler;
public:
    CompilersFoundDlgItemData(CompilerPtr compiler) : m_compiler( compiler ) {}
    virtual ~CompilersFoundDlgItemData() {}
    
    CompilerPtr GetCompiler() const {
        return m_compiler;
    }
};

CompilersFoundDlg::CompilersFoundDlg(wxWindow* parent, const ICompilerLocator::CompilerVec_t& compilers)
    : CompilersFoundDlgBase(parent)
{
    // Add the categories
    std::map<wxString, wxDataViewItem> categories;
    for(size_t i=0; i<compilers.size(); ++i) {
        if ( categories.count(compilers.at(i)->GetCompilerFamily()) == 0 ) {
            wxVector<wxVariant> cols;
            cols.push_back(compilers.at(i)->GetCompilerFamily());
            wxDataViewItem item = m_dataviewModel->AppendItem(wxDataViewItem(0), cols);
            categories.insert( std::make_pair(compilers.at(i)->GetCompilerFamily(), item) );
        }
    }

    for(size_t i=0; i<compilers.size(); ++i) {
        CompilerPtr compiler = compilers.at(i);
        wxDataViewItem parent = categories.find( compiler->GetCompilerFamily() )->second;
        wxVector<wxVariant> cols;
        cols.push_back( compiler->GetName() );
        cols.push_back( compiler->GetInstallationPath() );
        m_dataviewModel->AppendItem(parent, cols, new CompilersFoundDlgItemData(compiler));
    }
    std::map<wxString, wxDataViewItem>::iterator iter = categories.begin();
    for(; iter != categories.end(); ++iter ) {
        m_dataview->Expand( iter->second );
    }
    WindowAttrManager::Load(this, "CompilersFoundDlg");
}

CompilersFoundDlg::~CompilersFoundDlg()
{
    WindowAttrManager::Save(this, "CompilersFoundDlg");
}

