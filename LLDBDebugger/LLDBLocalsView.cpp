#include "LLDBLocalsView.h"
#include "lldbdebugger-plugin.h"

// Use custom renderer
class LLDBLocalsModelReal : public LLDBLocalsModel
{
    wxDataViewCtrl* m_view;
    
private:
    wxDataViewCtrl* GetView() const {
        return m_view;
    }
    
public:
    LLDBLocalsModelReal() {}
    virtual ~LLDBLocalsModelReal() {}
    
    bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const {
        
        LLDBLocalVariableClientData* cd = dynamic_cast<LLDBLocalVariableClientData*>(this->GetClientObject(item));
        if ( cd && cd->GetVariable()->GetLLDBValue().GetValueDidChange() ) {
            attr.SetColour("RED");
        }
        return true;
    }
};

LLDBLocalsView::LLDBLocalsView(wxWindow* parent, LLDBDebuggerPlugin* plugin)
    : LLDBLocalsViewBase(parent)
    , m_plugin(plugin)
{
    // Replace the model with custom one
    int colCount = m_dataviewModel->GetColCount();
    m_dataviewModel.reset( new LLDBLocalsModelReal );
    m_dataviewModel->SetColCount( colCount );
    m_dataview->AssociateModel(m_dataviewModel.get() );

    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STARTED, &LLDBLocalsView::OnLLDBStarted, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_EXITED,  &LLDBLocalsView::OnLLDBExited,  this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STOPPED, &LLDBLocalsView::OnLLDBStopped, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_RUNNING, &LLDBLocalsView::OnLLDBRunning, this);
}

LLDBLocalsView::~LLDBLocalsView()
{
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_STARTED, &LLDBLocalsView::OnLLDBStarted, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_EXITED,  &LLDBLocalsView::OnLLDBExited,  this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_STOPPED, &LLDBLocalsView::OnLLDBStopped, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_RUNNING, &LLDBLocalsView::OnLLDBRunning, this);
}

void LLDBLocalsView::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    m_dataviewModel->Clear();
}

void LLDBLocalsView::OnLLDBRunning(LLDBEvent& event)
{
    event.Skip();
    m_dataviewModel->Clear();
}

void LLDBLocalsView::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    m_dataviewModel->Clear();
}

void LLDBLocalsView::OnLLDBStopped(LLDBEvent& event)
{
    // FIXME: optimize this to only retrieve the top levle variables
    // the children should be obtained in the 'OnItemExpading' event handler
    event.Skip();
    m_dataviewModel->Clear();
    LLDBLocalVariable::Vect_t locals = m_plugin->GetLLDB()->GetLocalVariables();
    for(size_t i=0; i<locals.size(); ++i) {
        DoAddVariableToView(locals.at(i), wxDataViewItem(NULL));
    }
}

void LLDBLocalsView::DoAddVariableToView(LLDBLocalVariable::Ptr_t variable, const wxDataViewItem& parent)
{
    wxVector<wxVariant> cols;
    cols.push_back( variable->GetName() );
    cols.push_back( variable->GetSummary() );
    cols.push_back( variable->GetValue() );
    cols.push_back( variable->GetType() );
    wxDataViewItem item = m_dataviewModel->AppendItem(parent, cols, new LLDBLocalVariableClientData(variable) );
    if ( variable->HasChildren() ) {
        LLDBLocalVariable::Vect_t& children = variable->GetChildren();
        for(size_t i=0; i<children.size(); ++i) {
            DoAddVariableToView(children.at(i), item);
        }
    }
}
