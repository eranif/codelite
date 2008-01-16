#ifndef TREE_SYMBOL_PAGE_H
#define TREE_SYMBOL_PAGE_H

#include "wx/panel.h"
#include "wx/filename.h"

class SymbolTreePage : public wxPanel
{
	wxFileName m_fileName;
public:
	SymbolTreePage(wxWindow *parent);
	virtual ~SymbolTreePage();

	//accessors
	void SetFileName(const wxFileName &filename);
	const wxFileName &GetFilename() const {return m_fileName;}
};
#endif //TREE_SYMBOL_PAGE_H
