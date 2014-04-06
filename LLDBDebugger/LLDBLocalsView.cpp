#include "LLDBLocalsView.h"
#include "lldbdebugger-plugin.h"

LLDBLocalsView::LLDBLocalsView(wxWindow* parent, LLDBDebuggerPlugin* plugin)
    : LLDBLocalsViewBase(parent)
    , m_plugin(plugin)
{
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
}

void LLDBLocalsView::OnLLDBStopped(LLDBEvent& event)
{
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
