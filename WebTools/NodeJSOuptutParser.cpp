#include "NodeJSOuptutParser.h"

NodeJSOuptutParser::NodeJSOuptutParser() {}

NodeJSOuptutParser::~NodeJSOuptutParser() {}

NodeJSHandle NodeJSOuptutParser::ParseRef(const JSONElement& ref, std::map<int, NodeJSHandle>& handles)
{
    int handleId = ref.namedObject("handle").toInt();
    NodeJSHandle h;
    h.handleID = handleId;
    h.type = ref.namedObject("type").toString();
    if(h.type == "undefined") {
        h.value = "undefined";
    } else if(h.type == "number" || h.type == "boolean") {
        h.value = ref.namedObject("text").toString();
    } else if(h.type == "string") {
        // Make this string repesentable in a single line
        h.value << "\"" << ref.namedObject("text").toString() << "\"";
        h.value.Replace("\n", "\\n");
        h.value.Replace("\t", "\\t");
        h.value.Replace("\r", "\\r");
    } else if(h.type == "script" || h.type == "function") {
        h.value = ref.namedObject("name").toString();
    } else if(h.type == "null") {
        h.value = "null";
    } else if(h.type == "object") {

        if(ref.hasNamedObject("protoObject")) {
            h.properties.push_back(
                std::make_pair(ref.namedObject("protoObject").namedObject("ref").toInt(), "prototype"));
        }

        if(ref.hasNamedObject("className") && ref.namedObject("className").toString() == "Array") {
            h.type = "Array";
            h.value = "[]";
        } else if(ref.hasNamedObject("text")) {
            h.value = ref.namedObject("text").toString();
        } else {
            h.value = "{...}";
        }
        JSONElement props = ref.namedObject("properties");
        int propsCount = props.arraySize();
        for(int n = 0; n < propsCount; ++n) {
            JSONElement prop = props.arrayItem(n);
            wxString propName;
            if(prop.namedObject("name").isString()) {
                propName = prop.namedObject("name").toString();
            } else if(prop.namedObject("name").isNumber()) {
                propName << "[" << prop.namedObject("name").toInt() << "]";
            }

            int propId = prop.namedObject("ref").toInt();
            h.properties.push_back(std::make_pair(propId, propName));
        }
    }
    handles.insert(std::make_pair(handleId, h));
    return h;
}
