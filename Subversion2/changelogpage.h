#ifndef CHANGELOGPAGE_H
#define CHANGELOGPAGE_H

#include "subversion2_ui.h" // Base class: ChangeLogPageBase

class Subversion2;
class ChangeLogPage : public ChangeLogPageBase
{
	Subversion2 *m_plugin;
public:
	ChangeLogPage(wxWindow* parent, Subversion2* plugin);
	virtual ~ChangeLogPage();

	void AppendText(const wxString &text);
	void OnURL     (wxTextUrlEvent &event);
};

#endif // CHANGELOGPAGE_H
