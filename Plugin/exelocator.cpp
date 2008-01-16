#include "exelocator.h"
#include "procutils.h" 

bool ExeLocator::Locate(const wxString &name, wxString &where)
{
	wxString command;
	wxArrayString output;
	command << wxT("which \"") << name << wxT("\"");
	ProcUtils::ExecuteCommand(command, output);
	
	if(output.IsEmpty() == false){
		wxString interstingLine = output.Item(0);
		
		if(interstingLine.Trim().Trim(false).IsEmpty()){
			return false;
		}
		
		if(	!interstingLine.StartsWith(wxT("which: no ")) &&
			!interstingLine.Contains(wxT("command not found")) &&
			!interstingLine.StartsWith(wxT("no "))){
			where = output.Item(0);
			where = where.Trim().Trim(false);
			return true;
		}
	}
	return false;
}
