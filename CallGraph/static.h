/***************************************************************
 * Name:      static.h
 * Purpose:   Header to create static variables.
 * Author:    Vaclav Sprucek
 * Created:   2012-03-04
 * Copyright: Vaclav Sprucek 
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef __CALLGRAPH_STATIC_H__
#define __CALLGRAPH_STATIC_H__

#include <wx/string.h>

const wxString	GMON_FILENAME_OUT = "gmon.out";
const wxString	DOT_FILENAME_PNG = "dot.png";
const wxString	DOT_FILENAME_TXT = "dot.txt";
const wxString	CALLGRAPH_DIR = "CallGraph";

	#ifdef __WXMSW__
		const wxString	GPROF_FILENAME_EXE = "gprof.exe";
		const wxString	DOT_FILENAME_EXE = "dot.exe";
		const wxString	EXECUTABLE_EXTENSION = "exe";
	#else
		const wxString	GPROF_FILENAME_EXE = "gprof";
		const wxString	DOT_FILENAME_EXE = "dot";
		const wxString	EXECUTABLE_EXTENSION = "";
#endif

#endif // __CALLGRAPH_STATIC_H__
