//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : confcallgraph.cpp
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

#include "confcallgraph.h"

ConfCallGraph::ConfCallGraph()
: m_gprofPath(wxT("")),
m_dotPath(wxT("")),
m_colorsNode(1),
m_colorsEdge(1),
m_tresholdNode(0),
m_tresholdEdge(0),
m_boxHideParams(false),
m_boxStripParams(false),
m_boxHideNamespaces(false)
{
}

void ConfCallGraph::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_gprofPath"), m_gprofPath);
	arch.Read(wxT("m_dotPath"), m_dotPath);
	
	arch.Read(wxT("m_colorsNode"), m_colorsNode);
	arch.Read(wxT("m_colorsEdge"), m_colorsEdge);
	
	arch.Read(wxT("m_tresholdNode"), m_tresholdNode);
	arch.Read(wxT("m_tresholdEdge"), m_tresholdEdge);
	
	arch.Read(wxT("m_boxHideParams"), m_boxHideParams);
	arch.Read(wxT("m_boxHideNamespaces"), m_boxHideNamespaces);
	arch.Read(wxT("m_boxStripParams"), m_boxStripParams);
}

void ConfCallGraph::Serialize(Archive& arch)
{
	arch.Write(wxT("m_gprofPath"), m_gprofPath);
	arch.Write(wxT("m_dotPath"), m_dotPath);
	
	arch.Write(wxT("m_colorsNode"), m_colorsNode);
	arch.Write(wxT("m_colorsEdge"), m_colorsEdge);
	
	arch.Write(wxT("m_tresholdNode"), m_tresholdNode);
	arch.Write(wxT("m_tresholdEdge"), m_tresholdEdge);
	
	arch.Write(wxT("m_boxHideParams"), m_boxHideParams);
	arch.Write(wxT("m_boxHideNamespaces"), m_boxHideNamespaces);
	arch.Write(wxT("m_boxStripParams"), m_boxStripParams);
}
