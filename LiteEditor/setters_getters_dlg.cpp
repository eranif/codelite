//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : setters_getters_dlg.cpp
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
#include "precompiled_header.h"
#include "editor_config.h"
#include "ctags_manager.h"
#include "setters_getters_dlg.h"
#include "macros.h"
#include "language.h"
#include "wx/tokenzr.h"
#include "pluginmanager.h"
#include "bitmap_loader.h"
#include "windowattrmanager.h"
#include "cl_config.h"

//----------------------------------------------------

SettersGettersDlg::SettersGettersDlg(wxWindow* parent)
    : SettersGettersBaseDlg(parent)
    , m_checkForDuplicateEntries(false)
{
    clConfig::Get().ReadItem(&m_settings);
    m_checkStartWithUppercase->SetValue( m_settings.GetFlags() & SettersGetterData::FunctionStartWithUpperCase );
    m_checkBoxForamtFileWhenDone->SetValue( m_settings.GetFlags() & SettersGetterData::FormatFileWhenDone );
    SetName("SettersGettersDlg");
    WindowAttrManager::Load(this);
}

bool SettersGettersDlg::Init(const std::vector<TagEntryPtr> &tags, const wxFileName &file, int lineno)
{
    //convert the tags to string array
    m_file = file;
    m_lineno = lineno;
    m_members = tags;
    return BuildTree() > 0;
}

void SettersGettersDlg::OnCheckStartWithUpperCase(wxCommandEvent &event)
{
    wxUnusedVar(event);
    UpdateTree();
}

wxString SettersGettersDlg::GenerateFunctions()
{
    bool old_value = m_checkForDuplicateEntries;
    m_checkForDuplicateEntries = true;
    wxString code;
    GenerateSetters(code);
    if (code.IsEmpty() == false) {
        code << wxT("\n\n");
    }
    wxString settersCode;
    GenerateGetters(settersCode);
    if (settersCode.IsEmpty() == false) {
        code << settersCode << wxT("\n");
    }
    m_checkForDuplicateEntries = old_value;
    return code;
}

void SettersGettersDlg::GenerateGetters(wxString &code)
{
    wxDataViewItemArray members;
    m_dataviewModel->GetChildren(wxDataViewItem(0), members);

    for(size_t i=0; i<members.GetCount(); ++i) {
        wxDataViewItemArray funcs;
        wxDataViewItem member = members.Item(i);
        m_dataviewModel->GetChildren(member, funcs);

        for(size_t j=0; j<funcs.GetCount(); ++j) {
            bool already_exists = false;
            wxString display_name;
            SettersGettersTreeData *data = (SettersGettersTreeData *) m_dataviewModel->GetClientObject(funcs.Item(j));
            if ( data->m_kind == SettersGettersTreeData::Kind_Getter && data->m_checked ) {
                wxString getter_code = GenerateGetter(data->m_tag, already_exists, display_name);
                if ( !already_exists ) {
                    code << getter_code << "\n";
                }
                break;
            }
        }
    }
}

void SettersGettersDlg::GenerateSetters(wxString &code)
{
    wxDataViewItemArray members;
    m_dataviewModel->GetChildren(wxDataViewItem(0), members);

    for(size_t i=0; i<members.GetCount(); ++i) {
        wxDataViewItemArray funcs;
        wxDataViewItem member = members.Item(i);
        m_dataviewModel->GetChildren(member, funcs);

        for(size_t j=0; j<funcs.GetCount(); ++j) {
            bool already_exists = false;
            wxString display_name;
            SettersGettersTreeData *data = (SettersGettersTreeData *) m_dataviewModel->GetClientObject(funcs.Item(j));
            if ( data->m_kind == SettersGettersTreeData::Kind_Setter && data->m_checked ) {
                wxString setter_code = GenerateSetter(data->m_tag, already_exists, display_name);
                if ( !already_exists ) {
                    code << setter_code << "\n";
                }
                break;
            }
        }
    }
}

