#ifndef JSONRPC_RESULTS_H
#define JSONRPC_RESULTS_H

#include "JSON.h"
#include "LSP/JSONObject.h"
#include "codelite_exports.h"

#include <memory>

namespace LSP
{
//===----------------------------------------------------------------------------------
// Result
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL Result : public Serializable
{
public:
    using Ptr_t = std::shared_ptr<Result>;

public:
    Result() = default;
    virtual ~Result() = default;
    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<Result*>(this)); }
    virtual JSONItem ToJSON(const wxString& name) const { return JSONItem(nullptr); }
};

//===----------------------------------------------------------------------------------
// ResultString
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL ResultString : public Result
{
    wxString m_text;

public:
    ResultString(const wxString& text) {}
    virtual ~ResultString() = default;
    void FromJSON(const JSONItem& json);
    ResultString& SetText(const wxString& text)
    {
        this->m_text = text;
        return *this;
    }
    const wxString& GetText() const { return m_text; }
};

//===----------------------------------------------------------------------------------
// ResultNumber
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL ResultNumber : public Result
{
    int m_number = 0;

public:
    ResultNumber(const wxString& text) {}
    virtual ~ResultNumber() = default;
    void FromJSON(const JSONItem& json);
    ResultNumber& SetNumber(int number)
    {
        this->m_number = number;
        return *this;
    }
    int GetNumber() const { return m_number; }
};

//===----------------------------------------------------------------------------------
// ResultNumber
//===----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL ResultBoolean : public Result
{
    bool m_value = false;

public:
    ResultBoolean(const wxString& text) {}
    virtual ~ResultBoolean() = default;
    void FromJSON(const JSONItem& json);

    ResultBoolean& SetValue(bool value)
    {
        this->m_value = value;
        return *this;
    }
    bool GetValue() const { return m_value; }
};
}; // namespace LSP

#endif // JSONRPC_RESULTS_H
