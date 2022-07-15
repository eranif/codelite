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

#include "macrosdlg.h"

#include "globals.h"
#include "ieditor.h"
#include "macromanager.h"

#include <wx/app.h>
#include <wx/clipbrd.h>
#include <wx/crt.h>
#include <wx/dataobj.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

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
    menu.Append(XRCID("copy_macro"), _("Copy macro name"), NULL);
    PopupMenu(&menu);
}

void MacrosDlg::Initialize()
{
    m_listCtrlMacros->InsertColumn(0, _("Macro"));
    m_listCtrlMacros->InsertColumn(1, _("Description"));

    // Only show third column if we can expand the macros
    if(m_project && m_editor) {
        m_listCtrlMacros->InsertColumn(2, _("Value"));
    }

    switch(m_content) {
    case MacrosExternalTools:
        AddMacro("$(CurrentSelection)", _("Expands to the selected text in the active editor"));
        AddMacro("$(CurrentSelectionRange)",
                 _("Expands to the selected text range in bytes from beginning of file, eg. 150:200"));
        AddMacro("$(ProjectOutputFile)", _("Expands to the project binary output file"));
        AddMacro("$(ProjectWorkingDirectory)", _("Expands to the project's build working directory"));
        AddMacro("$(ProjectRunWorkingDirectory)", _("Expands to the project's run working directory"));
    // fall ...
    case MacrosProject:
        AddMacro("$(ProjectPath)", _("Expands to project's path"));
        AddMacro("$(WorkspacePath)", _("Expands to workspace's path"));
        AddMacro("$(WorkspaceConfiguration)", _("Expands to the workspace selected configuration"));
        AddMacro("$(ProjectName)", _("Expands to the current project name as appears in the 'File View'"));
        AddMacro("$(IntermediateDirectory)",
                 _("Expands to the current project intermediate directory path, as set in the project settings"));
        AddMacro("$(ConfigurationName)", _("Expands to the current project selected configuration"));
        AddMacro("$(OutDir)", _("An alias to $(IntermediateDirectory)"));
        AddMacro("$(CurrentFileName)", _("Expands to current file name (without extension and path)"));
        AddMacro("$(CurrentFilePath)", _("Expands to current file path"));
        AddMacro("$(CurrentFileFullPath)", _("Expands to current file full path (path and full name)"));
        AddMacro("$(CurrentFileFullName)", _("Expands to current file full name (name and extension)"));
        AddMacro("$(User)", _("Expands to logged-in user as defined by the OS"));
        AddMacro("$(Date)", _("Expands to current date"));
        AddMacro("$(CodeLitePath)",
                 _("Expands to CodeLite's startup directory on (e.g. on Unix it expands to ~/.codelite/)"));
        AddMacro(
            "$(ProjectFiles)",
            _("A space delimited string containing all of the project files in a relative path to the project file"));
        AddMacro("$(ProjectFilesAbs)",
                 _("A space delimited string containing all of the project files in an absolute path"));
        AddMacro("`expression`", _("backticks: evaluates the expression inside the backticks into a string"));
        AddMacro("$(OutputDirectory)", _("The directory part of $(OutputFile)"));
        AddMacro("$(OutputFile)", _("The output file"));
        AddMacro("$(Program)", _("The program to run"));
        break;

    case MacrosCompiler:
        AddMacro("$(CXX)", _("Expands to the compiler name as set in the Tools tab"));
        AddMacro("$(SourceSwitch)", _("Expands to the source switch (usually, -c)"));
        AddMacro("$(FileFullPath)", _("The file full path (includes path+name+extension)"));
        AddMacro("$(FileFullName)", _("The file full name (includes name+extension)"));
        AddMacro("$(FileName)", _("The file name (name only)"));
        AddMacro("$(FilePath)", _("The file's path with UNIX slashes, including terminating separator"));
        AddMacro("$(CXXFLAGS)", _("Expands to the compiler options as set in the project settings"));
        AddMacro("$(RcCompilerName)", _("Expands to the resource compiler name"));
        AddMacro("$(IntermediateDirectory)",
                 _("Expands to the current project intermediate directory path, as set in the project settings"));
        AddMacro("$(ConfigurationName)", _("Expands to the current project selected configuration"));
        AddMacro("$(OutDir)", _("An alias to $(IntermediateDirectory)"));
        AddMacro("$(LinkerName)", _("Expands to the linker name as set in the Tools tab"));
        AddMacro("$(AR)", _("Expands to the archive tool (e.g. ar) name as set in the Tools tab"));
        AddMacro("$(SharedObjectLinkerName)", _("Expands to the shared object linker name as set in the Tools tab"));
        AddMacro("$(ObjectSuffix)", _("Objects suffix (usually set to .o)"));
        AddMacro("$(ObjectName)", _("The object name (without the suffix)"));
        AddMacro("$(DependSuffix)", _("Objects suffix (usually set to .o.d)"));
        AddMacro("$(PreprocessSuffix)", _("Objects suffix (usually set to .o.i)"));
        AddMacro("$(IncludeSwitch)", _("The compiler include switch"));
        AddMacro("$(LibrarySwitch)", _("The library switch (e.g. -l)"));
        AddMacro("$(OutputSwitch)", _("The output switch (e.g. -o)"));
        AddMacro("$(LibraryPathSwitch)", _("Library switch (e.g. -L)"));
        AddMacro("$(PreprocessorSwitch)", _("Preprocessor switch (e.g. -D)"));
        AddMacro("$(Preprocessors)", _("Expands to all preprocessors set in the project setting where each entry "
                                       "is prefixed with $(PreprocessorSwitch)"));
        AddMacro("$(ArchiveOutputSwitch)", _("Archive switch, usually not needed (VC compiler sets it to /OUT:)"));
        AddMacro("$(PreprocessOnlySwitch)", _("The compiler preprocess-only switch (e.g. -E)"));
        AddMacro("$(LinkOptions)", _("The linker options as set in the project settings"));
        AddMacro("$(IncludePath)", _("All include paths prefixed with $(IncludeSwitch)"));
        AddMacro("$(RcIncludePath)", _("Resource compiler include path as set in the project settings"));
        AddMacro("$(Libs)", _("List of libraries to link with. Each library is prefixed with $(LibrarySwitch)"));
        AddMacro("$(LibPath)",
                 _("List of library paths to link with. Each library is prefixed with $(LibraryPathSwitch)"));
        AddMacro("$(OutputDirectory)", _("The directory part of $(ProjectOutputFile)"));
        AddMacro("$(ProjectOutputFile)", _("The output file"));
        AddMacro("$(OutputFile)", _("The output file, same as $(ProjectOutputFile)"));
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
        wxString value = MacroManager::Instance()->Expand(name, clGetManager(), m_project->GetName());
        SetColumnText(m_listCtrlMacros, row, 2, value);
    }
}

MacrosDlg::~MacrosDlg() {}

void MacrosDlg::OnCopy(wxCommandEvent& e)
{
    if(m_item != wxNOT_FOUND) {
        wxString value = GetColumnText(m_listCtrlMacros, m_item, 0);
        ::CopyToClipboard(value);
    }
    m_item = wxNOT_FOUND;
}
