/*********************************************************************
 * Name:      	main.cpp
 * Purpose:   	Defines exported class and linked list container.
 * Author:    
 * Created:   
 * Copyright: 
 * License:   	wxWidgets license (www.wxwidgets.org)
 * 
 * Notes:		Note, that preprocessor symbol WXMAKINGDLL_MYLIB
 * 				tested in defs.h file must be defined in the compiler
 * 				settings if the symbols should be exported from 
 * 				DYNAMIC library. If not defined then all declarations
 * 				will be local (usefull for STATIC library).
 *********************************************************************/
 
// include main wxWidgets header file
#include <wx/wx.h>
// inlude headers for list container
#include <wx/list.h>
#include <wx/listimpl.cpp>
// include import/export definitions
#include "defs.h"

// example of exported class
class WXDLLIMPEXP_MYLIB ExportedClass : public wxObject
{
	public:
		ExportedClass() {;}
		~ExportedClass() {;}
};

// example of non-exported class
class LocalClass : public wxObject
{
	public:
		LocalClass() {;}
		~LocalClass() {;}
};

// example of exported wxWidgets linked list container
WX_DECLARE_LIST_WITH_DECL(ExportedClass, ExportedList, class WXDLLIMPEXP_MYLIB);
// example of non-exported list
WX_DECLARE_LIST(LocalClass, LocalList);

WX_DEFINE_EXPORTED_LIST(ExportedList);
WX_DEFINE_LIST(LocalList);
