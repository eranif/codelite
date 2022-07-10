#include "DAPOutputPane.hpp"

#include "Notebook.h"

#include <wx/sizer.h>

DAPOutputPane::DAPOutputPane(wxWindow* parent, clModuleLogger& log)
    : wxPanel(parent)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_notebook = new Notebook(this, wxID_ANY);
    GetSizer()->Add(m_notebook, 1, wxEXPAND);

    m_consoleTab = new DAPConsoleOutput(m_notebook, log);
    m_moduleTab = new DAPModuleView(m_notebook, log);
    m_notebook->AddPage(m_consoleTab, _("Output"), true);
    m_notebook->AddPage(m_moduleTab, _("Modules"), false);
}

DAPOutputPane::~DAPOutputPane() {}

void DAPOutputPane::AddEvent(dap::ModuleEvent* event)
{
    m_moduleTab->AddModuleEvent(event);
}

void DAPOutputPane::AddEvent(dap::OutputEvent* event)
{
    m_consoleTab->AddOutputEvent(event);
}