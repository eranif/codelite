#ifndef PHP_HELPERS_H
#define PHP_HELPERS_H

#include <wx/string.h>
#define PHP_PREFIX_WITH_SPACE "<?php "
#define PHP_PREFIX            "<?php"

template <class T>
class TDeleter {
    T* m_token;
public:
    TDeleter(T* token) : m_token( token ) {}
    ~TDeleter() { wxDELETE(m_token); }
};

#endif //  PHP_HELPERS_H
