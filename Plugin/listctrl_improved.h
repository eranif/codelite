#ifndef WXCHECKLISTCTRL_H
#define WXCHECKLISTCTRL_H

#include <wx/listctrl.h>
#include <wx/bitmap.h>

enum {
	wxLC_COLOUR_BACKGROUND = 0x00010000
};

class ListCtrlImproved : public wxListCtrl {

	wxBitmap m_checkedBmp;
	wxBitmap m_uncheckedBmp;

protected:
	void DoInitialize();

public:
	ListCtrlImproved(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxLC_REPORT);
	virtual ~ListCtrlImproved();

	////////////////////////////////////////////////
	// API
	////////////////////////////////////////////////

	/**
	 * @brief return true if 'item' checked
	 */
	bool IsChecked  (long item);

	/**
	 * @brief check/uncheck given item
	 */
	void Check      (long item, bool b);

	/**
	 * @brief add new item to the list and return its number
	 */
	long AppendRow();

	/**
	 * @brief set text at given item/col
	 * @param item item number
	 * @param col column number
	 * @param text text to set
	 */
	void SetTextColumn(long item, long col, const wxString &text);

	/**
	 * @brief create a checkbox column
	 * @param item
	 * @param col
	 * @param checked
	 */
	void SetCheckboxRow(long item, bool checked);

	/**
	 * @brief return the text of a given item/column
	 * @param item item number
	 * @param col column number
	 * @return the text
	 */
	wxString GetText(long item, long col);

	/**
	 * @brief add user data. The list control takes ownership on it and will delete it
	 * @param item the item to associate the data
	 * @param clientData
	 */
	bool SetItemClientData(long item, wxClientData *clientData);

	/**
	 * @brief return the selected item -1 if not found
	 * @return
	 */
	long GetSelection() const;
	DECLARE_EVENT_TABLE();
	void OnLeftDown(wxMouseEvent &event);
};

extern const wxEventType wxEVT_CMD_CHECKLIST_ITEM_CHECKED;

#endif // WXCHECKLISTCTRL_H
