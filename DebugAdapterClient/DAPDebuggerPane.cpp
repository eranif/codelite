#include "DAPDebuggerPane.h"

#include "DAPBreakpointsView.h"
#include "DAPMainView.h"
#include "DAPOutputPane.hpp"
#include "DAPWatchesView.h"

namespace
{
const wxString DAP_DEBUGGER_PANE = _("Debugger Client");
const wxString DAP_MAIN_VIEW = _("Thread, stacks & variables");
const wxString DAP_BREAKPOINTS_VIEW = _("Breakpoints");
const wxString DAP_WATCHES_VIEW = _("Watches");
} // namespace

DAPDebuggerPane::DAPDebuggerPane(wxWindow* parent, DebugAdapterClient* adapter, clModuleLogger& log)
    : wxPanel(parent)
    , LOG(log)
    , m_dapPlugin(adapter)
{
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                          kNotebook_Default | kNotebook_AllowDnD | kNotebook_FixedWidth);

    SetSizer(new wxBoxSizer(wxVERTICAL));
    GetSizer()->Add(m_book, 1, wxEXPAND);

    m_mainView = new DAPMainView(m_book, m_dapPlugin, LOG);
    m_book->AddPage(m_mainView, DAP_MAIN_VIEW, true);

    m_watchesView = new DAPWatchesView(m_book, m_dapPlugin, LOG);
    m_book->AddPage(m_watchesView, DAP_WATCHES_VIEW, false);

    m_breakpointsView = new DAPBreakpointsView(m_book, m_dapPlugin, LOG);
    m_book->AddPage(m_breakpointsView, DAP_BREAKPOINTS_VIEW, false);

    GetSizer()->Fit(this);
}

DAPOutputPane* DAPDebuggerPane::GetOutputView() const { return m_mainView->GetOutputPane(); }

void DAPDebuggerPane::Clear()
{
    m_mainView->Clear();
    m_watchesView->Clear();
    m_breakpointsView->Clear();
}
