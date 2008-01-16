#ifndef DROPFILETARGET_H
#define DROPFILETARGET_H

#include <wx/dnd.h>
class FileDropTarget : public wxFileDropTarget {
public:
	FileDropTarget();
	virtual ~FileDropTarget();
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
};
#endif //DROPFILETARGET_H
