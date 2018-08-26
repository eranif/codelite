//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : stack_walker.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "stack_walker.h"
#include "wx/txtstrm.h"

#if wxUSE_STACKWALKER

StackWalker::StackWalker(wxTextOutputStream* output)
    : m_output(output)
{
}

StackWalker::~StackWalker() {}

void StackWalker::OnStackFrame(const wxStackFrame& frame)
{
    *m_output << frame.GetModule() << wxT(" ") << frame.GetName() << wxT(" ") << frame.GetFileName() << wxT(" ")
              << (int)frame.GetLine() << wxT("\n");
}
#endif
