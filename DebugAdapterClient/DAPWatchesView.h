#ifndef DAPWATCHESVIEW_H
#define DAPWATCHESVIEW_H

#include "DAPVariableListCtrl.hpp"
#include "UI.h"
#include "clModuleLogger.hpp"

class DebugAdapterClient;
class DAPWatchesView : public DAPWatchesViewBase
{
    DAPVariableListCtrl* m_list = nullptr;
    DebugAdapterClient* m_plugin = nullptr;
    clModuleLogger& LOG;

protected:
    void OnNewWatch(wxCommandEvent& event);
    void OnDeleteWatch(wxCommandEvent& event);
    void OnDeleteAll(wxCommandEvent& event);
    void OnDeleteAllUI(wxUpdateUIEvent& event);
    void OnDeleteWatchUI(wxUpdateUIEvent& event);

public:
    DAPWatchesView(wxWindow* parent, DebugAdapterClient* plugin, clModuleLogger& log);
    virtual ~DAPWatchesView();

    /**
     * @brief update the watches in the context of the current frame
     * @param current_frame_id
     */
    void Update(int current_frame_id);
    void UpdateChildren(int varId, dap::VariablesResponse* response);
};
#endif // DAPWATCHESVIEW_H
