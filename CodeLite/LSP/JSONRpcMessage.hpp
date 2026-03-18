#pragma once

#include "JSON.h"

#include <memory>
#include <sstream>

namespace LSP
{
class JSONRpcMessage
{
public:
    JSONRpcMessage(JSONItem json)
        : m_json(std::move(json))
    {
    }
    ~JSONRpcMessage() = default;

    /**
     * @brief Serializes the request into an HTTP message string.
     *
     * @details This method formats the underlying JSON payload into its raw string
     * representation, computes its byte length, and prepends the HTTP headers
     * required to describe the body. It returns the complete request text with a
     * Content-Length header, a JSON Content-Type header, and the serialized body
     * appended after a blank line.
     *
     * @return std::string The full HTTP request message, including headers and
     * body.
     */
    inline std::string ToString() const
    {
        using cstr_ptr = std::unique_ptr<char, decltype(&std::free)>;
        cstr_ptr data(m_json.FormatRawString(false), &std::free);

        std::string s;
        size_t len = strlen(data.get());

        // Build the request header
        std::stringstream ss;
        ss << "Content-Length: " << len << "\r\n";
        ss << "Content-Type: application/json; charset=utf-8" << "\r\n";
        ss << "\r\n";
        s = ss.str();

        // append the data
        s.append(data.get(), len);
        return s;
    }

private:
    JSONItem m_json;
};

} // namespace LSP
