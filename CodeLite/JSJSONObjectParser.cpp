#include "JSJSONObjectParser.h"
#include "JSLexerAPI.h"
#include "JSLexerTokens.h"

JSJSONObjectParser::JSJSONObjectParser(const wxString& text, JSLookUpTable::Ptr_t lookup)
    : m_lookup(lookup)
{
    m_scanner = ::jsLexerNew(text);
}

JSJSONObjectParser::~JSJSONObjectParser()
{
    if(m_scanner) {
        ::jsLexerDestroy(&m_scanner);
    }
}

bool JSJSONObjectParser::ReadUntil(int until)
{
    JSLexerToken token;
    int depth(0);
    while(::jsLexerNext(m_scanner, token)) {
        if(token.type == '{') {
            ++depth;
        } else if(token.type == '}') {
            --depth;
        }

        // breaking condition
        if((token.type == until) && (depth == 0)) {
            return true;
        } else if(depth < 0) {
            return false;
        }
    }
    return false;
}

bool JSJSONObjectParser::Parse(JSObject::Ptr_t parent)
{
    JSLexerToken token;
    if(!parent) {
        if(!::jsLexerNext(m_scanner, token)) return false;
        if(token.type == '[') {
            // an array
            if(!ReadUntil(']')) return false;
            m_result.Reset(new JSObject());
            m_result->SetType("Array");
            return true;

        } else if(token.type == '{') {
            // An object, recurse it
            m_result.Reset(new JSObject());
            m_result->SetType(GenerateName());
            m_result->SetPath(m_result->GetType());
            if(Parse(m_result)) {
                m_lookup->AddObject(m_result);
                return true;
            }
            return false;

        } else {
            return false;
        }
    }

    // searching for "[" or "{"
    JSObject::Ptr_t curobj(NULL);
    
    wxString tmpLabel;
    wxString label;
    while(::jsLexerNext(m_scanner, token)) {
        switch(token.type) {
        case kJS_IDENTIFIER:
            tmpLabel = token.text;
            break;
        case '[': {
            // looking for "[" or "}" or any possible value...
            // found an array, consume its body
            if(!ReadUntil(']')) return false;
            JSObject::Ptr_t arr(new JSObject());
            arr->SetName(label);
            arr->SetType("Array");
            parent->AddChild(arr);
            label.Clear();
        } break;
        case ',':
            label.Clear();
            tmpLabel.Clear();
            break;
        case ':':
            tmpLabel.swap(label);
            tmpLabel.Clear();
            break;
        case '{': {
            // found a new object
            if(!label.IsEmpty()) {
                JSObject::Ptr_t obj(new JSObject());
                obj->SetName(label);
                obj->SetType(GenerateName());
                obj->SetPath(obj->GetType());
                parent->AddChild(obj);

                // Recurse into this object
                if(!Parse(obj)) return false;

                // register this object
                m_lookup->AddObject(obj);

            } else {
                if(!Parse(parent)) return false;
            }

            label.Clear();

        } break;
        case '}':
            return true;
        case kJS_TRUE:
        case kJS_FALSE: {
            if(!label.IsEmpty()) {
                JSObject::Ptr_t obj(new JSObject());
                obj->SetName(label);
                obj->SetType("Boolean");
                parent->AddChild(obj);
                label.Clear();
            }
        } break;
        case kJS_DEC_NUMBER:
        case kJS_OCTAL_NUMBER:
        case kJS_HEX_NUMBER:
        case kJS_FLOAT_NUMBER: {
            if(!label.IsEmpty()) {
                JSObject::Ptr_t obj(new JSObject());
                obj->SetName(label);
                obj->SetType("Number");
                parent->AddChild(obj);
                label.Clear();
            }
        } break;
        case kJS_STRING: {
            if(!label.IsEmpty()) {
                JSObject::Ptr_t obj(new JSObject());
                obj->SetName(label);
                obj->SetType("String");
                parent->AddChild(obj);
                label.Clear();
            }
        } break;
        case kJS_FUNCTION:
            // FIXME: parse function, for now, just consume its body
            if(!ReadUntil('}')) return false;
            label.Clear();
            break;
        default:
            label.Clear();
            break;
        }
    }
    return true;
}

wxString JSJSONObjectParser::GenerateName()
{
    return m_lookup->GenerateNewType();
}
