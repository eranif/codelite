#include "top_level_win_wrapper.h"

#include "allocator_mgr.h"
#include "choice_property.h"
#include "virtual_folder_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_notebook_code_helper.h"
#include "wxgui_helpers.h"

#include <wx/arrstr.h>
#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include <wx/richmsgdlg.h>
#include <wx/tokenzr.h>
#include <wx/toplevel.h>

TopLevelWinWrapper::TopLevelWinWrapper(int type)
    : wxcWidget(type)
    , m_auiDropDownMenuHelperRegistered(false)
{
    PREPEND_STYLE(wxSTAY_ON_TOP, false);
    PREPEND_STYLE(wxCLOSE_BOX, false);
    PREPEND_STYLE(wxSYSTEM_MENU, false);
    PREPEND_STYLE(wxMINIMIZE_BOX, false);
    PREPEND_STYLE(wxMAXIMIZE_BOX, false);
    PREPEND_STYLE(wxRESIZE_BORDER, false);
    PREPEND_STYLE(wxCAPTION, false);

    if(type == ID_WXFRAME || type == ID_WXDIALOG) {
        // a real top level windows
        // add support for wxTopLevelWindow events
        RegisterEvent("wxEVT_MAXIMIZE", "wxMaximizeEvent", _("Process a wxEVT_MAXIMIZE event"));
        RegisterEvent("wxEVT_MOVE", "wxMoveEvent",
                      _("Process a wxEVT_MOVE event, which is generated when a window is moved"));
        RegisterEvent("wxEVT_MOVE_START", "wxMoveEvent",
                      _("Process a wxEVT_MOVE_START event, which is generated when the "
                        "user starts to move or size a window. Windows only"));
        RegisterEvent("wxEVT_MOVE_END", "wxMoveEvent",
                      _("Process a wxEVT_MOVE_END event, which is generated when "
                        "the user stops moving or sizing a window. Windows only"));
        RegisterEvent("wxEVT_SHOW", "wxShowEvent", _("Process a wxEVT_SHOW event"));
    }

    // Default size for top level windows
    DoSetPropertyStringValue(PROP_SIZE, "500,300");
    if(IsWxTopLevelWindow()) {
        // Add a persistency object support
        AddBool(PROP_PERSISTENT,
                _("When enabled, the generated code will add support for wxPersistenceManager (i.e. the Window will "
                  "remember its size, position and any child wxBookCtrlBase control will remember its selection)"),
                true);
    }

    wxArrayString arr;
    arr.Add("");
    arr.Add("wxBOTH");
    arr.Add("wxVERTICAL");
    arr.Add("wxHORIZONTAL");

    AddProperty(new StringProperty(PROP_TITLE, "", _("The title, if any")));
    AddProperty(
        new VirtualFolderProperty(PROP_VIRTUAL_FOLDER, "", _("CodeLite's virtual folder for the generated files")));
    AddProperty(new ChoiceProperty(PROP_CENTRE_ON_SCREEN, arr, 1,
                                   _("Centre on parent. This may be in both dimensions (the default); only "
                                     "vertically or horizontally; or not at all.")));

    AddProperty(new CategoryProperty(_("Inherited C++ Class Properties")));
    AddProperty(new StringProperty(PROP_INHERITED_CLASS, "",
                                   _("Inherited class name\nFill this field to generate a class that inherits from the "
                                     "base class,\nwhere you should place all your application logic.\ne.g. for a "
                                     "generated class 'FooDialogBase', you might enter 'FooDialog' here.")));
    AddProperty(new StringProperty(PROP_FILE, "",
                                   _("The name for the inherited class's files (without any file "
                                     "extension).\nwxCrafter will generate a $(FILE).cpp and $(FILE).hpp\ne.g. "
                                     "for an inherited class 'FooDialog', you might enter 'foodialog' here.")));
    AddProperty(new StringProperty(PROP_CLASS_DECORATOR, "",
                                   _("MSW Only\nC++ macro decorator - allows exporting this class from a DLL")));

    if(m_properties.Contains(PROP_NAME)) {
        m_properties.Item(PROP_NAME)->SetTooltip(_("The generated C++ class name"));
    }

    DelProperty(PROP_WINDOW_ID);
}

TopLevelWinWrapper::~TopLevelWinWrapper() {}

