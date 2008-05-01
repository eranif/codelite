#include <wx/dir.h>
#include "workspace.h"
#include "project.h"
#include "wx/tokenzr.h"
#include "globals.h"
#include "wx/app.h"
#include "wx/window.h"
#include "wx/listctrl.h"
#include "wx/ffile.h"
#include "procutils.h"

static wxString DoExpandAllVariables(const wxString &expression, const wxString &projectName, const wxString &fileName);

void SendCmdEvent(int eventId, void *clientData)
{
	wxCommandEvent e(eventId);
	if (clientData) {
		e.SetClientData(clientData);
	}
	wxTheApp->ProcessEvent(e);
}

void PostCmdEvent(int eventId, void *clientData)
{
	wxCommandEvent e(eventId);
	if (clientData) {
		e.SetClientData(clientData);
	}
	wxTheApp->AddPendingEvent(e);
}

void SetColumnText (wxListCtrl *list, long indx, long column, const wxString &rText, int imgId )
{
	wxListItem list_item;
	list_item.SetId ( indx );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	list_item.SetText ( rText );
	list_item.SetImage( imgId );
	list->SetItem ( list_item );
}

wxString GetColumnText(wxListCtrl *list, long index, long column)
{
	wxListItem list_item;
	list_item.SetId ( index );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	list->GetItem ( list_item );
	return list_item.GetText();
}

bool ReadFileWithConversion(const wxString &fileName, wxString &content)
{
	wxFFile file(fileName, wxT("rb"));

	//first try the Utf8
	file.ReadAll(&content, wxConvUTF8);
	if (content.IsEmpty()) {
		//try local
		file.Seek(0);

		file.ReadAll(&content, wxConvLocal);
		if (content.IsEmpty()) {
			file.Seek(0);
			file.ReadAll(& content, wxConvLibc);
		}
	}
	return content.IsEmpty() == false;
}

bool RemoveDirectory(const wxString &path)
{
	wxString cmd;
	if (wxGetOsVersion() & wxOS_WINDOWS) {
		//any of the windows variants
		cmd << wxT("rmdir /S /Q ") << wxT("\"") << path << wxT("\"");
	} else {
		cmd << wxT("\rm -fr ") << wxT("\"") << path << wxT("\"");
	}
	wxArrayString output;
	return wxShell(cmd);
}

bool IsValidCppIndetifier(const wxString &id)
{
	if (id.IsEmpty()) {
		return false;
	}
	//first char can be only _A-Za-z
	wxString first( id.Mid(0, 1) );
	if (first.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")) != wxString::npos ) {
		return false;
	}
	//make sure that rest of the id contains only a-zA-Z0-9_
	if (id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
		return false;
	}
	return true;
}

long AppendListCtrlRow(wxListCtrl *list)
{
	long item;
	wxListItem info;
	// Set the item display name
	info.SetColumn(0);
	item = list->InsertItem(info);

#ifdef __WXMAC__
	item = list->GetItemCount()-1;
#endif
	return item;
}

bool IsValidCppFile(const wxString &id)
{
	if (id.IsEmpty()) {
		return false;
	}

	//make sure that rest of the id contains only a-zA-Z0-9_
	if (id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
		return false;
	}
	return true;
}
// This functions accepts expression and expand all variables in it
wxString ExpandAllVariables(const wxString &expression, const wxString &projectName, const wxString &fileName)
{
	//add support for backticks commands
	wxString tmpExp;
	wxString noBackticksExpression;
	for (size_t i=0; i< expression.Length(); i++) {
		if (expression.GetChar(i) == wxT('`')) {
			//found a backtick, loop over until we found the closing backtick
			wxString backtick;
			bool found(false);
			i++;
			for (; i< expression.Length(); i++) {
				if (expression.GetChar(i) == wxT('`')) {
					found = true;
					i++;
					break;
				}
				backtick << expression.GetChar(i);
			}

			if (!found) {
				//dont replace anything
				wxLogMessage(wxT("Syntax error in expression: ") + expression + wxT(": expecting '`'"));
				return expression;
			} else {
				//expand the backtick statement
				wxString expandedBacktick = DoExpandAllVariables(backtick, projectName, fileName);

				//execute the backtick
				wxArrayString output;
				ProcUtils::SafeExecuteCommand(expandedBacktick, output);

				//concatenate the array into sAssign To:pace delimited string
				backtick.Clear();
				for (size_t xx=0; xx < output.GetCount(); xx++) {
					backtick << output.Item(xx).Trim().Trim(false) << wxT(" ");
				}

				//and finally concatente the result of the backtick command back to the expression
				tmpExp << backtick;
			}
		} else {
			tmpExp << expression.GetChar(i);
		}
	}

	return DoExpandAllVariables(tmpExp, projectName, fileName);
}