wxString SettersGettersDlg::GenerateSetter(TagEntryPtr tag, bool &alreadyExist, wxString &displayName)
{
    alreadyExist = false;
    bool startWithUpper  = m_checkStartWithUppercase->IsChecked();

    Variable var;
    wxString method_name, method_signature;

    if (LanguageST::Get()->VariableFromPattern(tag->GetPattern(), tag->GetName(), var)) {
        wxString func;
        wxString scope = _U(var.m_typeScope.c_str());
        func << wxT("void ");

        if (startWithUpper) {
            method_name << wxT("Set");
        } else {
            method_name << wxT("set");
        }

        wxString name = _U(var.m_name.c_str());
        FormatName(name);
        method_name << name;

        // add the method name
        func << method_name;

        // add the signature
        if( var.m_isBasicType ) {
            method_signature << wxT("(");

        } else if (!var.m_isPtr) {
            method_signature << wxT("(const ");

        } else {
            method_signature << wxT("(");

        }

        if (!scope.IsEmpty() && !(scope == wxT("<global>"))) {
            method_signature << scope << wxT("::");
        }

        method_signature << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str());
        if(var.m_isBasicType) {
            method_signature << wxT(" ");

        } else if (!var.m_isPtr) {
            method_signature << wxT("& ");

        } else {
            method_signature << wxT(" ");

        }

        wxString tmpName = _U(var.m_name.c_str());
        tmpName.StartsWith(wxT("m_"), &tmpName);

        method_signature << tmpName << wxT(")");
        func << method_signature;

        // at this point, func contains the display_name (i.e. the function without the implementation)
        displayName << func;

        // add the implementation
        func << wxT(" {this->") << _U(var.m_name.c_str()) << wxT(" = ") << tmpName << wxT(";}");

        if ( m_checkForDuplicateEntries ) {
            alreadyExist = DoCheckExistance(tag->GetScope(), method_name, method_signature);
        }

        return func;
    }
    return wxEmptyString;
}

wxString SettersGettersDlg::GenerateGetter(TagEntryPtr tag, bool &alreadyExist, wxString &displayName)
{
    alreadyExist = false;
    bool startWithUpper  = m_checkStartWithUppercase->IsChecked();

    Variable var;
    int midFrom(0);

    wxString method_name, method_signature;
    if (LanguageST::Get()->VariableFromPattern(tag->GetPattern(), tag->GetName(), var)) {
        wxString func;
        wxString scope = _U(var.m_typeScope.c_str());

        wxString tagName = tag->GetName();
        tagName.MakeLower();


        bool isBool = (var.m_isBasicType && (var.m_type.find("bool") != std::string::npos));
        // Incase the member is named 'isXX'
        // disable the "isBool" functionality
        if(isBool && tagName.StartsWith(wxT("is"))) {
            isBool = false;

        } else if( isBool && (tagName.StartsWith(wxT("m_is")) || tagName.StartsWith(wxT("_is"))) ) {
            midFrom = 2;
        }

        if (!var.m_isPtr && !var.m_isBasicType) {
            func << wxT("const ");
            if (!scope.IsEmpty() && !(scope == wxT("<global>"))) {
                func << scope
                     << wxT("::");
            }
            func << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str()) << wxT("& ");

        } else {
            // generate different code for pointer
            if (!scope.IsEmpty() && !(scope == wxT("<global>"))) {
                func << scope
                     << wxT("::");
            }
            func << _U(var.m_type.c_str()) << _U(var.m_templateDecl.c_str()) << _U(var.m_starAmp.c_str()) << wxT(" ");
        }

        // Prepare the prefix.
        // Make sure that boolean getters are treated differently
        // by making the getter in the format of 'IsXXX' or 'isXXX'
        wxString prefix = wxT("get");
        if(isBool) {
            prefix = wxT("is");
        }

        if(startWithUpper) {
            wxString captializedPrefix = prefix.Mid(0, 1);
            captializedPrefix.MakeUpper().Append(prefix.Mid(1));
            prefix.swap(captializedPrefix);
        }

        wxString name = _U(var.m_name.c_str());
        FormatName(name);

        method_name << prefix; // Add the "Get"
        if(midFrom) {
            name = name.Mid(midFrom);
        }
        method_name << name;   // Add the name

        // add the method name
        func << method_name;
        if (!var.m_isPtr) {
            method_signature << wxT("() const");

        } else {
            method_signature << wxT("()");

        }

        // add the signature
        func << method_signature;

        if (m_checkForDuplicateEntries) {
            alreadyExist = DoCheckExistance(tag->GetScope(), method_name, method_signature);
        }

        displayName << func;

        // add the implementation
        func << wxT(" {return ") << _U(var.m_name.c_str()) << wxT(";}");

        return func;
    }
    return wxEmptyString;
}

