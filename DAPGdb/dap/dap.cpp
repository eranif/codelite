#include "dap.hpp"

#define CREATE_JSON() JSONItem json = JSONItem::createObject(name)
#define REQUEST_TO() JSONItem json = Request::To(name)
#define RESPONSE_TO() JSONItem json = Response::To(name)
#define PROTOCOL_MSG_TO() JSONItem json = ProtocolMessage::To(name)
#define EVENT_TO() JSONItem json = Event::To(name)
#define ADD_OBJ(obj) json.add(obj.To(#obj))
#define ADD_PROP(obj) json.add(#obj, obj)

#define REQUEST_FROM() Request::From(json)
#define RESPONSE_FROM() Response::From(json)
#define EVENT_FROM() Event::From(json)
#define PROTOCOL_MSG_FROM() ProtocolMessage::From(json)
#define READ_OBJ(obj) obj.From(json.property(#obj))
#define GET_PROP(prop, Type) prop = json.property(#prop).to##Type(prop)

#define ADD_BODY()                             \
    JSONItem body = json.createObject("body"); \
    json.add(body)
#define ADD_BODY_PROP(prop) body.add(#prop, prop)
#define ADD_ARRAY(Parent, Name)              \
    JSONItem arr = Parent.createArray(Name); \
    Parent.add(arr);

#define READ_BODY() JSONItem body = json.property("body")

#define GET_BODY_PROP(prop, Type) prop = body.property(#prop).to##Type()

