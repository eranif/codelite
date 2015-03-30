#include "JSObjectParser.h"
#include "JSLexerAPI.h"
#include "JSLexerTokens.h"
#include "JSFunction.h"

JSObjectParser::JSObjectParser(JSSourceFile& sourceFile, JSLookUpTable::Ptr_t lookup)
    : m_sourceFile(sourceFile)
    , m_lookup(lookup)
{
}

JSObjectParser::~JSObjectParser() {}

bool JSObjectParser::ReadUntil(int until)
{
    JSLexerToken token;
    int depth(0);
    while(m_sourceFile.NextToken(token)) {
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

void JSObjectParser::SetResultObject(const wxString& type)
{
    m_result = m_lookup->NewObject();
    m_result->SetType(type);
}

bool JSObjectParser::Parse(JSObject::Ptr_t parent)
{
    JSLexerToken token;
    if(!parent) {
        // read the first non "(" token ("(" might be a self executing function)
        while(true) {
            if(!m_sourceFile.NextToken(token)) return false;
            if(token.type == '(') continue;
            break;
        }

        if(token.type == '[') {
            // an array
            if(!ReadUntil(']')) return false;
            SetResultObject("Array");
            return true;

        } else if(token.type == '{') {
            // An object, recurse it
            SetResultObject(GenerateName());
            if(Parse(m_result)) {
                m_lookup->AddObject(m_result);
                return true;
            }
            return false;

        } else if(token.type == kJS_NEW) {
            // an instation of an object
            if(!m_sourceFile.NextToken(token)) return false;
            if(token.type == kJS_IDENTIFIER) {
                SetResultObject(token.text);
                return true;
            }
        } else if(token.type == kJS_DEC_NUMBER || token.type == kJS_OCTAL_NUMBER || token.type == kJS_HEX_NUMBER ||
                  token.type == kJS_FLOAT_NUMBER) {
            SetResultObject("Number");
            return true;
        } else if(token.type == kJS_TRUE || token.type == kJS_FALSE) {
            SetResultObject("Boolean");
            return true;
        } else if(token.type == kJS_NULL) {
            SetResultObject("null");
            return true;
        } else if(token.type == kJS_STRING) {
            SetResultObject("String");
            return true;
        } else if(token.type == kJS_FUNCTION) {
            m_result = m_lookup->NewFunction();
            m_sourceFile.OnFunction();
            return true;
        } else if(token.type == kJS_IDENTIFIER) {
            // Check to see if this is a type
            JSObject::Ptr_t t = m_lookup->FindClass(token.text);
            if(t) {
                m_result = t->NewInstance(token.text);
                return true;
            } else {
                // not a type, try to see if the identifier is a local variable
                JSObject::Map_t locals = m_lookup->GetVisibleVariables();
                if(locals.count(token.text)) {
                    JSObject::Ptr_t o = locals.find(token.text)->second;
                    m_result = o->NewInstance(token.text);
                } else {
                    // an Object
                    SetResultObject("Object");
                }
                return true;
            }
        } else {
            return false;
        }
    }

    // searching for "[" or "{"
    JSObject::Ptr_t curobj(NULL);

    wxString tmpLabel;
    wxString label;
    while(m_sourceFile.NextToken(token)) {
        switch(token.type) {
        case kJS_VAR:
            m_sourceFile.OnVariable();
            break;
        case kJS_IDENTIFIER:
            if(!label.IsEmpty()) {
                // we already have a label, so this is the value
                JSObject::Ptr_t obj;
                JSObject::Ptr_t templ = m_lookup->FindClass(token.text);
                if(templ) {
                    // Create a new instance of this object
                    obj = templ->NewInstance(label);
                } else {
                    obj = m_lookup->NewObject();
                    obj->SetName(label);
                    obj->AddType(token.text);
                }
                parent->AddProperty(obj);
                label.clear();
            } else {
                tmpLabel = token.text;
            }
            break;
        case '[': {
            // looking for "[" or "}" or any possible value...
            // found an array, consume its body
            if(!ReadUntil(']')) return false;
            JSObject::Ptr_t arr = m_lookup->NewObject();
            arr->SetName(label);
            arr->AddType("Array");
            parent->AddProperty(arr);
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
                JSObject::Ptr_t obj = m_lookup->NewObject();
                obj->SetName(label);
                obj->AddType(GenerateName());
                parent->AddProperty(obj);

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
        case kJS_NULL: {
            if(!label.IsEmpty()) {
                JSObject::Ptr_t obj = m_lookup->NewObject();
                obj->SetName(label);
                obj->AddType("null");
                parent->AddProperty(obj);
                label.Clear();
            }
        } break;
        case kJS_TRUE:
        case kJS_FALSE: {
            if(!label.IsEmpty()) {
                JSObject::Ptr_t obj = m_lookup->NewObject();
                obj->SetName(label);
                obj->AddType("Boolean");
                parent->AddProperty(obj);
                label.Clear();
            }
        } break;
        case kJS_DEC_NUMBER:
        case kJS_OCTAL_NUMBER:
        case kJS_HEX_NUMBER:
        case kJS_FLOAT_NUMBER: {
            if(!label.IsEmpty()) {
                JSObject::Ptr_t obj = m_lookup->NewObject();
                obj->SetName(label);
                obj->AddType("Number");
                parent->AddProperty(obj);
                label.Clear();
            }
        } break;
        case kJS_STRING: {
            if(!label.IsEmpty()) {
                JSObject::Ptr_t obj = m_lookup->NewObject();
                obj->SetName(label);
                obj->AddType("String");
                parent->AddProperty(obj);
                label.Clear();
            } else {
                wxString strippedLabel = token.text;
                if(strippedLabel.StartsWith("\"") || strippedLabel.StartsWith("'")) {
                    strippedLabel.Remove(0, 1);
                }
                if(strippedLabel.EndsWith("\"") || strippedLabel.EndsWith("'")) {
                    strippedLabel.RemoveLast();
                }
                tmpLabel = strippedLabel;
            }
        } break;
        case kJS_FUNCTION: {
            JSObject::Ptr_t func = m_lookup->NewFunction();
            func->SetName(label);
            parent->AddProperty(func);
            label.Clear();

            m_sourceFile.OnFunction();
        } break;
        default:
            label.Clear();
            break;
        }
    }
    return true;
}

wxString JSObjectParser::GenerateName() { return m_lookup->GenerateNewType(); }

bool JSObjectParser::ReadSignature(JSObject::Ptr_t scope)
{
    wxString sig;
    JSLexerToken token;

    if(!m_sourceFile.NextToken(token)) return false;
    if(token.type != '(') return false;

    sig << "(";

    wxString curarg;
    int depth = 1;
    while(m_sourceFile.NextToken(token)) {

        if(curarg.IsEmpty() && token.type == kJS_IDENTIFIER) {
            curarg << token.text;
        }
        sig << token.text;

        if(token.type == '{') {
            ++depth;
        } else if(token.type == '}') {
            --depth;
        } else if(!curarg.IsEmpty() && (token.type == ',' || token.type == ')')) {
            JSObject::Ptr_t arg = m_lookup->NewObject();
            arg->SetName(curarg);
            arg->SetLine(token.lineNumber);
            scope->As<JSFunction>()->AddVariable(arg);
            scope->As<JSFunction>()->AddArgument(arg);
            curarg.Clear();
        }

        // breaking condition
        if((token.type == ')') && (1 == depth)) {
            break;
        } else if(depth < 1) {
            return false;
        }
    }

    scope->As<JSFunction>()->SetSignature(sig);
    return true;
}