wxString DoExpandAllVariables(const wxString &expression, const wxString &projectName, const wxString &fileName)
{
	wxString errMsg;
	wxString output(expression);
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
	if (proj) {
		wxString project_name(proj->GetName());

		//make sure that the project name does not contain any spaces
		project_name.Replace(wxT(" "), wxT("_"));

		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjSelBuildConf(proj->GetName());
		output.Replace(wxT("$(ProjectPath)"), proj->GetFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
		output.Replace(wxT("$(WorkspacePath)"), WorkspaceST::Get()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
		output.Replace(wxT("$(ProjectName)"), project_name);
		output.Replace(wxT("$(IntermediateDirectory)"), bldConf->GetIntermediateDirectory());
		output.Replace(wxT("$(ConfigurationName)"), bldConf->GetName());
		output.Replace(wxT("$(OutDir)"), bldConf->GetIntermediateDirectory());

	}

	if (fileName.IsEmpty() == false) {
		wxFileName fn(fileName);

		output.Replace(wxT("$(CurrentFileName)"), fn.GetName());
		output.Replace(wxT("$(CurrentFilePath)"), fn.GetPath());
		output.Replace(wxT("$(CurrentFileExt)"), fn.GetExt());
		output.Replace(wxT("$(CurrentFileFullPath)"), fn.GetFullPath());
	}

	//exapnd common macros
	wxDateTime now = wxDateTime::Now();
	output.Replace(wxT("$(User)"), wxGetUserName());
	output.Replace(wxT("$(Date)"), now.FormatDate());

	//call the environment & workspace variables expand function
	output = WorkspaceST::Get()->ExpandVariables(output);
	return output;
}

bool WriteFileUTF8(const wxString& fileName, const wxString& content)
{
	wxFFile file(fileName, wxT("w+b"));

	//first try the Utf8
	return file.Write(content, wxConvUTF8) == content.Length();
}
bool CopyDir(const wxString& src, const wxString& target)
{
	wxString SLASH = wxFileName::GetPathSeparator();

	wxString from(src);
	wxString to(target);

	// append a slash if there is not one (for easier parsing)
	// because who knows what people will pass to the function.
	if (to.EndsWith(SLASH) == false) {
		to << SLASH;
	}

	// for both dirs
	if (from.EndsWith(SLASH) == false) {
		from << SLASH;
	}

	// first make sure that the source dir exists
	if (!wxDir::Exists(from)) {
#ifdef __WXMSW__
		wxMkDir(from.GetData());
#else
		wxMkDir(from.ToAscii(), 0777);
#endif
		return false;
	}

	if (!wxDir::Exists(to)) {
#ifdef __WXMSW__
		wxMkDir(to.GetData());
#else
		wxMkDir(to.ToAscii(), 0777);
#endif
	}

	wxDir dir(from);
	wxString filename;
	bool bla = dir.GetFirst(&filename);

	if (bla) {
		do {

			if (wxDirExists(from + filename) ) {
#ifdef __WXMSW__
				wxMkDir(wxString(to + filename).GetData());
#else
				wxMkDir(wxString(to + filename).ToAscii(), 0777);
#endif
				CopyDir(from + filename, to + filename);
			} else {
				wxCopyFile(from + filename, to + filename);
			}
		} while (dir.GetNext(&filename) );
	}
	return true;
}
