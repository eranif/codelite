#ifndef CLFILEORFOLDERDROPTARGET_H
#define CLFILEORFOLDERDROPTARGET_H

#include <wx/dnd.h> // Base class: wxFileDropTarget
#include "codelite_exports.h"
#include <wx/event.h>

/**
 * @class clFileOrFolderDropTarget
 * @brief a drop target class for files or folder
 * This class translates the "OnDrop" function into codelite events:
 * wxEVT_DND_FOLDER_DROPPED and wxEVT_DND_FILE_DROPPED that will be sent to the eventSink object
 * The list of files or folders is set in the event.GetString() method
 * of the event
 */
class WXDLLIMPEXP_SDK clFileOrFolderDropTarget : public wxFileDropTarget
{
    wxEvtHandler* m_sink;
public:
    clFileOrFolderDropTarget(wxEvtHandler* eventSink);
    virtual ~clFileOrFolderDropTarget();
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
};

#endif // CLFILEORFOLDERDROPTARGET_H
