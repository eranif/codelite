#include "dap.hpp"

namespace dap
{

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ProtocolMessage::To(const string& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.add("seq", seq);
    json.add("type", type);
    return json;
}

void ProtocolMessage::From(const JSONItem& json)
{
    seq = json.property("seq").toInt(-1);
    type = json.property("type").toString();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
Request::~Request() {}

JSONItem Request::To(const string& name) const
{
    JSONItem json = ProtocolMessage::To(name);
    json.add("command", command);
    if(arguments) { json.add("arguments", arguments->To("")); }
    return json;
}

void Request::From(const JSONItem& json)
{
    ProtocolMessage::From(json);
    command = json.property("command").toString();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem CancelRequest::To(const string& name) const
{
    JSONItem json = Request::To(name);
    JSONItem arguments = JSONItem::createObject("arguments");
    json.append(arguments);
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
    if(body) { json.append(body->To("body")); }
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
    JSONItem json = Event::To(name);
    JSONItem body = JSONItem::createObject("body");
    json.append(body);
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
    json.append(body);
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
    json.append(body);
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
    json.append(body);
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
    json.append(body);
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
    JSONItem json = JSONItem::createObject(name);
    json.add("name", this->name);
    json.add("path", path);
    return json;
}

void Source::From(const JSONItem& json)
{
    this->name = json.property("name").toString();
    path = json.property("path").toString();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem Breakpoint::To(const string& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.add("id", id);
    json.add("verified", verified);
    json.add("message", message);
    json.append(source.To("source"));
    json.add("line", line);
    json.add("column", column);
    json.add("endLine", endLine);
    json.add("endColumn", endColumn);
    return json;
}

void Breakpoint::From(const JSONItem& json)
{
    id = json.property("id").toInt();
    verified = json.property("verified").toBool();
    message = json.property("message").toString();
    source.From(json.property("source"));
    line = json.property("line").toInt();
    column = json.property("column").toInt();
    endLine = json.property("endLine").toInt();
    endColumn = json.property("endColumn").toInt();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem BreakpointEvent::To(const string& name) const
{
    JSONItem json = Event::To(name);
    JSONItem body = JSONItem::createObject("body");
    json.append(body);
    body.add("reason", reason);
    body.append(breakpoint.To("breakpoint"));
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
    json.append(body);
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
    json.append(arguments.To("arguments"));
    return json;
}

void InitializeRequest::From(const JSONItem& json)
{
    Request::From(json);
    arguments.From(json.property("arguments"));
}

}; // namespace dap
