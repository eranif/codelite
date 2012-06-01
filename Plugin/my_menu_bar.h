#ifndef MYMENUBAR_H
#define MYMENUBAR_H

#include <wx/menu.h>

class MyMenuBar : public wxMenuBar
{
	wxMenuBar* m_mb;
public:
	MyMenuBar();
	virtual ~MyMenuBar();
	
	void Set(wxMenuBar* mb);
};

#endif // MYMENUBAR_H
