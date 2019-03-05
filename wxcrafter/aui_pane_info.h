#ifndef AUIPANEINFO_H
#define AUIPANEINFO_H

#include "wxcLib/json_node.h"
#include <wx/aui/framemanager.h>
#include <wx/propgrid/propgrid.h>

class wxPropertyGrid;
class AuiPaneInfo
{
    wxString m_name;
    wxString m_caption;
    wxString m_dockDirection;
    int m_layer;
    int m_row;
    int m_position;
    wxSize m_bestSize;
    wxSize m_minSize;
    wxSize m_maxSize;
    bool m_resizable;
    bool m_captionVisible;
    bool m_closeButton;
    bool m_minButton;
    bool m_maxButton;
    bool m_pinButton;
    bool m_toolbarPane;

public:
    AuiPaneInfo();
    virtual ~AuiPaneInfo();

    void Reset();

    wxString ToCppCode() const;
    wxString ToXRC(const wxString& objXRC) const;
    void Construct(wxPropertyGrid* pg) const;
    void OnChanged(wxPropertyGridEvent& e);

    void FromJSON(const JSONElement& json);
    JSONElement ToJSON() const;
};

#endif // AUIPANEINFO_H
