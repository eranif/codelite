#include "CompletionItem.h"
#include "JSON.h"

LSP::CompletionItem::CompletionItem() {}

LSP::CompletionItem::~CompletionItem() {}

JSONItem LSP::CompletionItem::ToJSON(const wxString& name) const { return JSONItem(NULL); }

void LSP::CompletionItem::FromJSON(const JSONItem& json)
{
    m_label = json.namedObject("label").toString();
    m_kind = json.namedObject("kind").toInt(m_kind);
    m_detail = json.namedObject("detail").toString();

    // parse the documentation
    auto doc = json.namedObject("documentation");
    if(doc.isOk()) {
        if(doc.isString()) {
            // plain string, nothing more to be done here
            m_documentation.SetKind("plaintext");
            m_documentation.SetValue(doc.toString());
        } else {
            // an object of type MarkupContent
            m_documentation.FromJSON(doc);
        }
    }

    m_filterText = json.namedObject("filterText").toString();
    m_insertText = json.namedObject("insertText").toString();
    m_insertTextFormat = json.namedObject("insertTextFormat").toString();
    m_vAdditionalText.clear();
    if(json.hasNamedObject("additionalTextEdits")) {
        JSONItem additionalTextEdits = json.namedObject("additionalTextEdits");
        int count = additionalTextEdits.arraySize();
        for(int i = 0; i < count; ++i) {
            wxSharedPtr<TextEdit> edit(new TextEdit());
            edit->FromJSON(additionalTextEdits.arrayItem(i));
            m_vAdditionalText.push_back(edit);
        }
    }

    m_insertText.Trim().Trim(false);
    m_label.Trim().Trim(false);
    m_detail.Trim().Trim(false);
    m_documentation.SetValue(wxString(m_documentation.GetValue()).Trim().Trim(false));
    if(json.hasNamedObject("textEdit") && !json.namedObject("textEdit").isNull()) {
        m_textEdit.reset(new LSP::TextEdit());
        m_textEdit->FromJSON(json.namedObject("textEdit"));
    }
}
