#include "dotwriter.h"
#include <wx/strconv.h>
#include <wx/file.h> 
#include <wx/msgdlg.h> 
#include <wx/math.h>
#include <math.h>


DotWriter::DotWriter()
{
	begin_graph = wxT("digraph\n{");
	end_graph = wxT("}\n");
	fontname = wxT("Arial");
	style = wxT("filled,rounded");
	shape = wxT("box");
	cwhite = wxT("white");
	cblack = wxT("black");
	hnode = wxT("");
	dedge = wxT("");
	hedge = wxT("");
	dlabel = wxT("");
	graph = wxT("");
	output = wxT("");
	mlines = NULL;
	m_mgr = NULL;
	dwcn = 0;
	dwce = 0;
	dwtn = 0;
	dwte = 0;
	dwbname = false;
	dwbparam = false;
	writedotfile = false;
}

DotWriter::~DotWriter()
{
	
}

void DotWriter::setLineParser(LineParserList *pLines )//, int numOfLines)
{
	mlines = pLines;
}

void DotWriter::setDotWriterFromDialogSettings(IManager *mgr)
{
	m_mgr = mgr;
	m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
	dwcn = confData.GetColorsNode();
	dwce = confData.GetColorsEdge();
	dwtn = confData.GetTresholdNode();
	dwte = confData.GetTresholdEdge();
	dwbname = confData.GetBoxName();
	dwbparam = confData.GetBoxParam();
}


void DotWriter::WriteToDotLanguade()
{
	int pl_index = 0;
	float pl_time = 0;
	bool is_node = false;
	wxArrayInt index_pl_nodes;
	
	if (mlines == NULL)
		return;
		
	graph = wxT("graph [ranksep=\"0.25\", fontname=") + fontname + wxT(", nodesep=\"0.125\"];");
	
	hnode = wxT("node [label=\"\\N\", fontsize=10.00, fontname=") + fontname + wxT(", style=\"") + style + wxT("\", height=0, width=0, shape=") + shape + wxT(", fontcolor=") + cwhite + wxT("];");

	hedge = wxT("edge [fontname=") + fontname + wxT("];");

	//graph []; -- not used
	
	output += begin_graph + wxT("\n") + graph + wxT("\n") + hnode + wxT("\n") + hedge + wxT("\n");
	
	LineParserList::compatibility_iterator it = mlines->GetFirst();
	
	while(it)
	{
		LineParser *line = it->GetData();
	
		//if (line->name.IsEmpty()) break;
		
		if(line->pline && wxRound(line->time) >= dwtn)
		{
			is_node = true;
			index_pl_nodes.Add(line->index);
			dlabel = wxString::Format(wxT("%i"),line->index); 
			dlabel += wxT(" [label=\"");
			dlabel += OptionsShortNameAndParameters(line->name);
			dlabel += wxT("\\n");
			dlabel += wxString::Format(wxT("%.2f"), line->time);
			dlabel += wxT("% \\n");
			dlabel += wxT("(");
			dlabel += wxString::Format(wxT("%.2f"), line->self); 
			dlabel += wxT("%)");
			dlabel += wxT("\\n");
			if(line->called0 != -1)
				dlabel += wxString::Format(wxT("%i"),line->called0) + wxT("x");
			//if(line->recursive)	
			//	dlabel += wxT(" (") + wxString::Format(wxT("%i"),line->called0 + line->called1) + wxT("x)");
			dlabel += wxT("\",fontcolor=\"");
			dlabel += DefineColorForLabel(ReturnIndexForColor(line->time, dwcn));
			dlabel += wxT("\", color=\"");
			dlabel += DefineColorForNodeEdge(ReturnIndexForColor(line->time, dwcn));
			//
			dlabel += wxT("\", fontsize=\"10.00\"];");
			//
			output += dlabel + wxT("\n");
			//
			dlabel.Clear();
		}
		it = it->GetNext();
	}
	
	it = mlines->GetFirst();
	
	while(it)
	{
		LineParser *line = it->GetData();
		
		if(line->pline)
		{
			pl_index = line->index; // index for primary node
			pl_time = line->time; // time for primary node
		}
		
		if (line->child && IsInArray(line->nameid,index_pl_nodes) && IsInArray(pl_index,index_pl_nodes) && (wxRound(pl_time) >= dwte))
		{
			dedge = wxString::Format(wxT("%i"), pl_index);
			dedge += wxT(" -> ");
			dedge += wxString::Format(wxT("%i"),line->nameid);
			dedge += wxT(" [color=\"");
			dedge += DefineColorForNodeEdge(ReturnIndexForColor(pl_time, dwce)); // color by primary node
			dedge += wxT("\", label=\"");
			//if(line->self != -1)
			//dedge += wxString::Format(wxT("%.2f"),line->self) + wxT("%");
			//dedge += wxT("\\n");
			dedge += wxString::Format(wxT("%i"),line->called0);
			dedge += wxT("x"); 
			dedge += wxT("\" ,arrowsize=\"0.50\", fontsize=\"10.00\", fontcolor=\"");
			dedge += cblack;
			dedge += wxT("\", penwidth=\"2.00\"];"); // labeldistance=\"4.00\",	 		
			//
			output += dedge + wxT("\n");
			//
			dedge.Clear();		
		}
		it = it->GetNext();
	}
	output += end_graph;
	
	if (!is_node) // if the call graph is empty create new graph with label node
	{	 
		output = wxT("digraph e {0 [label=");
		output += wxT("\"Call Graph is empty, please check settings of node resolutions for this plugin!\"");
		output += wxT(", shape=none, height=2, width=2, fontname=Arial, fontsize=14.00];}");
	}
}

