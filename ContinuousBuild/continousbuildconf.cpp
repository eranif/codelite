//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : continousbuildconf.cpp
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

#include "continousbuildconf.h"
ContinousBuildConf::ContinousBuildConf()
		: m_enabled(false)
		, m_parallelProcesses(1)
{
}

void ContinousBuildConf::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_enabled"), m_enabled);
	arch.Read(wxT("m_parallelProcesses"), m_parallelProcesses);
}

void ContinousBuildConf::Serialize(Archive& arch)
{
	arch.Write(wxT("m_enabled"), m_enabled);
	arch.Write(wxT("m_parallelProcesses"), m_parallelProcesses);
}
