#include "confcallgraph.h"

ConfCallGraph::ConfCallGraph()
: m_gprofPath(wxT("")),
m_dotPath(wxT("")),
m_colorsEdge(1),
m_colorsNode(1),
m_tresholdNode(0),
m_tresholdEdge(0),
m_boxName(false),
m_boxParam(false)
{
}

ConfCallGraph::~ConfCallGraph()
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
	
	arch.Read(wxT("m_boxName"), m_boxName);
	arch.Read(wxT("m_boxParam"), m_boxParam);
}

void ConfCallGraph::Serialize(Archive& arch)
{
	arch.Write(wxT("m_gprofPath"), m_gprofPath);
	arch.Write(wxT("m_dotPath"), m_dotPath);
	
	arch.Write(wxT("m_colorsNode"), m_colorsNode);
	arch.Write(wxT("m_colorsEdge"), m_colorsEdge);
	
	arch.Write(wxT("m_tresholdNode"), m_tresholdNode);
	arch.Write(wxT("m_tresholdEdge"), m_tresholdEdge);
	
	arch.Write(wxT("m_boxName"), m_boxName);
	arch.Write(wxT("m_boxParam"), m_boxParam);
}
