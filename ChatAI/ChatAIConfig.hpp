#pragma once

#include "cl_config.h"

class ChatAIConfig : clConfigItem
{
public:
    ChatAIConfig();
    virtual ~ChatAIConfig();

    struct Model {
        wxString m_name;
        wxString m_modelFile;

        Model(const wxString& name, const wxString& file)
            : m_name(name)
            , m_modelFile(file)
        {
        }
        Model() = default;
        void Clear()
        {
            m_name.clear();
            m_modelFile.clear();
        }

        bool IsOk() const { return !m_name.empty() && ::wxFileExists(m_modelFile); }

        JSONItem ToJSON() const
        {
            auto item = JSONItem::createObject();
            item.addProperty("name", m_name).addProperty("file", m_modelFile);
            return item;
        }

        void FromJSON(const JSONItem& json)
        {
            m_name = json.namedObject("name").toString();
            m_modelFile = json.namedObject("file").toString();
        }
    };

    void SetLlamaCli(const wxString& llamCli) { this->m_llamaCli = llamCli; }
    void SetModels(const std::vector<std::shared_ptr<Model>>& models) { this->m_models = models; }
    void SetSelectedModelName(const wxString& selectedModel);
    bool ContainsModel(const wxString& modelName) const;
    const wxString& GetLlamaCli() const { return m_llamaCli; }
    std::vector<std::shared_ptr<Model>> GetModels() const { return m_models; }
    std::shared_ptr<Model> GetSelectedModel() const { return m_selectedModel; }

    void Load();
    void Save();

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

private:
    wxString m_llamaCli;
    std::vector<std::shared_ptr<Model>> m_models;
    std::shared_ptr<Model> m_selectedModel;
};
