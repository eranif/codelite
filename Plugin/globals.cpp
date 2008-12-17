//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : globals.cpp
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
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include "editor_config.h"
#include "workspace.h"
#include "project.h"
#include "wx/tokenzr.h"
#include "globals.h"
#include "wx/app.h"
#include "wx/window.h"
#include "wx/listctrl.h"
#include "wx/ffile.h"
#include "procutils.h"
#include <wx/clipbrd.h>

static wxString DoExpandAllVariables(const wxString &expression, Workspace *workspace, const wxString &projectName, const wxString &confToBuild, const wxString &fileName);

// define in LineMarker.cxx
extern void HSL_2_RGB(float h, float s, float l, float *r, float *g, float *b);
extern void RGB_2_HSL(float r, float g, float b, float *h, float *s, float *l);

#ifdef __WXMAC__
#include <mach-o/dyld.h>

//On Mac we determine the base path using system call
//_NSGetExecutablePath(path, &path_len);
static wxString MacGetInstallPath()
{
	char path[257];
	uint32_t path_len = 256;
	_NSGetExecutablePath(path, &path_len);

	//path now contains
	//CodeLite.app/Contents/MacOS/
	wxFileName fname(wxString(path, wxConvUTF8));

	//remove he MacOS part of the exe path
	wxString file_name = fname.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
	wxString rest;
	file_name.EndsWith(wxT("MacOS/"), &rest);
	rest.Append(wxT("SharedSupport/"));

	return rest;
}
#endif


static bool ReadFile8BitData(const char *file_name, wxString &content)
{
	content.Empty();

	FILE *fp = fopen(file_name, "rb");
	if ( fp ) {
		struct stat buff;
		if ( stat(file_name, &buff) == 0 ) {
			size_t size = buff.st_size;
			char *buffer = new char[size+1];
			if ( fread(buffer, sizeof(char), size, fp) == size ) {
				buffer[size] = 0;
				content = wxString::From8BitData(buffer);
			}
			delete [] buffer;
		}
		fclose(fp);
	} // From8BitData
	return content.IsEmpty() == false;
}

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

	if (file.IsOpened()) {

		// first try the user defined encoding
		wxCSConv fontEncConv(EditorConfigST::Get()->GetOptions()->GetFileFontEncoding());
		file.ReadAll(&content, fontEncConv);

		if (content.IsEmpty()) {

			// now try the Utf8
			file.ReadAll(&content, wxConvUTF8);
			if (content.IsEmpty()) {

				// try local 8 bit data
				const wxCharBuffer name = _C(fileName);
				ReadFile8BitData(name.data(), content);

			} // UTF8
		} // user encoding
		return content.IsEmpty() == false;
	} else {
		return false;
	}
	return !content.IsEmpty();
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
wxString ExpandAllVariables(const wxString &expression, Workspace *workspace, const wxString &projectName, const wxString &selConf, const wxString &fileName)
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
				wxString expandedBacktick = DoExpandAllVariables(backtick, workspace, projectName, selConf, fileName);

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

	return DoExpandAllVariables(tmpExp, workspace, projectName, selConf, fileName);
}

wxString DoExpandAllVariables(const wxString &expression, Workspace *workspace, const wxString &projectName, const wxString &confToBuild, const wxString &fileName)
{
	wxString errMsg;
	wxString output(expression);
	if ( workspace ) {
		ProjectPtr proj = workspace->FindProjectByName(projectName, errMsg);
		if (proj) {
			wxString project_name(proj->GetName());

			//make sure that the project name does not contain any spaces
			project_name.Replace(wxT(" "), wxT("_"));

			BuildConfigPtr bldConf = workspace->GetProjBuildConf(proj->GetName(), confToBuild);
			output.Replace(wxT("$(ProjectPath)"), proj->GetFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
			output.Replace(wxT("$(WorkspacePath)"), workspace->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
			output.Replace(wxT("$(ProjectName)"), project_name);
			if(bldConf) {
				output.Replace(wxT("$(IntermediateDirectory)"), bldConf->GetIntermediateDirectory());
				output.Replace(wxT("$(ConfigurationName)"), bldConf->GetName());
				output.Replace(wxT("$(OutDir)"), bldConf->GetIntermediateDirectory());
			}
		}
	}
	if (fileName.IsEmpty() == false) {
		wxFileName fn(fileName);

		output.Replace(wxT("$(CurrentFileName)"), fn.GetName());
		
		wxString fpath(fn.GetPath());
		fpath.Replace(wxT("\\"), wxT("/"));
		output.Replace(wxT("$(CurrentFilePath)"), fpath);
		output.Replace(wxT("$(CurrentFileExt)"), fn.GetExt());
		
		wxString ffullpath(fn.GetFullPath());
		ffullpath.Replace(wxT("\\"), wxT("/"));
		output.Replace(wxT("$(CurrentFileFullPath)"), ffullpath);
	}

	//exapnd common macros
	wxDateTime now = wxDateTime::Now();
	output.Replace(wxT("$(User)"), wxGetUserName());
	output.Replace(wxT("$(Date)"), now.FormatDate());
	
	if(workspace){
		output.Replace(wxT("$(CodeLitePath)"), workspace->GetStartupDir());
	}

	//call the environment & workspace variables expand function
	if ( workspace ) {
		output = workspace->ExpandVariables(output);
	}
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
		Mkdir(from);
		return false;
	}

	if (!wxDir::Exists(to)) {
		Mkdir(to);
	}

	wxDir dir(from);
	wxString filename;
	bool bla = dir.GetFirst(&filename);
	if (bla) {
		do {
			if (wxDirExists(from + filename) ) {
				Mkdir(to + filename);
				CopyDir(from + filename, to + filename);
			} else {
				// change the umask for files only
				wxCopyFile(from + filename, to + filename);
			}
		} while (dir.GetNext(&filename) );
	}
	return true;
}

void Mkdir(const wxString& path)
{
#ifdef __WXMSW__
	wxMkDir(path.GetData());
#else
	wxMkDir(path.ToAscii(), 0777);
#endif
}

bool WriteFileWithBackup(const wxString &file_name, const wxString &content, bool backup)
{
	if (backup) {
		wxString backup_name(file_name);
		backup_name << wxT(".bak");
		if (!wxCopyFile(file_name, backup_name, true)) {
			wxLogMessage(wxString::Format(wxT("Failed to backup file %s, skipping it"), file_name.c_str()));
			return false;
		}
	}

	wxFFile file(file_name, wxT("wb"));
	if (file.IsOpened() == false) {
		// Nothing to be done
		wxString msg = wxString::Format(wxT("Failed to open file %s"), file_name.c_str());
		wxLogMessage( msg );
		return false;
	}

	// write the new content
	wxCSConv fontEncConv(EditorConfigST::Get()->GetOptions()->GetFileFontEncoding());
	file.Write(content, fontEncConv); // JK was without conversion
	file.Close();
	return true;
}

bool CopyToClipboard(const wxString& text)
{
	bool ret(true);
	
#if wxUSE_CLIPBOARD
	if (wxTheClipboard->Open()) {
		wxTheClipboard->UsePrimarySelection(false);
		if (!wxTheClipboard->SetData(new wxTextDataObject(text))) {
			ret = false;
		}
		wxTheClipboard->Close();
	} else {
		ret = false;
	}
#else // wxUSE_CLIPBOARD
	ret = false;
#endif
	return ret;
}

wxColour MakeColourLighter(wxColour color, float level)
{
	float h, s, l, r, g, b;
	RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);
	HSL_2_RGB(h, s, l + std::max(level/20.0, 0.0), &r, &g, &b);
	return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}
