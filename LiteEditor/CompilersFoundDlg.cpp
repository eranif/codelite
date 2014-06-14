#include "CompilersFoundDlg.h"
#include <windowattrmanager.h>
#include <compiler.h>
#include <includepathlocator.h>

class MyCompilersFoundModel : public CompilersFoundModel
{
    CompilersFoundDlg* m_dlg;
public:
    MyCompilersFoundModel(CompilersFoundDlg* dlg) : m_dlg(dlg) {}
    virtual ~MyCompilersFoundModel() {}


    virtual bool GetAttr(const wxDataViewItem &item,
                         unsigned int col,
                         wxDataViewItemAttr & attr) const 
    {
        CompilerPtr compiler = m_dlg->GetCompiler(item);
        if ( compiler && m_dlg->IsDefaultCompiler(compiler) ) {
            attr.SetBold( true );
            return true;
        }
        return false;
    }
};

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
    m_allCompilers = compilers;
    
    // Replace the model with a custom one
    unsigned int colCount = m_dataviewModel->GetColCount();
    m_dataviewModel = new MyCompilersFoundModel(this);
    m_dataviewModel->SetColCount( colCount );
    m_dataview->AssociateModel( m_dataviewModel.get() );

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
        if ( m_defaultCompilers.count(compiler->GetCompilerFamily()) == 0 ) {
            compiler->SetIsDefault(true); // Per family
            m_defaultCompilers.insert( std::make_pair(compiler->GetCompilerFamily(), compiler) );
            MSWUpdateToolchain(compiler);
        }
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

void CompilersFoundDlg::OnItemActivated(wxDataViewEvent& event)
{
    CompilerPtr compiler = GetCompiler(event.GetItem());
    if ( compiler ) {
        m_defaultCompilers.erase( compiler->GetCompilerFamily() );
        compiler->SetIsDefault(true);
        m_defaultCompilers.insert( std::make_pair(compiler->GetCompilerFamily(), compiler) );
        m_dataview->UnselectAll();
        m_dataview->CallAfter( &wxDataViewCtrl::Refresh, true, (const wxRect*)NULL );
        MSWUpdateToolchain( compiler );
    }
}

CompilerPtr CompilersFoundDlg::GetCompiler(const wxDataViewItem& item) const
{
    CompilersFoundDlgItemData* itemData = dynamic_cast<CompilersFoundDlgItemData*>(m_dataviewModel->GetClientObject(item));
    if ( itemData ) {
        return itemData->GetCompiler();
    }
    return CompilerPtr(NULL);
}

bool CompilersFoundDlg::IsDefaultCompiler(CompilerPtr compiler) const
{
    if ( m_defaultCompilers.count(compiler->GetCompilerFamily()) == 0 )
        return false;
    CompilerPtr defaultCompiler = m_defaultCompilers.find(compiler->GetCompilerFamily())->second;
    return defaultCompiler->GetName() == compiler->GetName();
}

void CompilersFoundDlg::MSWUpdateToolchain(CompilerPtr compiler)
{
    wxUnusedVar( compiler );
#ifdef __WXMSW__
    if ( compiler->GetCompilerFamily() == COMPILER_FAMILY_MINGW ) {
        // Clang and VC lacks 2 tools: make and windres
        // so we copy those from the default MinGW compiler
        wxString make = compiler->GetTool("MAKE");
        wxString resourceCompiler = compiler->GetTool("ResourceCompiler");
        
        for(size_t i=0; i<m_allCompilers.size(); ++i) {
            CompilerPtr c = m_allCompilers.at(i);
            if ( c->GetCompilerFamily() == COMPILER_FAMILY_CLANG || c->GetCompilerFamily() == COMPILER_FAMILY_VC) {
                c->SetTool("MAKE", make);
                c->SetTool("ResourceCompiler", resourceCompiler);
                
                if ( c->GetCompilerFamily() == COMPILER_FAMILY_CLANG ) {
                    // Clang under Windows, needs the include paths from the MinGW compiler
                    IncludePathLocator locator(NULL);
                    wxArrayString includePaths, excludePaths;
                    locator.Locate(includePaths, excludePaths, false, compiler->GetTool("CXX"));
                    
                    // Convert the include paths to semi colon separated list
                    wxString mingwIncludePaths = wxJoin(includePaths, ';');
                    c->SetGlobalIncludePath( mingwIncludePaths );
                }
            }
        }
    }
#endif
}
