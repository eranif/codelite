#ifndef FORMATTERPAGE_HPP
#define FORMATTERPAGE_HPP

#include "GenericFormatter.hpp"
#include "clPropertiesPage.hpp"
#include "codelite_exports.h"

class FormatterPage : public clPropertiesPage
{
    std::shared_ptr<GenericFormatter> m_cur_formatter;

public:
    FormatterPage(wxWindow* parent);
    virtual ~FormatterPage();

    void Clear() override;
    void Load(std::shared_ptr<GenericFormatter> formatter);
};

#endif // FORMATTERPAGE_HPP
