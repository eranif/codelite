#include "CompilersModifiedDlg.h"
#include <build_settings_config.h>
#include <windowattrmanager.h>

#define SELECT_COMPILER "<Click to select a compiler...>"

CompilersModifiedDlg::CompilersModifiedDlg(wxWindow* parent, const wxStringSet_t& deletedCompilers)
    : CompilersModifiedDlgBase(parent)
    , m_enableOKButton(false)
{
    wxArrayString compilers;
    compilers = BuildSettingsConfigST::Get()->GetAllCompilers();
    compilers.Insert(SELECT_COMPILER, 0);
    
    wxStringSet_t::const_iterator iter = deletedCompilers.begin();
    for(; iter != deletedCompilers.end(); ++iter ) {
        m_table.insert( std::make_pair(*iter, SELECT_COMPILER) );
        wxPGProperty* prop = m_pgMgrCompilers->AppendIn( m_pgPropHeader,  new wxEnumProperty( *iter, wxPG_LABEL, compilers) );
        m_props.push_back( prop );
        
        wxString message;
        message << _("Create a new compiler named '") << *iter << "' by cloning an existing compiler";
        prop->SetHelpString( message );
    }
    WindowAttrManager::Load(this, "CompilersModifiedDlg");
}

CompilersModifiedDlg::~CompilersModifiedDlg()
{
    WindowAttrManager::Save(this, "CompilersModifiedDlg");
}

void CompilersModifiedDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( m_enableOKButton );
}

void CompilersModifiedDlg::OnValueChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    wxString newCompiler = event.GetProperty()->GetValueAsString();
    wxString oldCompiler = event.GetPropertyName();
    
    m_table.erase( oldCompiler );
    m_table[ oldCompiler ] = newCompiler;
    
    m_enableOKButton = true;
    wxStringMap_t::iterator iter = m_table.begin();
    for(; iter != m_table.end(); ++iter ) {
        if ( iter->second == SELECT_COMPILER ) {
            m_enableOKButton = false;
            break;
        }
    }
}
