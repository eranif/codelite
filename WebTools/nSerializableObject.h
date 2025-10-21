#ifndef NODEJSONSERIALISABLE_H
#define NODEJSONSERIALISABLE_H

#include "JSON.h"

#include <memory>
#include <vector>
#include <wx/string.h>

class nSerializableObject
{
public:
    using Ptr_t = std::shared_ptr<nSerializableObject>;
    using Vec_t = std::vector<nSerializableObject::Ptr_t>;

public:
    nSerializableObject();
    virtual ~nSerializableObject();

    virtual JSONItem ToJSON(const wxString& name) const = 0;
    virtual void FromJSON(const JSONItem& json) = 0;
    template <typename T> T* To() { return reinterpret_cast<T*>(this); }
};

#endif // NODEJSONSERIALISABLE_H
