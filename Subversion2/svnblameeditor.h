#ifndef SVNBLAMEEDITOR_H
#define SVNBLAMEEDITOR_H

#include "wx/wxscintilla.h" // Base class
#include <vector>

struct BlameLineInfo {
	wxString revision;
	int      style;
};

class SvnBlameEditor : public wxScintilla {
	std::vector<BlameLineInfo> m_lineInfo;
	int                        m_lineNumber;
protected:
	void Initialize();

public:
	SvnBlameEditor(wxWindow *win);
	virtual ~SvnBlameEditor();

	void SetText(const wxString &text);

	DECLARE_EVENT_TABLE()
	void OnContextMenu      (wxContextMenuEvent &event);
	void OnHighlightRevision(wxCommandEvent &event);
};

#endif // SVNBLAMEEDITOR_H
