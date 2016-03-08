//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : macrosdlg.cpp
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

#include <wx/dataobj.h>
#include <wx/app.h>
#include <wx/clipbrd.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/crt.h>
#include "macrosdlg.h"
#include "globals.h"
#include "ieditor.h"

BEGIN_EVENT_TABLE(MacrosDlg, MacrosBaseDlg)
EVT_MENU(XRCID("copy_macro"), MacrosDlg::OnCopy)
END_EVENT_TABLE()

MacrosDlg::MacrosDlg(wxWindow* parent, int content, ProjectPtr project, IEditor* editor)
    : MacrosBaseDlg(parent)
    , m_item(wxNOT_FOUND)
    , m_content(content)
    , m_project(project)
    , m_editor(editor)
{
    Initialize();
    CenterOnParent();
}

void MacrosDlg::OnItemRightClick(wxListEvent& event)
{
    m_item = event.m_itemIndex;
    wxMenu menu;
    menu.Append(XRCID("copy_macro"), wxT("Copy macro name"), NULL);
    PopupMenu(&menu);
}

void MacrosDlg::Initialize()
{
    m_listCtrlMacros->InsertColumn(0, _("Macro"));
    m_listCtrlMacros->InsertColumn(1, _("Description"));

    // Only show third column if we can expand the macros
    if(m_project && m_editor) {
        m_listCtrlMacros->InsertColumn(2, wxT("Value"));
    }

    switch(m_content) {
    case MacrosExternalTools:
        AddMacro(wxT("$(CurrentSelection)"), _("Expands to the selected text in the active editor"));
        AddMacro(wxT("$(CurrentSelectionRange)"),
                 _("Expands to the selected text range in bytes from beginning of file, eg. 150:200"));
        AddMacro(wxT("$(ProjectOutputFile)"), _("Expands to the project binary output file"));
        AddMacro(wxT("$(ProjectWorkingDirectory)"), _("Expands to the project's build working directory"));
        AddMacro(wxT("$(ProjectRunWorkingDirectory)"), _("Expands to the project's run working directory"));
    // fall ...
    case MacrosProject:
        AddMacro(wxT("$(ProjectPath)"), _("Expands to project's path"));
        AddMacro(wxT("$(WorkspacePath)"), _("Expands to workspace's path"));
        AddMacro(wxT("$(ProjectName)"), _("Expands to the current project name as appears in the 'File View'"));
        AddMacro(wxT("$(IntermediateDirectory)"),
                 _("Expands to the current project intermediate directory path, as set in the project settings"));
        AddMacro(wxT("$(ConfigurationName)"), _("Expands to the current project selected configuration"));
        AddMacro(wxT("$(OutDir)"), _("An alias to $(IntermediateDirectory)"));
        AddMacro(wxT("$(CurrentFileName)"), _("Expands to current file name (without extension and path)"));
        AddMacro(wxT("$(CurrentFilePath)"), _("Expands to current file path"));
        AddMacro(wxT("$(CurrentFileFullPath)"), _("Expands to current file full path (path and full name)"));
        AddMacro(wxT("$(CurrentFileFullName)"), _("Expands to current file full name (name and extension)"));
        AddMacro(wxT("$(User)"), _("Expands to logged-in user as defined by the OS"));
        AddMacro(wxT("$(Date)"), _("Expands to current date"));
        AddMacro(wxT("$(CodeLitePath)"),
                 _("Expands to CodeLite's startup directory on (e.g. on Unix it expands to ~/.codelite/"));
        AddMacro(
            wxT("$(ProjectFiles)"),
            _("A space delimited string containing all of the project files in a relative path to the project file"));
        AddMacro(wxT("$(ProjectFilesAbs)"),
                 _("A space delimited string containing all of the project files in an absolute path"));
        AddMacro(wxT("`expression`"), _("backticks: evaluates the expression inside the backticks into a string"));
        AddMacro(wxT("$(OutputFile)"), _("The output file"));
        break;

    case MacrosCompiler:
        AddMacro(wxT("$(CXX)"), _("Expands to the compiler name as set in the Tools tab"));
        AddMacro(wxT("$(SourceSwitch)"), _("Expands to the source switch (usually, -c)"));
        AddMacro(wxT("$(FileFullPath)"), _("The file full path (includes path+name+extension)"));
        AddMacro(wxT("$(FileFullName)"), _("The file full name (includes name+extension)"));
        AddMacro(wxT("$(FileName)"), _("The file name (name only)"));
        AddMacro(wxT("$(FilePath)"), _("The file's path with UNIX slashes, including terminating separator"));
        AddMacro(wxT("$(CXXFLAGS)"), _("Expands to the compiler options as set in the project settings"));
        AddMacro(wxT("$(RcCompilerName)"), _("Expands to the resource compiler name"));
        AddMacro(wxT("$(IntermediateDirectory)"),
                 _("Expands to the current project intermediate directory path, as set in the project settings"));
        AddMacro(wxT("$(ConfigurationName)"), _("Expands to the current project selected configuration"));
        AddMacro(wxT("$(OutDir)"), _("An alias to $(IntermediateDirectory)"));
        AddMacro(wxT("$(LinkerName)"), _("Expands to the linker name as set in the Tools tab"));
        AddMacro(wxT("$(AR)"), _("Expands to the archive tool (e.g. ar) name as set in the Tools tab"));
        AddMacro(wxT("$(SharedObjectLinkerName)"),
                 _("Expands to the shared object linker name as set in the Tools tab"));
        AddMacro(wxT("$(ObjectSuffix)"), _("Objects suffix (usually set to .o)"));
        AddMacro(wxT("$(ObjectName)"), _("The object name (without the suffix)"));
        AddMacro(wxT("$(DependSuffix)"), _("Objects suffix (usually set to .o.d)"));
        AddMacro(wxT("$(PreprocessSuffix)"), _("Objects suffix (usually set to .o.i)"));
        AddMacro(wxT("$(IncludeSwitch)"), _("The compiler include switch"));
        AddMacro(wxT("$(LibrarySwitch)"), _("The library switch (e.g. -l)"));
        AddMacro(wxT("$(OutputSwitch)"), _("The output switch (e.g. -o)"));
        AddMacro(wxT("$(LibraryPathSwitch)"), _("Library switch (e.g. -L)"));
        AddMacro(wxT("$(PreprocessorSwitch)"), _("Preprocessor switch (e.g. -D)"));
        AddMacro(wxT("$(Preprocessors)"),
                 _("Expands to all preprocessors set in the project setting where each entry "
                   "is prefixed with $(PreprocessorSwitch)"));
        AddMacro(wxT("$(ArchiveOutputSwitch)"), _("Archive switch, usually not needed (VC compiler sets it to /OUT:"));
        AddMacro(wxT("$(PreprocessOnlySwitch)"), _("The compiler preprocess-only switch (e.g. -E)"));
        AddMacro(wxT("$(LinkOptions)"), _("The linker options as set in the project settings"));
        AddMacro(wxT("$(IncludePath)"), _("All include paths prefixed with $(IncludeSwitch)"));
        AddMacro(wxT("$(RcIncludePath)"), _("Resource compiler include path as set in the project settings"));
        AddMacro(wxT("$(Libs)"), _("List of libraries to link with. Each library is prefixed with $(LibrarySwitch)"));
        AddMacro(wxT("$(LibPath)"),
                 _("List of library paths to link with. Each library is prefixed with $(LibraryPathSwitch)"));
        AddMacro(wxT("$(ProjectOutputFile)"), _("The output file"));
        AddMacro(wxT("$(OutputFile)"), _("The output file, same as $(ProjectOutputFile)"));
        break;
    }

    // resize columns
    m_listCtrlMacros->SetColumnWidth(0, wxLIST_AUTOSIZE);
    m_listCtrlMacros->SetColumnWidth(1, wxLIST_AUTOSIZE);

    // Only resize third column if we can and may expand the macros
    if(m_project && m_editor) {
        // Do not autosize this column as it may grow very, very wide
        m_listCtrlMacros->SetColumnWidth(2, 220);
    }
}