void SettersGettersDlg::FormatName(wxString &name)
{
    if(name.StartsWith(wxT("m_"))) {
        name = name.Mid(2);

    } else if(name.StartsWith(wxT("_"))) {
        name = name.Mid(1);
    }

    wxStringTokenizer tkz(name, wxT("_"));
    name.Clear();
    while (tkz.HasMoreTokens()) {
        wxString token = tkz.NextToken();
        wxString pre = token.Mid(0, 1);
        token.Remove(0, 1);
        pre.MakeUpper();
        token.Prepend(pre);
        name << token;
    }
}

void SettersGettersDlg::UpdatePreview()
{
    m_code.Clear();
    m_code = GenerateFunctions();
}

void SettersGettersDlg::OnCheckAll(wxCommandEvent &e)
{
    wxDataViewItemArray members;
    m_dataviewModel->GetChildren(wxDataViewItem(0), members);

    for(size_t i=0; i<members.GetCount(); ++i) {
        wxDataViewItemArray funcs;
        wxDataViewItem member = members.Item(i);
        m_dataviewModel->GetChildren(member, funcs);

        for(size_t j=0; j<funcs.GetCount(); ++j) {
            SettersGettersTreeData *data = (SettersGettersTreeData *) m_dataviewModel->GetClientObject(funcs.Item(j));
            data->m_checked = true;
        }
    }
    UpdateTree();
}

void SettersGettersDlg::OnUncheckAll(wxCommandEvent &e)
{
    wxDataViewItemArray members;
    m_dataviewModel->GetChildren(wxDataViewItem(0), members);

    for(size_t i=0; i<members.GetCount(); ++i) {
        wxDataViewItemArray funcs;
        wxDataViewItem member = members.Item(i);
        m_dataviewModel->GetChildren(member, funcs);

        for(size_t j=0; j<funcs.GetCount(); ++j) {
            SettersGettersTreeData *data = (SettersGettersTreeData *) m_dataviewModel->GetClientObject(funcs.Item(j));
            data->m_checked = false;
        }
    }
    UpdateTree();
}

void SettersGettersDlg::OnUpdatePreview(wxCommandEvent& e)
{
    wxUnusedVar(e);
    UpdatePreview();
}

bool SettersGettersDlg::DoCheckExistance(const wxString& scope, const wxString& name, const wxString& method_signature)
{
    std::vector<TagEntryPtr> tmp_tags;
    TagsManagerST::Get()->FindByNameAndScope(name, scope.IsEmpty() ? wxT("<global>") : scope, tmp_tags);
    for ( size_t i=0; i<tmp_tags.size(); i++) {
        TagEntryPtr t = tmp_tags.at(i);
        wxString sig_one = TagsManagerST::Get()->NormalizeFunctionSig(t->GetSignature());
        wxString sig_two = TagsManagerST::Get()->NormalizeFunctionSig( method_signature );
        if ( sig_one == sig_two ) {
            return true;
        }
    }
    return false;
}

wxString SettersGettersDlg::GenerateSetter(TagEntryPtr tag)
{
    bool dummy;
    wxString s_dummy;
    return GenerateSetter(tag, dummy, s_dummy);
}


wxString SettersGettersDlg::GenerateGetter(TagEntryPtr tag)
{
    bool dummy;
    wxString s_dummy;
    return GenerateGetter(tag, dummy, s_dummy);
}


