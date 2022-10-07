#include "FormatterPage.hpp"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

#define UPDATE_TEXT_CB(SETTER_METHOD)                   \
    [this](const wxString& label, const wxAny& value) { \
        wxUnusedVar(label);                             \
        wxString str_value;                             \
        if(value.GetAs(&str_value)) {                   \
            m_cur_formatter->SETTER_METHOD(str_value);  \
        }                                               \
    }

#define UPDATE_BOOL_CB(SETTER_METHOD)                   \
    [this](const wxString& label, const wxAny& value) { \
        wxUnusedVar(label);                             \
        bool bool_value;                                \
        if(value.GetAs(&bool_value)) {                  \
            m_cur_formatter->SETTER_METHOD(bool_value); \
        }                                               \
    }

#define UPDATE_LANGS_CB()                                                                    \
    [this](const wxString& label, const wxAny& value) {                                      \
        wxUnusedVar(label);                                                                  \
        wxString str_value;                                                                  \
        if(value.GetAs(&str_value)) {                                                        \
            m_cur_formatter->SetLanguages(wxStringTokenize(str_value, ";", wxTOKEN_STRTOK)); \
        }                                                                                    \
    }

FormatterPage::FormatterPage(wxWindow* parent)
    : clPropertiesPage(parent)
{
}

FormatterPage::~FormatterPage() {}

void FormatterPage::Load(std::shared_ptr<GenericFormatter> formatter)
{
    Clear();
    CHECK_PTR_RET(formatter);
    m_cur_formatter = formatter;

    AddHeader(_("General"));
    AddProperty(_("Enabled"), m_cur_formatter->IsEnabled(), UPDATE_BOOL_CB(SetEnabled));
    AddProperty(_("Inplace edit"), m_cur_formatter->IsInlineFormatter(), UPDATE_BOOL_CB(SetInlineFormatter));
    AddProperty(_("Working directory"), m_cur_formatter->GetWorkingDirectory(), UPDATE_TEXT_CB(SetWorkingDirectory));
    AddPropertyLanguagePicker(_("Supported languages"), m_cur_formatter->GetLanguages(), UPDATE_LANGS_CB());

    AddHeader(_("Commands"));
    AddProperty(_("Local"), m_cur_formatter->GetCommandWithComments(), UPDATE_TEXT_CB(SetCommandFromString));
    AddProperty(_("Remote"), m_cur_formatter->GetRemoteCommandWithComments(),
                UPDATE_TEXT_CB(SetRemoteCommandFromString));
}

void FormatterPage::Clear()
{
    clPropertiesPage::Clear();
    m_cur_formatter = nullptr;
}
