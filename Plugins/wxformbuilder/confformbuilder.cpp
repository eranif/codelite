//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : confformbuilder.cpp
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

#include "confformbuilder.h"
ConfFormBuilder::ConfFormBuilder()
    : m_command(wxT("$(wxfb) $(wxfb_project)"))
{
}

void ConfFormBuilder::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_command"), m_command);
    arch.Read(wxT("m_fbPath"), m_fbPath);
}

void ConfFormBuilder::Serialize(Archive& arch)
{
    arch.Write(wxT("m_command"), m_command);
    arch.Write(wxT("m_fbPath"), m_fbPath);
}
