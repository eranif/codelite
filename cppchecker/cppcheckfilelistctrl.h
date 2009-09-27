#ifndef __cppcheckfilelistctrl__
#define __cppcheckfilelistctrl__

#include <wx/wxscintilla.h>
#include <set>

class CppCheckReportPage;
class CppCheckFileListCtrl : public wxScintilla
{
	CppCheckReportPage * m_report;
	std::set<wxString>   m_files;
public:
	CppCheckFileListCtrl(wxWindow * window);
	virtual ~CppCheckFileListCtrl();

	void AddFile         (const wxString &filename);
	void Clear           ();
	void SetReportPage   ( CppCheckReportPage *report );
	void OnHotspotClicked(wxScintillaEvent& e);

	DECLARE_EVENT_TABLE();
};
#endif // __cppcheckfilelistctrl__
