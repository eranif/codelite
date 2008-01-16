#ifndef FNB_POPUPDLG_H
#define FNB_POPUPDLG_H

#include <wx/dialog.h>
#include <map>
#include <wx/bitmap.h>

class wxListBox;
class wxFlatNotebook;
class wxPanel;

class wxTabNavigatorWindow : public wxDialog
{
protected:
	wxListBox *m_listBox;
	long m_selectedItem;
	std::map<int, int> m_indexMap;
	wxPanel *m_panel;
	static wxBitmap m_bmp;

protected:
	void CloseDialog();

public:
	/**
	 * Parameterized constructor
	 * \param parent dialog parent window
	 */
	wxTabNavigatorWindow(wxWindow* parent);

	/**
	 * Default constructor
	 */
	wxTabNavigatorWindow();

	/**
	 * Destructor
	 */
	virtual ~wxTabNavigatorWindow();

	/**
	 * Create the dialog, usually part of the two steps construction of a 
	 * dialog
	 * \param parent dialog parent window
	 */
	void Create(wxWindow* parent);

	/// Event handling
	void OnKeyUp(wxKeyEvent &event);
	void OnNavigationKey(wxNavigationKeyEvent &event);
	void OnItemSelected(wxCommandEvent &event);
	void OnPanelPaint(wxPaintEvent &event);
	void OnPanelEraseBg(wxEraseEvent &event);
	void PopulateListControl(wxFlatNotebook *book);
};

#endif // FNB_POPUPDLG_H

