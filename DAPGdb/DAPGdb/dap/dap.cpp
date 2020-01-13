#include "dap.hpp"

namespace dap
{

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

JSONItem ProtocolMessage::To(const string& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.add("number", number);
    json.add("type", type);
    return json;
}

void ProtocolMessage::From(const JSONItem& json)
{
    number = json.property("number").toInt(-1);
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
}; // namespace dap