int SettersGettersDlg::BuildTree()
{
    m_dataviewModel->Clear();
    wxBitmap memberBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap("cc/16/member_public");
    wxBitmap funcBmp   = PluginManager::Get()->GetStdIcons()->LoadBitmap("cc/16/function_public");
    
    m_checkForDuplicateEntries = true;
    
    std::vector<TagEntryPtr> tmpTags = m_members;
    m_members.clear();
    for (size_t i=0; i<tmpTags.size() ; i++) {
        
        // add two children to generate the name of the next entries
        bool     getter_exist (false);
        bool     setter_exist (false);
        wxString setter_display_name;
        wxString getter_display_name;

        wxString getter = GenerateGetter(tmpTags.at(i), getter_exist, getter_display_name);
        wxString setter = GenerateSetter(tmpTags.at(i), setter_exist, setter_display_name);
        if ( getter_exist && setter_exist ) 
            continue;
        
        m_members.push_back( tmpTags.at(i) );
        wxVector<wxVariant> cols;
        cols.push_back( SettersGettersModel::CreateIconTextVariant(tmpTags.at(i)->GetName(), memberBmp) );
        cols.push_back( false );
        wxDataViewItem memberItem = m_dataviewModel->AppendItem(wxDataViewItem(0), cols);


        if ( !setter_exist ) {
            cols.clear();
            cols.push_back( SettersGettersModel::CreateIconTextVariant(setter_display_name, funcBmp) );
            cols.push_back( false );
            m_dataviewModel->AppendItem(memberItem, cols, new SettersGettersTreeData(tmpTags.at(i), SettersGettersTreeData::Kind_Setter, false));
        }
        
        if ( !getter_exist ) {
            cols.clear();
            cols.push_back( SettersGettersModel::CreateIconTextVariant(getter_display_name, funcBmp) );
            cols.push_back( false );
            m_dataviewModel->AppendItem(memberItem, cols, new SettersGettersTreeData(tmpTags.at(i), SettersGettersTreeData::Kind_Getter, false));
        }
        
        m_dataview->Expand( memberItem );
    }

    m_checkForDuplicateEntries = false;

    if (m_members.empty() == false) {
        wxString msg, title;
        msg << _("Select the functions to generate from the list below");
        title << _("Generate Setters/Getters for class '") << m_members.at(0)->GetParent() << _("'");
        m_banner6->SetText(title, msg);
    }
    return m_members.size();
}

void SettersGettersDlg::UpdateTree()
{
    wxDataViewItemArray members;
    m_dataviewModel->GetChildren(wxDataViewItem(0), members);

    wxBitmap memberBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap("cc/16/member_public");
    wxBitmap funcBmp   = PluginManager::Get()->GetStdIcons()->LoadBitmap("cc/16/function_public");

    for(size_t i=0; i<members.GetCount(); ++i) {
        wxDataViewItemArray funcs;
        wxDataViewItem member = members.Item(i);
        m_dataviewModel->GetChildren(member, funcs);

        for(size_t j=0; j<funcs.GetCount(); ++j) {
            SettersGettersTreeData *data = (SettersGettersTreeData *) m_dataviewModel->GetClientObject(funcs.Item(j));
            wxString display_name;
            bool dummy;
            if ( data->m_kind == SettersGettersTreeData::Kind_Getter ) {
                GenerateGetter(data->m_tag, dummy, display_name);

            } else if ( data->m_kind == SettersGettersTreeData::Kind_Setter ) {
                GenerateSetter(data->m_tag, dummy, display_name);

            }

            wxVector<wxVariant> cols;
            cols.push_back( SettersGettersModel::CreateIconTextVariant(display_name, funcBmp) );
            cols.push_back( data->m_checked );
            m_dataviewModel->UpdateItem( funcs.Item(j), cols );
        }
    }
}

void SettersGettersDlg::OnButtonOk(wxCommandEvent& e)
{
    e.Skip();
}

SettersGettersDlg::~SettersGettersDlg()
{
    size_t flags (0);
    if ( m_checkStartWithUppercase->IsChecked() )
        flags |= SettersGetterData::FunctionStartWithUpperCase;
    
    if ( m_checkBoxForamtFileWhenDone->IsChecked() )
        flags |= SettersGetterData::FormatFileWhenDone;
    m_settings.SetFlags( flags );
    clConfig::Get().WriteItem(&m_settings);
    
    
}

wxString SettersGettersDlg::GetGenCode()
{
    UpdatePreview();
    return m_code;
}

void SettersGettersDlg::OnValueChanged(wxDataViewEvent& event)
{
    SettersGettersTreeData *data = (SettersGettersTreeData *) m_dataviewModel->GetClientObject(event.GetItem());
    wxVariant v;
    m_dataviewModel->GetValue(v, event.GetItem(), 1);
    data->m_checked = v.GetBool();
}

//----------------------------------------------------
//----------------------------------------------------

SettersGetterData::SettersGetterData()
    : clConfigItem("SettersGetterData")
    , m_flags(FormatFileWhenDone|FunctionStartWithUpperCase)
{
}

SettersGetterData::~SettersGetterData()
{
}
void SettersGetterData::FromJSON(const JSONElement& json)
{
    m_flags = json.namedObject("m_flags").toSize_t(m_flags);
}

JSONElement SettersGetterData::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_flags", m_flags);
    return element;
}

