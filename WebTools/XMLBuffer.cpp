#include "XMLBuffer.h"
#include "XMLLexerTokens.h"
#include "macros.h"

XMLBuffer::XMLBuffer(const wxString& buffer)
    : m_buffer(buffer)
    , m_state(kNormal)
{
    m_scanner = ::xmlLexerNew(buffer);
}

XMLBuffer::~XMLBuffer()
{
    if(m_scanner) {
        ::xmlLexerDestroy(&m_scanner);
    }
}

void XMLBuffer::Parse()
{
    m_elements.clear();
    CHECK_PTR_RET(m_scanner);
    XMLLexerToken token;
    m_state = kNormal;
    while(::xmlLexerNext(m_scanner, token)) {
        switch(token.type) {
        case kXML_T_CDATA_START:
            m_state = kCdata;
            if(ConsumeUntil(kXML_T_CDATA_END)) {
                m_state = kNormal;
            }
            break;
        case kXML_T_COMMENT_START:
            m_state = kComment;
            if(ConsumeUntil(kXML_T_COMMENT_END)) {
                m_state = kNormal;
            }
            break;
        case kXML_T_OPEN_TAG:
            // "<" was found
            OnOpenTag();
            break;
        case kXML_T_CLOSE_TAG_PREFIX:
            // "</" was found
            OnTagClosePrefix();
            break;
        case kXML_T_CLOSE_TAG_SUFFIX:
            // "/>" closing tag was found
            if(!m_elements.empty()) m_elements.pop_back();
            break;
        default:
            break;
        }
    }
}

void XMLBuffer::OnOpenTag()
{
    XMLLexerToken token;
    if(!::xmlLexerNext(m_scanner, token)) return;
    if(token.type == kXML_T_IDENTIFIER) {
        m_elements.push_back(token.text);
    }
}

void XMLBuffer::OnTagClosePrefix()
{
    XMLLexerToken token;
    if(!::xmlLexerNext(m_scanner, token)) return;
    if(token.type == kXML_T_IDENTIFIER) {
        if(!m_elements.empty()) {
            m_elements.pop_back();
        }
    }
}

wxString XMLBuffer::GetCurrentScope() const
{
    if(m_elements.empty()) return "";
    return m_elements.back();
}

bool XMLBuffer::ConsumeUntil(int until)
{
    XMLLexerToken token;
    while(::xmlLexerNext(m_scanner, token)) {
        if(token.type == until) return true;
    }
    return false;
}
