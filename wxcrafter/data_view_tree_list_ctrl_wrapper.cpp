#include "data_view_tree_list_ctrl_wrapper.h"

#include "allocator_mgr.h"
#include "bool_property.h"
#include "data_view_list_ctrl_wrapper.h"
#include "wxc_project_metadata.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_helpers.h"

#include <wx/dataview.h>

DataViewTreeListCtrlWrapper::DataViewTreeListCtrlWrapper()
    : wxcWidget(ID_WXDATAVIEWTREELISTCTRL)
{
    PREPEND_STYLE(wxDV_SINGLE, true);
    PREPEND_STYLE(wxDV_MULTIPLE, false);
    PREPEND_STYLE(wxDV_ROW_LINES, true);
    PREPEND_STYLE(wxDV_HORIZ_RULES, false);
    PREPEND_STYLE(wxDV_VERT_RULES, false);
    PREPEND_STYLE(wxDV_VARIABLE_LINE_HEIGHT, false);
    PREPEND_STYLE(wxDV_NO_HEADER, false);

    RegisterEvent("wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED event.\nThis event is triggered by double "
                    "clicking an item or pressing some special key (usually \"Enter\") when it is focused"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_START_EDITING", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_START_EDITING event.\nThis event can be vetoed in order to "
                    "prevent editing on an item by item basis"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU event generated when the user right clicks "
                    "inside the control.\nNotice that this menu is generated even if the click didn't occur on any "
                    "valid item, in this case wxDataViewEvent::GetItem() simply returns an invalid item."));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_COLUMN_REORDERED", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_COLUMN_REORDERED event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE event"));
    RegisterEvent("wxEVT_COMMAND_DATAVIEW_ITEM_DROP", "wxDataViewEvent",
                  _("Process a wxEVT_COMMAND_DATAVIEW_ITEM_DROP event"));

    m_namePattern = "m_dataview";
    SetName(GenerateName());

    SetPropertyString(_("Common Settings"), "wxDataViewTreeListCtrl");
    DelProperty(_("Control Specific Settings"));
    AddProperty(new CategoryProperty(_("Inherited C++ Class Properties")));

    wxString tip;
    tip << _("The generated model class name\n")
        << _("wxCrafter will generate a TreeListCtrl model like 'Tree-List-Ctrl\n")
        << _("** Leave this field empty if you do not want wxCrafter to generate a model class for you");

    wxString name = GetName();
    // Using the plain name here, e.g. m_dataview13, creates code that won't compile:
    // wxObjectDataPtr<m_dataview13Model> m_dataview13Model;
    if(name.Left(2) == "m_") {
        name = name.Mid(2);
    }
    AddProperty(new StringProperty(PROP_DV_MODEL_CLASS_NAME, name + "Model", tip));
    AddProperty(new BoolProperty(PROP_DV_CONTAINER_ITEM_HAS_COLUMNS, true,
                                 _("Indicate if a container item merely acts as a headline (or for categorisation) or "
                                   "if it also acts a normal item with entries for further columns")));
}

DataViewTreeListCtrlWrapper::~DataViewTreeListCtrlWrapper() {}

wxcWidget* DataViewTreeListCtrlWrapper::Clone() const { return new DataViewTreeListCtrlWrapper(); }

wxString DataViewTreeListCtrlWrapper::CppCtorCode() const
{
    wxString code = CPPStandardWxCtor("wxDV_SINGLE|wxDV_ROW_LINES");
    code << AssociateModelCode();
    return code;
}

void DataViewTreeListCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/dataview.h>");

    wxString modelName = GetModelName();
    if(modelName.IsEmpty()) {
        return;
    }

    // Model file name
    wxString headerFileName = modelName;
    headerFileName << "." << wxcProjectMetadata::Get().GetHeaderFileExt();
    headerFileName.MakeLower();

    wxString modelIncludeStatement;
    modelIncludeStatement << "#include \"" << headerFileName << "\"";
    headers.Add(modelIncludeStatement);
}

wxString DataViewTreeListCtrlWrapper::GetWxClassName() const { return "wxDataViewCtrl"; }

void DataViewTreeListCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();

    } else {
        text << "<object class=\""
             << "wxDataViewListCtrl"
             << "\" name=\"" << wxCrafter::XMLEncode(GetName()) << "\"";
        if(!PropertyString(PROP_SUBCLASS_NAME).empty()) {
            text << " subclass=\"" << wxCrafter::XMLEncode(PropertyString(PROP_SUBCLASS_NAME)) << "\"";
        }
        text << ">";

        text << XRCSize() << XRCStyle() << XRCCommonAttributes();

        // Columns...
        ChildrenXRC(text, type);

        text << XRCSuffix();
    }
}

void DataViewTreeListCtrlWrapper::GenerateAdditionalFiles(wxStringMap_t& extraFile) const
{
    wxString modelName = GetModelName();
    if(modelName.IsEmpty()) {
        return;
    }

    wxCrafter::ResourceLoader rl;
    wxString modelCpp = rl.File("my_tree_list_model.cpp");
    wxString modelHpp = rl.File("my_tree_list_model.hpp");

    wxString blockGuard = modelName;
    blockGuard << "_GUARD__" << wxcProjectMetadata::Get().GetHeaderFileExt();
    blockGuard.MakeUpper();

    // Replace place holders
    modelCpp.Replace("MODEL_NAME", modelName);
    modelHpp.Replace("MODEL_NAME", modelName);
    modelHpp.Replace("BLOCK_GUARD", blockGuard);
    modelHpp.Replace("HAS_CONTAINER_COLUMNS", PropertyBool(PROP_DV_CONTAINER_ITEM_HAS_COLUMNS));

    wxString headerFileName, cppFileName;
    wxString basename = modelName;
    basename.MakeLower();

    cppFileName = basename;
    headerFileName = basename;

    cppFileName << ".cpp";
    headerFileName << "." << wxcProjectMetadata::Get().GetHeaderFileExt();

    // change the include statement in the template file
    modelCpp.Replace("my_tree_list_model.hpp", headerFileName);

    // add the files
    extraFile.insert(std::make_pair(headerFileName, modelHpp));
    extraFile.insert(std::make_pair(cppFileName, modelCpp));
}

wxString DataViewTreeListCtrlWrapper::DoGenerateClassMember() const
{
    wxString members;
    members << wxcWidget::DoGenerateClassMember();

    wxString modelName = GetModelName();
    if(modelName.IsEmpty()) {
        return members;
    }

    // Add the model member
    members << "\n    "
            << "wxObjectDataPtr<" << modelName << "> " << GetName() << "Model;\n";
    return members;
}

wxString DataViewTreeListCtrlWrapper::AssociateModelCode() const
{
    wxString modelBaseName = GetModelName();
    if(modelBaseName.IsEmpty()) {
        return "";
    }

    wxString code;
    wxString modelName = GetName() + "Model";
    code << "\n";
    code << modelName << " = new " << modelBaseName << ";\n";
    code << modelName << "->SetColCount( " << GetChildren().size() << " );\n";
    code << GetName() << "->AssociateModel(" << modelName << ".get() );\n";
    return code;
}

wxString DataViewTreeListCtrlWrapper::GetModelName() const
{
    wxString modelName = PropertyString(PROP_DV_MODEL_CLASS_NAME);
    modelName.Trim().Trim(false);
    return modelName;
}
