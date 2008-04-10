#ifndef __wxtabcontainer__
#define __wxtabcontainer__

#include "wx/panel.h"
#include <wx/dynarray.h>

class CustomTab;
class wxMenu;

class wxTabContainer : public wxPanel
{
	int m_orientation;
	wxSizer *m_tabsSizer;
	CustomTab *m_draggedTab;
	wxArrayPtrVoid m_history;
	wxMenu *m_rightClickMenu;
	
protected:
	void Initialize();

	//add page to history
	void PushPageHistory(CustomTab *page);

	//remove page from the history by its value
	//after the page removal, all items in the history
	//are updated if needed
	void PopPageHistory(CustomTab *page);
	
	void DoRemoveTab(CustomTab *deleteTab, bool deleteIt, bool notify = false);
	
	void EnsureVisible(CustomTab *tab);
	
	bool IsVisible(CustomTab *tab);
	
public:
	wxTabContainer(wxWindow *win, wxWindowID id = wxID_ANY, int orientation = wxLEFT, long style = 0);
	virtual ~wxTabContainer();

	void AddTab(CustomTab *tab);
	CustomTab *GetSelection();

	void SetSelection(CustomTab *tab, bool notify = false);
	CustomTab *IndexToTab(size_t page);
	size_t TabToIndex(CustomTab *tab);
	size_t GetTabsCount();
	
	void SetRightClickMenu(wxMenu* rightClickMenu) {this->m_rightClickMenu = rightClickMenu;}
	wxMenu* GetRightClickMenu() {return m_rightClickMenu;}
	
	void Resize();
	
	/**
	 * \brief delete tab from the tab container (also destroying it)
	 * \param deleteTab tab to delete
	 * \param notify set this to true if you wish to receive wxEVT_COMMAND_BOOK_PAGE_CLOSING & wxEVT_COMMAND_BOOK_PAGE_CLOSED
	 */
	void DeletePage(CustomTab *deleteTab, bool notify);
	
	/**
	 * \brief remove tab from the tab container (the tab will *not* be destroyed)
	 * \param removePage tab to remove
	 * \param notify set this to true if you wish to receive wxEVT_COMMAND_BOOK_PAGE_CLOSING & wxEVT_COMMAND_BOOK_PAGE_CLOSED
	 */
	void RemovePage(CustomTab *removePage, bool notify);
	
	/**
	 * \brief return previous selection
	 * \return previous selection, or NULL if history list is empty
	 */
	CustomTab* GetPreviousSelection();
	
	/**
	 * \brief set tab as being dragged
	 * \param tab
	 */
	void SetDraggedTab(CustomTab *tab);

	/**
	 * \brief swap tab with the dragged one
	 * \param tab swap dragged tab this one
	 */
	void SwapTabs(CustomTab *tab);

	//Setters
	void SetOrientation(const int& orientation) ;
	const int& GetOrientation() const {return m_orientation;}
	
	
	//Setters
	void SetHistory(const wxArrayPtrVoid& history) {this->m_history = history;}
	const wxArrayPtrVoid& GetHistory() const {return m_history;}
	
	DECLARE_EVENT_TABLE()
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnSizeEvent(wxSizeEvent &e);
	virtual void OnLeaveWindow(wxMouseEvent &e);
	virtual void OnLeftUp(wxMouseEvent &e);
	virtual void OnDeleteTab(wxCommandEvent &e);


};
#endif // __wxtabcontainer__
