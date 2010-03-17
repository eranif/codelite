#ifndef OutputPaneBook_H
#define OutputPaneBook_H

#include <wx/choicebk.h>
#include <wx/bitmap.h>
#include <wx/imaglist.h>
#include <vector>
#include <wx/sizer.h>

class OutputPaneBook : public wxChoicebook
{
	std::vector<wxBitmap> m_bmps;
protected:
	wxImageList* GetImageList() const { return wxChoicebook::GetImageList(); }
public:
	OutputPaneBook(wxWindow *parent,
	               wxWindowID id,
	               const wxPoint& pos = wxDefaultPosition,
	               const wxSize& size = wxDefaultSize,
	               long style = 0,
	               const wxString& name = wxEmptyString)
			: wxChoicebook(parent, id, pos, size, style, name) 
	{
		// Hide the tabs
		GetControlSizer()->Hide((size_t)0);
		
		AssignImageList( new wxImageList(16, 16, true) );
		Layout();
	}

	virtual ~OutputPaneBook() {}

	void AddPage(wxWindow *page, const wxString &text, bool select, const wxBitmap &bmp) {
		// First add the page to the notebook
		int imgId = GetImageList()->Add(bmp);
		wxChoicebook::AddPage(page, text, select, imgId);

		// Keep a copy of the bitmap
		m_bmps.push_back( bmp );
	}

	const wxBitmap& GetBitmap(size_t indx) const {
		if (indx >= m_bmps.size())
			return wxNullBitmap;
		return m_bmps.at(indx);
	}
};

#endif // OutputPaneBook_H
