#ifndef IEDITOR_H
#define IEDITOR_H

#include "wx/filename.h"
#include "wx/string.h"

//------------------------------------------------------------------
// Defines the interface to the editor control
//------------------------------------------------------------------
class IEditor {
public:
	IEditor(){}
	virtual ~IEditor(){}

	virtual wxString GetEditorText() = 0;
	virtual void SetEditorText(const wxString &text) = 0;
	virtual void SetCaretAt(long pos) = 0;
	virtual void ReloadFile() = 0;
	virtual long GetCurrentPosition() = 0;
	virtual const wxFileName &GetFileName() const = 0;
	virtual const wxString &GetProjectName() const = 0;
};

#endif //IEDITOR_H


