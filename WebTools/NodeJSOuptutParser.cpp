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
        h.value << "\"" << ref.namedObject("text").toString() << "\"";
    } else if(h.type == "script" || h.type == "function") {
        h.value = ref.namedObject("name").toString();
    } else if(h.type == "null") {
        h.value = "null";
    } else if(h.type == "object") {
        h.value = "{...}";
        JSONElement props = ref.namedObject("properties");
        int propsCount = props.arraySize();
        for(int n = 0; n < propsCount; ++n) {
            JSONElement prop = props.arrayItem(n);
            wxString propName = prop.namedObject("name").toString();
            int propId = prop.namedObject("ref").toInt();
            h.properties.insert(std::make_pair(propId, propName));
        }
    }
    handles.insert(std::make_pair(handleId, h));
    return h;
}