void MacrosDlg::AddMacro(const wxString& name, const wxString& desc)
{
    long row = AppendListCtrlRow(m_listCtrlMacros);
    SetColumnText(m_listCtrlMacros, row, 0, name);
    SetColumnText(m_listCtrlMacros, row, 1, desc);

    // Only fill third column if we can and may expand the macros
    if(m_project && m_editor && name != "$(ProjectFiles)" && name != "$(ProjectFilesAbs)") {
        wxString value = ExpandVariables(name, m_project, m_editor);
        SetColumnText(m_listCtrlMacros, row, 2, value);

    } else {
        // No third column here... don't fill it or we get an assertion
    }
}

MacrosDlg::~MacrosDlg() {}

void MacrosDlg::OnCopy(wxCommandEvent& e)
{
    if(m_item != wxNOT_FOUND) {
        wxString value = GetColumnText(m_listCtrlMacros, m_item, 0);
#if wxUSE_CLIPBOARD
        if(wxTheClipboard->Open()) {
            wxTheClipboard->UsePrimarySelection(false);
            if(!wxTheClipboard->SetData(new wxTextDataObject(value))) {
                // wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
            }
            wxTheClipboard->Close();
        } else {
            wxPrintf(wxT("Failed to open the clipboard"));
        }
#endif
    }
    m_item = wxNOT_FOUND;
}
