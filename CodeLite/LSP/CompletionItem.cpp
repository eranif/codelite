#include "CompletionItem.h"

LSP::CompletionItem::CompletionItem() {}

LSP::CompletionItem::~CompletionItem() {}

JSONItem LSP::CompletionItem::ToJSON(const wxString& name) const { return JSONItem(NULL); }

void LSP::CompletionItem::FromJSON(const JSONItem& json)
{
    m_label = json.namedObject("label").toString();
    m_kind = json.namedObject("kind").toInt(m_kind);
    m_detail = json.namedObject("detail").toString();
    m_documentation = json.namedObject("documentation").toString();
    m_filterText = json.namedObject("filterText").toString();
    m_insertText = json.namedObject("insertText").toString();

    m_insertText.Trim().Trim(false);
    m_label.Trim().Trim(false);
    m_detail.Trim().Trim(false);
    m_documentation.Trim().Trim(false);
    if(json.hasNamedObject("textEdit") && !json.namedObject("textEdit").isNull()) { 
        m_textEdit.reset(new LSP::TextEdit());
        m_textEdit->FromJSON(json.namedObject("textEdit")); 
    }
}
