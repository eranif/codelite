#ifndef __notebooknavdialog__
#define __notebooknavdialog__

#include <wx/dialog.h>
#include <map>
#include <wx/bitmap.h>

class wxListBox;
class Notebook;
class CustomTab;

class NotebookNavDialog : public wxDialog
{
protected:
	wxListBox *m_listBox;
	size_t m_selectedItem;
	std::map< int, CustomTab* > m_tabsIndex;
	CustomTab* m_selTab;
	
protected:
	void CloseDialog();

public:
	/**
	 * Parameterized constructor
	 * \param parent dialog parent window
	 */
	NotebookNavDialog(wxWindow* parent);

	/**
	 * Default constructor
	 */
	NotebookNavDialog();

	/**
	 * Destructor
	 */
	virtual ~NotebookNavDialog();

	/**
	 * Create the dialog, usually part of the two steps construction of a 
	 * dialog
	 * \param parent dialog parent window
	 */
	void Create(wxWindow* parent);
	
	CustomTab *GetSelection(){return m_selTab;}
	
	/// Event handling
	void OnKeyUp(wxKeyEvent &event);
	void OnNavigationKey(wxNavigationKeyEvent &event);
	void OnItemSelected(wxCommandEvent &event);
	void PopulateListControl(Notebook *book);
};

#endif // __notebooknavdialog__
