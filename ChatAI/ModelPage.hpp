#ifndef MODELPAGE_HPP
#define MODELPAGE_HPP

#include "ChatAIConfig.hpp"
#include "UI.hpp"

class ModelPage : public ModelPageBase
{
public:
    ModelPage(wxWindow* parent, std::shared_ptr<ChatAIConfig::Model> model);
    virtual ~ModelPage();
    void Save();
    std::shared_ptr<ChatAIConfig::Model> GetModel() const { return m_model; }

private:
    std::shared_ptr<ChatAIConfig::Model> m_model;
};
#endif // MODELPAGE_HPP