void DotWriter::SendToDotAppOutputDirectory(wxString apppathfolder)
{
	wxString dotfilespath = apppathfolder + stvariables::dotfilesdir + stvariables::sd;
	wxString dottxtpath = dotfilespath + stvariables::dottxtname;
	if(!wxDirExists(dotfilespath))
	{
		wxMkdir(dotfilespath.c_str());
	}
	
	if (wxFileExists(dottxtpath))
		wxRemoveFile(dottxtpath);	
	
	//create new txt file	
	wxFile pFile(dottxtpath, wxFile::write);
	pFile.Open(dottxtpath, wxFile::write);
	
	if(pFile.IsOpened()) 
	{
		writedotfile = pFile.Write(output);
		pFile.Close();
	}
	else
	{
		return;
	}
}

bool DotWriter::DotFileExist(wxString apppathfolder)
{
	wxString dotfilespath = apppathfolder + stvariables::dotfilesdir + stvariables::sd + stvariables::dottxtname;
	
	if (wxFileExists(dotfilespath) && writedotfile) 
		return true;
		else return false;	
}

wxString DotWriter::OptionsShortNameAndParameters(wxString name)
{
	if (name.Contains(wxT('(')) && name.Contains(wxT(')')) && dwbname)
	{
		wxString out = name.BeforeFirst(wxT('('));
		out += wxT("()"); // for function return just ()
		return out;
	
	}
	else if (name.Contains(wxT('(')) && name.Contains(wxT(')')) && dwbparam)
	{
		wxString out = name.BeforeFirst(wxT('('));
		wxString sub = name.AfterFirst(wxT('(')).BeforeFirst(wxT(')'));
		if (sub.IsEmpty())
		{
			out += wxT("\\n(\\n)");
			return out;
		}
		else if(sub.Contains(wxT(",")))
		{
			sub.Replace(wxT(","), wxT(",\\n"));
			out += wxT("\\n(\\n") + sub + wxT("\\n)");
			return out;
		}
		else
		{
			out += wxT("\\n(\\n") + sub + wxT("\\n)");
			return out;
		
		}
	}
	else return name;
}

int DotWriter::ReturnIndexForColor(float time, int dwc)
{
	int index = 0;
	struct BarvaRozsah
	{
		int dolniIndex;
		int horniIndex;       
		int indexBarvy;
	};
	
	BarvaRozsah *barvyVybrane;
	barvyVybrane = new BarvaRozsah[dwc];
	
	if(dwc == 1)
    {
     barvyVybrane[0].dolniIndex = 0;
     barvyVybrane[0].horniIndex = 100;
     barvyVybrane[0].indexBarvy = 0;
    }
    else if(dwc == 2)
    {
     barvyVybrane[0].dolniIndex = 0;
     barvyVybrane[0].horniIndex = 50;
     barvyVybrane[0].indexBarvy = 0;
     barvyVybrane[1].dolniIndex = 51;
     barvyVybrane[1].horniIndex = 100;
     barvyVybrane[1].indexBarvy = 9;
    }
    else if(dwc > 2 && dwc < 11)
    {
	int pocetIntervalu = dwc-1;
	float krokIntervalu = 8.0/(float)pocetIntervalu;
	float delkaIntervalu = 100.0/(float)dwc;
	delkaIntervalu = round(delkaIntervalu);
	int poModulu = 8 % pocetIntervalu;
	float zbytekPripocet = (float)poModulu/pocetIntervalu;
	float pripocitat = 0.0;
	
		for(int i=0; i<dwc; i++)
		{
			pripocitat += zbytekPripocet;
			if(i==0)
			{
			barvyVybrane[i].dolniIndex = 0;
			barvyVybrane[i].horniIndex = (int)delkaIntervalu;
			barvyVybrane[i].indexBarvy = 0;       
			}        
			else if(i==(dwc-1))
			{
			barvyVybrane[i].dolniIndex = barvyVybrane[i-1].horniIndex+1;
			barvyVybrane[i].horniIndex = 100;
			barvyVybrane[i].indexBarvy = 9;       
			}
			else if(i>0 && i<(dwc-1))
			{
				int pripocet = 0;
				if(0.8 < pripocitat && pripocitat < 1.2)
				{
					pripocet = 1;
					pripocitat = 0;
				}
				barvyVybrane[i].dolniIndex = barvyVybrane[i-1].horniIndex + 1;
				barvyVybrane[i].horniIndex = barvyVybrane[i-1].horniIndex + (int)delkaIntervalu;
				barvyVybrane[i].indexBarvy = (int)round((float)barvyVybrane[i-1].indexBarvy + krokIntervalu) + pripocet;  
			}       
		}
	}
	
	for(int i=0; i<dwc; i++)
	{
		if ((int)barvyVybrane[i].dolniIndex <= (int)time && (int)time <= (int)barvyVybrane[i].horniIndex)
		{
			index = (int)barvyVybrane[i].indexBarvy;
			break;
		}
	}
	return index;
}

wxString DotWriter::DefineColorForNodeEdge(int index)
{
	wxString colors[10] = {wxT("#006837"), wxT("#1a9850"), wxT("#66bd63"), wxT("#a6d96a"), wxT("#d9ef8b"), wxT("#fee08b"), wxT("#fdae61"), wxT("#f46d43"), wxT("#d73027") ,wxT("#a50026")};
	return colors[index];
}

bool DotWriter::IsInArray(int index, wxArrayInt arr)
{
	for(unsigned int i = 0; i < arr.GetCount(); i++)
	{
		if (arr.Item(i) == index) return true;
	}
	return false;

}

wxString DotWriter::DefineColorForLabel(int index)
{
	if ((index < 3) || (index > 6))
	{
		return cwhite;
	}
	else
	{
		return cblack;
	}
}