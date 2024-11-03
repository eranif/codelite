#ifndef DAPDEBUGGERPANE_H
#define DAPDEBUGGERPANE_H

#include "Notebook.h"
#include "clModuleLogger.hpp"

#include <wx/panel.h>

class DebugAdapterClient;
class DAPMainView;
class DAPBreakpointsView;
class DAPWatchesView;
class DAPOutputPane;

class DAPDebuggerPane : public wxPanel
{
public:
    DAPDebuggerPane(wxWindow* parent, DebugAdapterClient* adapter, clModuleLogger& log);
    virtual ~DAPDebuggerPane();

    DAPMainView* GetMainView() const { return m_mainView; }
    DAPBreakpointsView* GetBreakpointsView() const { return m_breakpointsView; }
    DAPWatchesView* GetWatchesView() const { return m_watchesView; }
    DAPOutputPane* GetOutputView() const;
    void Clear();

private:
    clModuleLogger& LOG;
    Notebook* m_book = nullptr;
    DebugAdapterClient* m_dapPlugin = nullptr;
    DAPMainView* m_mainView = nullptr;
    DAPBreakpointsView* m_breakpointsView = nullptr;
    DAPWatchesView* m_watchesView = nullptr;
};
#endif // DAPDEBUGGERPANE_H