namespace dap
{

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ProtocolMessage::To(const string& name) const
{
    CREATE_JSON();
    ADD_PROP(seq);
    ADD_PROP(type);
    return json;
}

void ProtocolMessage::From(const JSONItem& json)
{
    GET_PROP(seq, Int);
    GET_PROP(type, String);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
Request::~Request() {}

JSONItem Request::To(const string& name) const
{
    PROTOCOL_MSG_TO();
    ADD_PROP(command);
    return json;
}

void Request::From(const JSONItem& json)
{
    PROTOCOL_MSG_FROM();
    GET_PROP(command, String);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem CancelRequest::To(const string& name) const
{
    JSONItem json = Request::To(name);
    JSONItem arguments = JSONItem::createObject("arguments");
    json.add(arguments);
    arguments.add("requestId", requestId);
    return json;
}

void CancelRequest::From(const JSONItem& json)
{
    Request::From(json);
    if(json.hasProperty("arguments")) { requestId = json.property("requestId").toInt(); }
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
Event::~Event() {}

JSONItem Event::To(const string& name) const
{
    JSONItem json = ProtocolMessage::To(name);
    json.add("event", event);
    if(body) { json.add(body->To("body")); }
    return json;
}

void Event::From(const JSONItem& json)
{
    ProtocolMessage::From(json);
    event = json.property("event").toString();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
Response::~Response() {}
JSONItem Response::To(const string& name) const
{
    JSONItem json = ProtocolMessage::To(name);
    json.add("request_seq", request_seq);
    json.add("success", success);
    json.add("message", message);
    return json;
}

void Response::From(const JSONItem& json)
{
    ProtocolMessage::From(json);
    request_seq = json.property("request_seq").toInt(request_seq);
    success = json.property("success").toBool(success);
    message = json.property("message").toBool();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem InitializedEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    return json;
}

void InitializedEvent::From(const JSONItem& json) { Event::From(json); }

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem StoppedEvent::To(const string& name) const
{
    EVENT_TO();
    ADD_BODY();

    body.add("reason", reason);
    body.add("text", text);
    return json;
}

void StoppedEvent::From(const JSONItem& json)
{
    Event::From(json);
    JSONItem body = json.property("body");
    reason = body.property("reason").toString();
    text = body.property("text").toString();
}
// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ContinuedEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    JSONItem body = JSONItem::createObject("body");
    json.add(body);
    body.add("threadId", threadId);
    body.add("allThreadsContinued", allThreadsContinued);
    return json;
}

void ContinuedEvent::From(const JSONItem& json)
{
    Event::From(json);
    JSONItem body = json.property("body");
    threadId = body.property("threadId").toInt();
    allThreadsContinued = body.property("allThreadsContinued").toBool(false);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ExitedEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    JSONItem body = JSONItem::createObject("body");
    json.add(body);
    body.add("exitCode", exitCode);
    return json;
}

void ExitedEvent::From(const JSONItem& json)
{
    Event::From(json);
    JSONItem body = json.property("body");
    exitCode = body.property("exitCode").toInt();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem TerminatedEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    return json;
}

void TerminatedEvent::From(const JSONItem& json) { Event::From(json); }

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ThreadEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    JSONItem body = JSONItem::createObject("body");
    json.add(body);
    body.add("reason", reason);
    body.add("threadId", threadId);
    return json;
}

void ThreadEvent::From(const JSONItem& json)
{
    Event::From(json);
    JSONItem body = json.property("body");
    reason = body.property("reason").toString();
    threadId = body.property("threadId").toInt();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem OutputEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    JSONItem body = JSONItem::createObject("body");
    json.add(body);
    body.add("category", category);
    body.add("output", output);
    return json;
}

void OutputEvent::From(const JSONItem& json)
{
    Event::From(json);
    JSONItem body = json.property("body");
    category = body.property("category").toString();
    output = body.property("output").toString();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem Source::To(const string& name) const
{
    CREATE_JSON();
    ADD_PROP(name);
    ADD_PROP(path);
    return json;
}

void Source::From(const JSONItem& json)
{
    GET_PROP(name, String);
    GET_PROP(path, String);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem Breakpoint::To(const string& name) const
{
    CREATE_JSON();
    ADD_PROP(id);
    ADD_PROP(verified);
    ADD_PROP(message);
    ADD_PROP(line);
    ADD_PROP(column);
    ADD_PROP(endLine);
    ADD_PROP(endColumn);
    ADD_OBJ(source);
    return json;
}

void Breakpoint::From(const JSONItem& json)
{
    GET_PROP(id, Int);
    GET_PROP(verified, Bool);
    GET_PROP(message, String);
    GET_PROP(line, Int);
    GET_PROP(column, Int);
    GET_PROP(endLine, Int);
    GET_PROP(endColumn, Int);
    READ_OBJ(source);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem BreakpointEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    JSONItem body = JSONItem::createObject("body");
    json.add(body);
    body.add("reason", reason);
    body.add(breakpoint.To("breakpoint"));
    return json;
}

void BreakpointEvent::From(const JSONItem& json)
{
    Event::From(json);
    JSONItem body = json.property("body");
    reason = body.property("reason").toString();
    breakpoint.From(body.property("breakpoint"));
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ProcessEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    JSONItem body = JSONItem::createObject("body");
    json.add(body);
    body.add("name", name);
    body.add("systemProcessId", systemProcessId);
    body.add("isLocalProcess", isLocalProcess);
    body.add("startMethod", startMethod);
    body.add("pointerSize", pointerSize);
    return json;
}

void ProcessEvent::From(const JSONItem& json)
{
    Event::From(json);
    JSONItem body = json.property("body");
    name = body.property("name").toString();
    systemProcessId = body.property("systemProcessId").toInt();
    isLocalProcess = body.property("isLocalProcess").toBool(true);
    startMethod = body.property("startMethod").toString();
    pointerSize = body.property("pointerSize").toInt();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem InitializeRequestArguments::To(const string& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.add("clientID", clientID);
    json.add("clientName", clientName);
    json.add("adapterID", adapterID);
    json.add("locale", locale);
    json.add("linesStartAt1", linesStartAt1);
    json.add("columnsStartAt1", columnsStartAt1);
    json.add("pathFormat", pathFormat);
    return json;
}

void InitializeRequestArguments::From(const JSONItem& json)
{
    clientID = json.property("clientID").toString();
    clientName = json.property("clientName").toString();
    adapterID = json.property("adapterID").toString();
    locale = json.property("locale").toString();
    linesStartAt1 = json.property("linesStartAt1").toBool();
    columnsStartAt1 = json.property("columnsStartAt1").toBool();
    pathFormat = json.property("pathFormat").toString();
}
// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem InitializeRequest::To(const string& name) const
{
    JSONItem json = Request::To(name);
    json.add(arguments.To("arguments"));
    return json;
}

void InitializeRequest::From(const JSONItem& json)
{
    Request::From(json);
    arguments.From(json.property("arguments"));
}
// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
JSONItem InitializeResponse::To(const string& name) const
{
    JSONItem json = Response::To(name);
    JSONItem body = json.createObject("body");
    json.add(body);
    return json;
}

void InitializeResponse::From(const JSONItem& json) { Response::From(json); }

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ConfigurationDoneRequest::To(const string& name) const
{
    JSONItem json = Request::To(name);
    return json;
}

void ConfigurationDoneRequest::From(const JSONItem& json) { Request::From(json); }

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
JSONItem EmptyAckResponse::To(const string& name) const
{
    JSONItem json = Response::To(name);
    return json;
}

void EmptyAckResponse::From(const JSONItem& json) { Response::From(json); }

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
JSONItem LaunchRequestArguments::To(const string& name) const
{
    CREATE_JSON();
    ADD_PROP(noDebug);
    ADD_PROP(debuggee);
    ADD_PROP(debugger);
    ADD_PROP(stopAtMain);
    return json;
}

void LaunchRequestArguments::From(const JSONItem& json)
{
    GET_PROP(noDebug, Bool);
    GET_PROP(debuggee, StringArray);
    GET_PROP(debugger, String);
    GET_PROP(stopAtMain, Bool);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem LaunchRequest::To(const string& name) const
{
    JSONItem json = Request::To(name);
    json.add(arguments.To("arguments"));
    return json;
}

void LaunchRequest::From(const JSONItem& json)
{
    Request::From(json);
    arguments.From(json.property("arguments"));
}
// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem DisconnectRequest::To(const string& name) const
{
    JSONItem json = Request::To(name);
    JSONItem arguments = json.createObject("arguments");
    json.add(arguments);
    arguments.add("restart", restart);
    arguments.add("terminateDebuggee", terminateDebuggee);
    return json;
}

void DisconnectRequest::From(const JSONItem& json)
{
    Request::From(json);
    JSONItem arguments = json.property("arguments");
    restart = arguments.property("restart").toBool();
    terminateDebuggee = arguments.property("terminateDebuggee").toBool(terminateDebuggee);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem BreakpointLocationsRequest::To(const string& name) const
{
    JSONItem json = Request::To(name);
    json.add(arguments.To("arguments"));
    return json;
}

void BreakpointLocationsRequest::From(const JSONItem& json)
{
    Request::From(json);
    arguments.From(json.property("arguments"));
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem BreakpointLocationsArguments::To(const string& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.add(source.To("source"));
    json.add("line", line);
    json.add("column", column);
    json.add("endLine", endLine);
    json.add("endColumn", endColumn);
    return json;
}

void BreakpointLocationsArguments::From(const JSONItem& json)
{
    source.From(json.property("source"));
    line = json.property("restart").toInt(line);
    column = json.property("column").toInt(column);
    column = json.property("column").toInt(column);
    endColumn = json.property("endColumn").toInt(endColumn);
}
// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem BreakpointLocation::To(const string& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.add("line", line);
    json.add("column", column);
    json.add("endLine", endLine);
    json.add("endColumn", endColumn);
    return json;
}

void BreakpointLocation::From(const JSONItem& json)
{
    line = json.property("restart").toInt(line);
    column = json.property("column").toInt(column);
    column = json.property("column").toInt(column);
    endColumn = json.property("endColumn").toInt(endColumn);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem BreakpointLocationsResponse::To(const string& name) const
{
    RESPONSE_TO();
    ADD_BODY();
    // create arr
    ADD_ARRAY(body, "breakpoints");
    for(const auto& b : breakpoints) {
        arr.arrayAppend(b.To());
    }
    return json;
}

void BreakpointLocationsResponse::From(const JSONItem& json)
{
    Response::From(json);
    JSONItem body = json.property("body");
    JSONItem arr = body.property("breakpoints");
    breakpoints.clear();
    int size = arr.arraySize();
    for(int i = 0; i < size; ++i) {
        BreakpointLocation loc;
        loc.From(arr.arrayItem(i));
        breakpoints.push_back(loc);
    }
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem SourceBreakpoint::To(const string& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.add("line", line);
    json.add("condition", condition);
    return json;
}

void SourceBreakpoint::From(const JSONItem& json)
{
    line = json.property("line").toInt(line);
    condition = json.property("condition").toString(condition);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem SetBreakpointsArguments::To(const string& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.add(source.To("source"));

    JSONItem arr = json.createArray("breakpoints");
    json.add(arr);
    for(const auto& sb : breakpoints) {
        arr.arrayAppend(sb.To());
    }
    return json;
}

void SetBreakpointsArguments::From(const JSONItem& json)
{
    source.From(json.property("source"));
    breakpoints.clear();
    JSONItem arr = json.property("breakpoints");
    int size = arr.arraySize();
    for(int i = 0; i < size; ++i) {
        SourceBreakpoint sb;
        sb.From(arr.arrayItem(i));
        breakpoints.push_back(sb);
    }
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem SetBreakpointsRequest::To(const string& name) const
{
    REQUEST_TO();
    ADD_OBJ(arguments);
    JSONItem j = json.createObject();
    j << make_pair("name", 1234);
    return json;
}

void SetBreakpointsRequest::From(const JSONItem& json)
{
    REQUEST_FROM();
    READ_OBJ(arguments);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ContinueArguments::To(const string& name) const
{
    CREATE_JSON();
    ADD_PROP(threadId);
    return json;
}

void ContinueArguments::From(const JSONItem& json) { GET_PROP(threadId, Int); }

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ContinueRequest::To(const string& name) const
{
    REQUEST_TO();
    ADD_OBJ(arguments);
    return json;
}

void ContinueRequest::From(const JSONItem& json)
{
    REQUEST_FROM();
    READ_OBJ(arguments);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ContinueResponse::To(const string& name) const
{
    RESPONSE_TO();
    ADD_BODY();
    ADD_BODY_PROP(allThreadsContinued);
    return json;
}

void ContinueResponse::From(const JSONItem& json)
{
    RESPONSE_FROM();
    READ_BODY();
    GET_BODY_PROP(allThreadsContinued, Int);
}

}; // namespace dap
