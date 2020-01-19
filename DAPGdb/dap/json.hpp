#ifndef JSON_HPP
#define JSON_HPP

#include "cJSON.hpp"
#include <vector>

using namespace std;
namespace dap
{
class JSONItem
{
protected:
    cJSON* m_json = nullptr;
    cJSON* m_walker = nullptr;
    string m_name;
    int m_type = -1;

    // Values
    string m_valueString;
    double m_valueNumer = 0;

public:
    JSONItem(cJSON* json);
    JSONItem(const string& name, double val);
    JSONItem(const string& name, const string& val);
    JSONItem(const string& name, const char* pval, size_t len);
    JSONItem(const string& name, bool val);
    virtual ~JSONItem() {}

    // Walkers
    JSONItem firstChild();
    JSONItem nextChild();

    void SetValueNumer(double valueNumer) { this->m_valueNumer = valueNumer; }
    double GetValueNumer() const { return m_valueNumer; }

    void SetValueString(const string& valueString) { this->m_valueString = valueString; }
    const string& GetValueString() const { return m_valueString; }

    // Setters
    ////////////////////////////////////////////////
    void setName(const string& m_name) { this->m_name = m_name; }
    void setType(int m_type) { this->m_type = m_type; }
    int getType() const { return m_type; }
    string getName() const { return m_name; }

    // Readers
    ////////////////////////////////////////////////
    JSONItem property(const string& name) const;
    bool hasProperty(const string& name) const;

    bool toBool(bool defaultValue = false) const;
    string toString(const string& defaultValue = "") const;
    vector<string> toStringArray(const vector<string>& defaultValue = {}) const;
    JSONItem arrayItem(int pos) const;

    // Retuen the object type
    bool isNull() const;
    bool isBool() const;
    bool isString() const;
    bool isNumber() const;
    bool isArray() const;
    bool isObject() const;

    string Format(bool formatted = true) const;
    /**
     * @brief format the JSON into a raw c string
     * The caller should free the pointer (using free())
     */
    char* FormatRawString(bool formatted = true) const;
    int arraySize() const;
    int toInt(int defaultVal = -1) const;
    size_t toSize_t(size_t defaultVal = 0) const;
    double toDouble(double defaultVal = -1.0) const;

    // Writers
    ////////////////////////////////////////////////
    /**
     * @brief create new named object and append it to this json element
     * @return the newly created object
     */
    static JSONItem createObject(const string& name = "");
    /**
     * @brief create new named array and append it to this json element
     * @return the newly created array
     */
    static JSONItem createArray(const string& name = "");

    /**
     * @brief append new element to this json element
     */
    void append(const JSONItem& element);

    JSONItem& add(const JSONItem& item)
    {
        append(item);
        return *this;
    }
    JSONItem& add(const string& name, const string& value);
    JSONItem& add(const string& name, int value) { return add(name, (long)value); }
    JSONItem& add(const string& name, long value);
    JSONItem& add(const string& name, size_t value);
    JSONItem& add(const string& name, bool value);
    JSONItem& add(const string& name, const JSONItem& element);
    JSONItem& add(const string& name, const char* value);
    JSONItem& add(const string& name, const vector<string>& arr);

    /**
     * @brief delete property by name
     */
    void removeProperty(const string& name);

    /**
     * @brief detach element from json. Return the detached element
     */
    JSONItem detachProperty(const string& name);

    //////////////////////////////////////////////////
    // Array operations
    //////////////////////////////////////////////////

    /**
     * @brief append new number
     * @return the newly added property
     */
    void arrayAppend(const JSONItem& element);
    void arrayAppend(const string& value);

    bool isOk() const { return m_json != NULL; }

    template <typename T> JSONItem& operator<<(const pair<string, T>& p) { return add(p.first, p.second); }

    template <typename T> JSONItem& operator<<(const pair<const char*, T>& p) { return add(p.first, p.second); }

    /**
     * @brief release the internal pointer
     */
    cJSON* release()
    {
        cJSON* temp = m_json;
        m_json = nullptr;
        return temp;
    }
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class JSON
{
protected:
    cJSON* m_json;
    string _errorString;

public:
    JSON(int type);
    JSON(const string& text);
    JSON(JSONItem item);
    JSON(cJSON* json);
    virtual ~JSON();

    string errorString() const;
    bool isOk() const { return m_json != NULL; }

    JSONItem toElement() const;

    void clear();
    cJSON* release();

private:
    // Make this class not copyable
    JSON(const JSON& src);
    JSON& operator=(const JSON& src);
};

};     // namespace dap
#endif // JSON_HPP