wxString TopLevelWinWrapper::FormatCode(const wxString& chunk) const
{
    // Format the code a bit
    wxString formattedCode;
    wxArrayString codeArr = ::wxStringTokenize(chunk, "\n\r", wxTOKEN_RET_EMPTY_ALL);
    for(size_t i = 0; i < codeArr.GetCount(); i++) {
        formattedCode << "    " << codeArr.Item(i) << "\n";
    }

    formattedCode.Replace("|@@|", "");

    // Remove any double empty lines
    while(formattedCode.Replace("    \n    \n", "    \n")) {}
    return formattedCode;
}

void TopLevelWinWrapper::GenerateCode(const wxcProjectMetadata& project, bool promptUser, bool baseOnly,
                                      wxString& baseCpp, wxString& baseHeader, wxArrayString& headers,
                                      wxStringMap_t& additionalFiles)
{
    wxString size = PropertyString(PROP_SIZE);
    wxString filename = PropertyFile(PROP_FILE);

    if(promptUser && (GetType() != ID_WXIMAGELIST)) { // By design we dont provide a subclass for wxImageList
        if(filename.IsEmpty() && !wxcSettings::Get().HasFlag(wxcSettings::DONT_PROMPT_ABOUT_MISSING_SUBCLASS)) {
            wxString message;
            message << _("You did not set the 'Inherited C++ Class Properties -> File name' property for the top level "
                         "window: '")
                    << GetName() << "'\n"
                    << _("This means that only base class code will be generated\nTo fix this, select the toplevel "
                         "entry from the tree-view and provide an Inherited class name and file name");

            wxRichMessageDialog dlg(NULL, message, "wxCrafter", wxOK | wxOK_DEFAULT | wxCENTER | wxICON_WARNING);
            dlg.SetOKLabel(_("OK, continue with code generation"));
            dlg.ShowCheckBox(_("Don't show this message again"));

            if(dlg.ShowModal() == wxID_CANCEL) {
                return;
            }
            if(dlg.IsCheckBoxChecked()) {
                wxcSettings::Get().EnableFlag(wxcSettings::DONT_PROMPT_ABOUT_MISSING_SUBCLASS, true);
            }
        }
    }

    /// Clear all window IDs related to this top level window
    m_auiDropDownMenuHelperRegistered = false;
    wxcCodeGeneratorHelper::Get().ClearWindowIds();
    wxcCodeGeneratorHelper::Get().ClearIcons(); // Icons should be cleared between top-level windows

    wxcNotebookCodeHelper::Get().Clear();
    wxString ctorBody, dtorCode, membersChunk, eventFunctions, eventConnectCode, eventDisconnectCode,
        extraFunctionsCodeImpl, extraFunctionsCodeDecl;
    headers.Add("#include <wx/settings.h>");
    headers.Add("#include <wx/xrc/xmlres.h>");
    headers.Add("#include <wx/xrc/xh_bmp.h>");

    DoTraverseAndGenCode(headers, ctorBody, membersChunk, eventFunctions, eventConnectCode, additionalFiles, dtorCode,
                         extraFunctionsCodeImpl, extraFunctionsCodeDecl);

    // Format the code a bit
    ctorBody = FormatCode(ctorBody);

    // Window colors
    if(IsWindow()) {
        wxString bgcol = wxCrafter::ColourToCpp(PropertyString(PROP_BG));
        wxString fgcol = wxCrafter::ColourToCpp(PropertyString(PROP_FG));

        // Add colors and fonts here
        if(bgcol.IsEmpty() == false) {
            ctorBody << "    SetBackgroundColour(" << bgcol << ");\n";
        }

        if(fgcol.IsEmpty() == false) {
            ctorBody << "    SetForegroundColour(" << fgcol << ");\n";
        }
    }

    ctorBody << FormatCode(wxcNotebookCodeHelper::Get().Code());
    wxcNotebookCodeHelper::Get().Clear();

    if(IsWindow()) {
        // Set the window name
        ctorBody << "    SetName(" << wxCrafter::WXT(GetName()) << ");\n";
        wxSize sz = wxCrafter::DecodeSize(PropertyString(PROP_MINSIZE));
        if(sz != wxDefaultSize) {
            ctorBody << "    SetMinClientSize(wxSize(" << wxCrafter::EncodeSize(sz) << "));\n";
        }

        if(GetType() != ID_WXAUITOOLBARTOPLEVEL) {
            ctorBody << "    SetSize(" << wxCrafter::GetSizeAsDlgUnits(GetSize(), "this") << ");\n";
            ctorBody << "    if (GetSizer()) {\n";
            ctorBody << "         GetSizer()->Fit(this);\n";
            ctorBody << "    }\n";

            if(IsWxTopLevelWindow()) {
                ctorBody << "    if(GetParent()) {\n";
                ctorBody << "        CentreOnParent(" << PropertyString(PROP_CENTRE_ON_SCREEN) << ");\n";
                ctorBody << "    } else {\n";
                ctorBody << "        CentreOnScreen(" << PropertyString(PROP_CENTRE_ON_SCREEN) << ");\n";
                ctorBody << "    }\n";
            }

            // Add support for wxPersistenceManager object
            if(IsWxTopLevelWindow() && IsPropertyChecked(PROP_PERSISTENT)) {
                ctorBody << "    if(!wxPersistenceManager::Get().Find(this)) {\n";
                ctorBody << "        wxPersistenceManager::Get().RegisterAndRestore(this);\n";
                ctorBody << "    } else {\n";
                ctorBody << "        wxPersistenceManager::Get().Restore(this);\n";
                ctorBody << "    }\n";
            }
        }
    }

    // Format the function code
    eventFunctions = FormatCode(eventFunctions);
    eventConnectCode = FormatCode(eventConnectCode);

    // use Unbind syntax
    eventDisconnectCode = eventConnectCode;
    eventDisconnectCode.Replace("->Bind(", "->Unbind(");

    wxString baseClassName = CreateBaseclassName();

    wxString cppDecorator = PropertyString(PROP_CLASS_DECORATOR);
    cppDecorator.Trim().Trim(false);

    wxString superclass = GetRealClassName();
    wxString derivedSuperclass = baseClassName;

    if(baseClassName.IsEmpty()) { // meaning that it was empty until BASE_CLASS_SUFFIX was appended
        wxString msg;
        msg << _("Can not generate code.\nMake sure that all toplevel windows have a valid C++ class name");
        wxMessageBox(msg, "wxCrafter", wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    if(filename.IsEmpty()) {
        baseOnly = true;
    }

    wxFileName headerFile, sourceFile;
    wxFileName baseFile(wxcProjectMetadata::Get().GetGeneratedFilesDir(),
                        wxcProjectMetadata::Get().GetOutputFileName());

    // If the files are relative make them abs
    if(baseFile.IsRelative()) {
        baseFile.MakeAbsolute(project.GetProjectPath());
    }

    wxString dbg = baseFile.GetFullPath();

    headerFile = baseFile;
    sourceFile = baseFile;

    headerFile.SetExt(wxcProjectMetadata::Get().GetHeaderFileExt());
    sourceFile.SetExt("cpp");

    dtorCode.Prepend(eventDisconnectCode);

    // Write the C++ file
    baseCpp << "\n"
            << BaseCtorImplPrefix() << "{\n"
            << wxcCodeGeneratorHelper::Get().GenerateInitCode(this) << ctorBody;

    if(eventConnectCode.IsEmpty() == false) {
        baseCpp << "    // Connect events\n" << eventConnectCode;
    }

    // Now Connect() any auitoolbar dropdown menu; it must be *after* the normal Connect()s, otherwise it won't be
    // called if the user-code forgets to event.Skip()
    if(IsAuiToolBarDropDownHelpersRegistered()) {
        baseCpp << "    this->Connect("
                << "wxID_ANY, "
                << "wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(" << GetName()
                << "::" << DEFAULT_AUI_DROPDOWN_FUNCTION << "), NULL, this);\n";

        // We need to disconnect it too
        dtorCode << "    this->Disconnect("
                 << "wxID_ANY, "
                 << "wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN, wxAuiToolBarEventHandler(" << GetName()
                 << "::" << DEFAULT_AUI_DROPDOWN_FUNCTION << "), NULL, this);\n";
    }

    baseCpp << "}\n\n"
            << baseClassName << "::~" << baseClassName << "()\n"
            << "{\n"
            << dtorCode << "}\n";

    // Write any extra (i.e. not ctor/dtor) functions
    if(!extraFunctionsCodeImpl.empty()) {
        baseCpp << extraFunctionsCodeImpl; // Don't use FormatCode() here; it'd indent the signature etc too
    }

    // prepare the enum block for the
    // Write the header file

    baseHeader << "\n"
               << "class " << (cppDecorator.IsEmpty() ? "" : cppDecorator + " ") << baseClassName << " : public "
               << superclass << "\n"
               << "{\n"
               << wxcCodeGeneratorHelper::Get().GenerateWinIdEnum() << "protected:\n"
               << membersChunk << "\n"
               << "protected:\n"
               << eventFunctions << "\n"
               << "public:\n"
               << extraFunctionsCodeDecl << BaseCtorDecl() << "    virtual ~" << baseClassName << "();\n"
               << "};\n\n";

    if(baseOnly) {
        return;
    }

    wxString inheritedClass = PropertyString(PROP_INHERITED_CLASS);
    inheritedClass.Trim().Trim(false);

    if(inheritedClass.IsEmpty()) {
        return;
    }

    if(inheritedClass == baseClassName) {
        ::wxMessageBox(_("Base class and inherited class have the same name"), "wxCrafter",
                       wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    // Create the derived classes files
    /////////////////////////////////////////////////////////////////////////////////////////

    wxFileName derivedClassFileCPP(filename);
    wxFileName derivedClassFileHPP(filename);

    derivedClassFileCPP.SetExt("cpp");
    derivedClassFileHPP.SetExt(wxcProjectMetadata::Get().GetHeaderFileExt());

    // Fix the paths if needed
    // i.e. if the derived classes are relative, make them use the same
    // path as the base classes file
    if(derivedClassFileCPP.IsRelative()) {
        derivedClassFileCPP.SetPath(headerFile.GetPath());
    }

    if(derivedClassFileHPP.IsRelative()) {
        derivedClassFileHPP.SetPath(headerFile.GetPath());
    }

    wxString dCpp, dH, dBlockGuard;
    dBlockGuard << inheritedClass;
    dBlockGuard << "_" << derivedClassFileHPP.GetExt();
    dBlockGuard.MakeUpper();

    // Prepare the Header file content
    dH << "#ifndef " << dBlockGuard << "\n"
       << "#define " << dBlockGuard << "\n"
       << "#include \"" << headerFile.GetFullName() << "\"\n"
       << "\n"
       << "class " << (cppDecorator.IsEmpty() ? "" : cppDecorator + " ") << inheritedClass << " : public "
       << derivedSuperclass << "\n"
       << "{\n"
       << "public:\n"
       << "    " << inheritedClass << GetDerivedClassCtorSignature() << ";\n"
       << "    virtual ~" << inheritedClass << "();\n"
       << "};\n"
       << "#endif // " << dBlockGuard << "\n";

    // Prepare the CPP file content
    dCpp << "#include \"" << derivedClassFileHPP.GetFullName() << "\"\n\n"
         << inheritedClass << "::" << inheritedClass << GetDerivedClassCtorSignature() << "\n"
         << "    : " << baseClassName << GetParentCtorInitArgumentList() << "\n"
         << "{\n"
         << "}\n\n"
         << inheritedClass << "::~" << inheritedClass << "()\n"
         << "{\n"
         << "}\n\n";

    // Keep track of the generated files
    if(WantsSubclass()) {
        wxcProjectMetadata::Get().SetGeneratedHeader(derivedClassFileHPP);
        wxcProjectMetadata::Get().SetGeneratedSource(derivedClassFileCPP);
    }

    wxcProjectMetadata::Get().SetGeneratedClassName(inheritedClass);
    wxcProjectMetadata::Get().SetVirtualFolder(GetVirtualFolder());

    if(WantsSubclass() && wxCrafter::IsTheSame(derivedClassFileHPP, dH) == false) {
        wxCrafter::WriteFile(derivedClassFileHPP, dH, false);
    }

    if(WantsSubclass() && wxCrafter::IsTheSame(derivedClassFileCPP, dCpp) == false) {
        wxCrafter::WriteFile(derivedClassFileCPP, dCpp, false);
    }

    // Tell CodeLite to reload externall modified files
    EventNotifier::Get()->PostReloadExternallyModifiedEvent(true);
}

// static
void TopLevelWinWrapper::WrapXRC(wxString& text)
{
    wxString prefix, sifa;
    prefix << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>"
           << "<resource xmlns=\"http://www.wxwidgets.org/wxxrc\">";
    sifa << "</resource>";
    text.Prepend(prefix).Append(sifa);
}

bool TopLevelWinWrapper::IsWxTopLevelWindow() const
{
    return (GetType() == ID_WXWIZARD || GetType() == ID_WXDIALOG || GetType() == ID_WXFRAME);
}

wxString TopLevelWinWrapper::CppCtorCode() const { return CPPCommonAttributes(); }
