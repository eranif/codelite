#include "gprofparser.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/strconv.h>

int cmpint(int* a, int* b) { return *b - *a; }

GprofParser::GprofParser()
{
	lineheader = false;
	primaryline = false;
	nameLen = 0;
	nameandid = NULL;
	isdot = false;
	iscycle = false;
	islom = false;
	isplus = false;

};

GprofParser::~GprofParser()
{
	lines.DeleteContents(true);
	lines.Clear();
};

void GprofParser::GprofParserStream(wxInputStream *m_pInputStream)
{
	readlinetext = wxT("");
	readlinetexttemp = wxT("");
	wxCSConv conv( wxT("ISO-8859-1") );
	wxTextInputStream text(*m_pInputStream);//, wxT(" \t"), conv);
	//wxFileOutputStream out( wxT("DEBUG.TXT") );
	//wxTextOutputStream tout( out );

	isspontaneous = false;
	calls.clear();

	while(!m_pInputStream->Eof()) {
		readlinetext = text.ReadLine();
		//tout.WriteString( readlinetext + wxT("\n") );
		nameLen = readlinetext.Len();

		if(readlinetext != wxT("")) {
			if(readlinetext == wxT("index % time    self  children    called     name")) {
				lineheader = true;
			} else if (readlinetext[0] == wxT('-')) {
				primaryline = false;
				continue;
			} else if (readlinetext.Contains(wxT("<spontaneous>"))) {
				isspontaneous = true;
				continue;
			} else if (lineheader) {
				delete [] nameandid;
				nameandid = new char[nameLen + 1]; // dynam array for pointer nameandid

				LineParser *line = new LineParser();

				//inicializace struktury
				line->called0 = -1;
				line->called1 = -1;
				line->child = false;
				line->children = -1;
				line->cycle = false;
				line->cycleid = -1;
				line->index = -1;
				line->name = wxT("<undefined>");
				line->nameid = -1;
				line->parents = false;
				line->pline = false;
				line->recursive = false;
				line->self = -1;
				line->time = -1;


				wxRegEx removeSpace;
				if( removeSpace.Compile( wxT("[ ]{2,}") ) ) {
					if( removeSpace.Matches( readlinetext ) ) {
						removeSpace.Replace( &readlinetext, wxT(" ") );
					}
				}
				//

				if (readlinetext.Contains(wxT("."))) isdot = true;
				else isdot = false;

				if (readlinetext.Contains(wxT("cycle"))) iscycle = true;
				else iscycle = false;

				if (readlinetext.Contains(wxT("/"))) islom = true;
				else islom = false;

				if (readlinetext.Contains(wxT("+"))) isplus = true;
				else isplus = false;

				wxString dot = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
				readlinetext.Replace( wxT("."), dot );

				//nahrazeni plus za mezeru
				if(isplus) {
					readlinetext.Replace( wxT("+"), wxT(" ") );
				}

				if ((readlinetext[0] == '[') && (readlinetext[(readlinetext.length()) - 1] == ']')) {
					primaryline = true;
					//
					if(iscycle && isplus) { // [3]     91.71    1.77        0.00    1+5    <cycle 1 as a whole> [3]
						//warning
						sscanf((const char*)readlinetext.mb_str(conv),"[%d] %f %f %f %d %d <cycle %d as a whole> [%d]",&line->index,&line->time,&line->self,&line->children,&line->called0,&line->called1,&line->cycleid,&line->nameid);
						line->name = wxT("whole");
						line->parents = false;
						line->pline = true;
						line->child = false;
						line->cycle = true;
						line->recursive = true;
					} else if(iscycle) { //  [5]     38.86    0.75        0    1      a <cycle 1> [5]
						sscanf((const char*)readlinetext.mb_str(conv),"[%d] %f %f %f %d %[^\n]s",&line->index,&line->time,&line->self,&line->children,&line->called0,nameandid);

						wxString readlinesubtext(nameandid, conv); // musi byt kodovani ?
						line->name = readlinesubtext.BeforeFirst(wxT('<'));
						wxSscanf(readlinesubtext.AfterFirst(wxT('<')).BeforeFirst(wxT('>')),wxT("cycle %d"), &line->cycleid);
						//line->cycleid = readlinesubtext.AfterFirst('<').BeforeFirst('>');
						wxSscanf(readlinesubtext.AfterFirst(wxT('[')).BeforeFirst(wxT(']')),wxT("%d"),&line->nameid);

						line->parents = false;
						line->pline = true;
						line->child = false;
						line->cycle = true;
						line->recursive = false;
					} else if(isplus) { //[4]      3.7    0.00    0.01       1+6       quicksort(int*, int, int) [4]
						sscanf((const char*)readlinetext.mb_str(conv),"[%d] %f %f %f %d %d %[^\n]s",&line->index,&line->time,&line->self,&line->children,&line->called0,&line->called1,nameandid);
						//
						wxString readlinesubtext(nameandid, conv);
						//
						line->name = readlinesubtext.BeforeFirst(wxT('['));
						//
						wxSscanf(readlinesubtext.AfterFirst(wxT('[')).BeforeFirst(wxT(']')),wxT("%d"),&line->nameid);
						line->parents = false;
						line->pline = true;
						line->child = false;
						line->cycle = false;
						line->recursive = true;
					} else if (isspontaneous) { // || !readlinetext.Contains(wxT('('))) // [3]    100.0    0.00    0.03       %d - neni        main [3]
						//special case (probably appears after 'spontaneous'
						sscanf((const char*)readlinetext.mb_str(conv),"[%d] %f %f %f %[^\n]s",&line->index,&line->time,&line->self,&line->children,nameandid);
						//
						wxString readlinesubtext(nameandid, conv); // musi byt kodovani ?
						line->name = readlinesubtext.BeforeFirst(wxT('['));
						//
						wxSscanf(readlinesubtext.AfterFirst(wxT('[')).BeforeFirst(wxT(']')),wxT("%d"),&line->nameid);
						line->parents = false;
						line->pline = true;
						line->child = false;
						line->cycle = false;
						line->recursive = false;

						isspontaneous = false;
					} else { //[2]    100.00    0.16     1.77    1      main [2]
						sscanf((const char*)readlinetext.mb_str(conv),"[%d] %f %f %f %d %[^\n]s",&line->index,&line->time,&line->self,&line->children,&line->called0,nameandid);
						//
						wxString readlinesubtext(nameandid, conv); // musi byt kodovani ?
						line->name = readlinesubtext.BeforeFirst(wxT('['));
						//
						wxSscanf(readlinesubtext.AfterFirst(wxT('[')).BeforeFirst(wxT(']')),wxT("%d"),&line->nameid);
						line->parents = false;
						line->pline = true;
						line->child = false;
						line->cycle = false;
						line->recursive = false;

					}
				} else { //parents and childern
					if(iscycle && !islom) { //3          a <cycle 1> [5]
						// warning
						sscanf((const char*)readlinetext.mb_str(conv),"%d %[^\n]s",&line->called0,nameandid);
						wxString readlinesubtext(nameandid, conv);
						line->name = readlinesubtext.BeforeFirst(wxT('<'));
						wxSscanf(readlinesubtext.AfterFirst(wxT('<')).BeforeFirst(wxT('>')),wxT("cycle %d"), &line->cycleid);
						//line->cycleid = readlinesubtext.AfterFirst('<').BeforeFirst('>');
						wxSscanf(readlinesubtext.AfterFirst(wxT('[')).BeforeFirst(wxT(']')),wxT("%d"),&line->nameid);

						line->cycle = true;
						line->recursive = false;

					} else if(!iscycle && islom) { // 0        0    6/6        c [6]
						sscanf((const char*)readlinetext.mb_str(conv),"%f %f %d/%d %[^\n]s",&line->self,&line->children,&line->called0,&line->called1,nameandid);
						wxString readlinesubtext(nameandid, conv);
						line->name = (wxString)readlinesubtext.BeforeFirst(wxT('['));
						wxSscanf(readlinesubtext.AfterFirst(wxT('[')).BeforeFirst(wxT(']')),wxT("%d"),&line->nameid);

						line->cycle = false;
						line->recursive = false;
						//
					} else if(iscycle && islom) { //1.77        0    1/1        a <cycle 1> [5]
						sscanf((const char*)readlinetext.mb_str(conv),"%f %f %d/%d %[^\n]s",&line->self,&line->children,&line->called0,&line->called1,nameandid);
						wxString readlinesubtext(nameandid, conv); // musi byt kodovani ?
						line->name = readlinesubtext.BeforeFirst(wxT('<'));
						wxSscanf(readlinesubtext.AfterFirst(wxT('<')).BeforeFirst(wxT('>')),wxT("cycle %d"), &line->cycleid);
						//line->cycleid = readlinesubtext.AfterFirst('<').BeforeFirst('>');
						wxSscanf(readlinesubtext.AfterFirst(wxT('[')).BeforeFirst(wxT(']')),wxT("%d"),&line->nameid);

						line->cycle = true;
						line->recursive = false;

					} else if(!iscycle && !islom && !isdot) { //15             faktorial(int) [8]
						sscanf((const char*)readlinetext.mb_str(conv),"%d %[^\n]s",&line->called0,nameandid);
						wxString readlinesubtext(nameandid, conv); // musi byt kodovani ?
						line->name = readlinesubtext.BeforeFirst(wxT('['));
						wxSscanf(readlinesubtext.AfterFirst(wxT('[')).BeforeFirst(wxT(']')),wxT("%d"),&line->nameid);

						line->cycle = false;
						line->recursive = true;

					}
					/*else if(!iscycle && !islom)
					{
						sscanf((const char*)readlinetext.mb_str(conv),"%f %f %d %[^\n]s",&line->self,&line->childern,&line->called0,nameandid);
						//
						wxString readlinesubtext(nameandid, conv); // musi byt kodovani ?
						line->name = readlinesubtext.BeforeFirst('[');
						//
						wxSscanf(readlinesubtext.AfterFirst('[').BeforeFirst(']'),"%d",&line->nameid);

						line->cycle = false;
						line->recursive = false;
						//
					}*/

					if (primaryline) {
						line->parents = false;
						line->pline = false;
						line->child = true;
					} else {
						line->parents = true;
						line->pline = false;
						line->child = false;
					}
				}
				
				lines.Append( line );
				calls[ wxRound(line->time) ] = calls[ wxRound(line->time) ] + 1;
			}
		} else if (lineheader) {
			break;
		}
	}

	delete [] nameandid;
}

int GprofParser::GetSuggestedNodeThreshold()
{
	sortedCalls.Clear();
	
	for( OccurenceMap::iterator it = calls.begin(); it != calls.end(); ++it )
		sortedCalls.Add( it->first );
		
	sortedCalls.Sort(cmpint);
	int totalCount = 0;
	int minCallTime = INT_MAX;
	
	for( size_t i = 0; i < sortedCalls.GetCount() && totalCount < 100; ++i )
	{
		totalCount += calls[ sortedCalls[i] ];
		if( totalCount < 100 && sortedCalls[i] < minCallTime  ) minCallTime = sortedCalls[i];
	}
	
	if( minCallTime < 0 ) minCallTime = 0;
	else if( minCallTime > 100 ) minCallTime = 100;
	
	if( sortedCalls.GetCount() > 1 && totalCount >= 100 ) return minCallTime;
	else
		return -1;
}
