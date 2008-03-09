#include "precompiled_header.h"
#include "makefile_lexer.h"
#include "VariableLexer.h"
#include "MakefileParser.h"
#include "Target.h"
#include "TargetLexer.h"
#include "Node.h"
#include <wx/file.h>

bool g_verbose = false;
bool g_debugOnly = false;

void LogMessage(const wxString& msg)
{
	wxCharBuffer buf = msg.ToAscii();
	printf("%s", buf.data());
}

MakefileNode* ImportFromMakefile(const wxString &path)
{
	LogMessage(wxT("Path: ") + path + wxT("\n"));

	Tokens tokens;
	tokens["RM"] = "rm";
	tokens["MAKE"] = "make";

	VariableLexer expander(path.data(), tokens);
	wxArrayString expanded = expander.getResult();
	std::map<wxString, wxString> restokens = expander.getTokens();
	
	if(g_verbose)
	{
		LogMessage(wxT("Expanded makefile is:\n"));
		LogMessage(wxT("\n\n====================\n\n"));
		for(wxArrayString::iterator it = expanded.begin(); it != expanded.end(); it++)
		{
			wxString line = *it;
			LogMessage(line + wxT("\n"));			
		}
		LogMessage(wxT("\n\n====================\n\n"));
		LogMessage(wxT("End of expanded makefile.\n"));
		
		for(std::map<wxString, wxString>::iterator it = restokens.begin(); it != restokens.end(); it++)
		{
			std::pair<wxString, wxString> token = *it;
			LogMessage(token.first + wxT("[=]") + token.second + wxT("\n"));
		}
	}

	MakefileParser parser(expanded);
	TypedStrings parsed = parser.getResult();

	TargetLexer lexer(parsed);
	Targets lexed = lexer.getResult();
	
	MakefileNode* result = NULL;
	std::map<wxString, Target*> targetByName;

	for(Targets::iterator it = lexed.begin(); it != lexed.end(); it++)
	{
		Target* target = *it;
		wxString name = target->getName();
		if(name.Contains(wxT("%")))
		{
			// do special stuff
			continue;
		}
		targetByName[name] = target;
		// LogMessage(wxT("found: ") + name + wxT("\n"));
	}
	
	wxString defaultGoal = restokens[wxT(".DEFAULT_GOAL")];
	Target* root = targetByName[defaultGoal];
	if(!root)
	{
		LogMessage(wxT("Could not find target for .DEFAULT_GOAL='") + defaultGoal + wxT("'.\n"));
		return NULL;
	}
	
	result = new MakefileNode(NULL, root);
	result->addNodes(targetByName);
	
	/*
	{		
		Target* target = *it;
		
		if(!g_debugOnly)
			LogMessage(target->getName() + wxT("\n"));
		
		if(result == NULL)
			result = new MakefileNode(NULL, target);
		else
			result->addNode(target);
	}
	*/
	
	return result;
}

int main(int argv, char* argc[])
{	
	wxString path;
	switch(argv)
	{
		case 3:
		{
			if(!strcmp(argc[2], "-v"))
				g_verbose = true;
				
			if(!strcmp(argc[2], "-vd"))
			{
				g_verbose = true;
				g_debugOnly = true;
			}
			// fallthrough
		}
		
		case 2:
			path = _U(argc[1]);
			break;
			
		default:
			path = wxT("input");
	}
	
	if(g_verbose)
		printf("Verbose enabled.\n");
		
	printf("Fetching result...\n");
		
	// TypedStrings result = ImportFromMakefile(path);
	MakefileNode* result = ImportFromMakefile(path);
	
	printf("Done.\n");
	printf("Result:\n");
	
	if(!g_debugOnly && result)
		LogMessage(result->toString());
	
	/*
	for(Targets::iterator it = result.begin(); it != result.end(); it++)
	{
		Target token = *it;
		wxString line;
		line = wxT("Target ");
		line << token.getName();
		line << wxT(":\n"); 
		LogMessage(line);
		
		LogMessage(wxT("Actions: \n"));
		
		wxArrayString actions = token.getActions();
		for(wxArrayString::iterator act = actions.begin(); act != actions.end(); act++)
			LogMessage(*act + wxT("\n"));
			
		LogMessage(wxT("Deps: \n"));
		
		wxArrayString deps = token.getDeps();
		for(wxArrayString::iterator dep = deps.begin(); dep != deps.end(); dep++)
			LogMessage(*dep + wxT("\n"));
		
		printf("\n");
	}
	*/
	
	/*
	for(TypedStrings::iterator it = result.begin(); it != result.end(); it++)
	{
		TypedString token = *it;
		wxString msg;
		msg << LINETYPES::toString(token.type);
		msg << wxT(": '");
		msg << token.line;
		msg << wxT("'.\n");
		LogMessage(msg);
	}
	*/
	
	printf("Done.\n");
	printf("Exiting.\n");
	
	return 0;
}
