#ifndef DAPOUTPUTPANE_HPP
#define DAPOUTPUTPANE_HPP

#include "DAPConsoleOutput.hpp"
#include "DAPModuleView.h"
#include "Notebook.h"
#include "clModuleLogger.hpp"
#include "dap/dap.hpp"

#include <wx/panel.h>

class DAPOutputPane : public wxPanel
{
    Notebook* m_notebook = nullptr;
    DAPConsoleOutput* m_consoleTab = nullptr;
    DAPModuleView* m_moduleTab = nullptr;

public:
    DAPOutputPane(wxWindow* parent, clModuleLogger& log);
    virtual ~DAPOutputPane();

    void AddEvent(dap::ModuleEvent* event);
    void AddEvent(dap::OutputEvent* event);
};

#endif // DAPOUTPUTPANE_HPP
