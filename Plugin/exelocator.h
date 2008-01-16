#ifndef ITEMLOCATOR_H
#define ITEMLOCATOR_H

#include "wx/string.h"

class ExeLocator 
{
public:
	ExeLocator (){}
	~ExeLocator (){}

	static bool Locate(const wxString &name, wxString &where);
};

#endif //ITEMLOCATOR_H


