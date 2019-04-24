#include "HelpPluginSettings.h"

HelpPluginSettings::HelpPluginSettings()
    : clConfigItem("HelpPlugin")
    , m_cxxDocset("cpp,net,boost,qt 4,qt 5,cvcpp,cocos2dx,c,manpages")
    , m_phpDocset("php,wordpress,drupal,zend,laravel,yii,joomla,ee,codeigniter,cakephp,phpunit,symfony,typo3,twig,"
                  "smarty,phpp,html,statamic,mysql,sqlite,mongodb,psql,redis,zend framework 1,zend framework 2")
    , m_htmlDocset(
          "html,svg,css,bootstrap,less,foundation,awesome,statamic,javascript,jquery,jqueryui,jquerym,angularjs,"
          "backbone,marionette,meteor,moo,prototype,ember,lodash,underscore,sencha,extjs,knockout,zepto,"
          "cordova,phonegap,yui")
    , m_cmakeDocset("cmake")
    , m_cssDocset(m_htmlDocset)
    , m_jsDocset(m_htmlDocset)
    , m_javaDocset("java,javafx,grails,groovy,playjava,spring,cvj,processing")
{
}

HelpPluginSettings::~HelpPluginSettings() {}

HelpPluginSettings& HelpPluginSettings::Load()
{
    clConfig conf("help-plugin.conf");
    conf.ReadItem(this);
    return *this;
}

HelpPluginSettings& HelpPluginSettings::Save()
{
    clConfig conf("help-plugin.conf");
    conf.WriteItem(this);
    return *this;
}
void HelpPluginSettings::FromJSON(const JSONItem& json)
{
    m_cxxDocset = json.namedObject("m_cxxDocset").toString(m_cxxDocset);
    m_phpDocset = json.namedObject("m_phpDocset").toString(m_phpDocset);
    m_htmlDocset = json.namedObject("m_htmlDocset").toString(m_htmlDocset);
    m_cmakeDocset = json.namedObject("m_cmakeDocset").toString(m_cmakeDocset);
    m_cssDocset = json.namedObject("m_cssDocset").toString(m_cssDocset);
    m_jsDocset = json.namedObject("m_jsDocset").toString(m_jsDocset);
    m_javaDocset = json.namedObject("m_javaDocset").toString(m_javaDocset);
}

JSONItem HelpPluginSettings::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
    json.addProperty("m_cxxDocset", m_cxxDocset);
    json.addProperty("m_phpDocset", m_phpDocset);
    json.addProperty("m_htmlDocset", m_htmlDocset);
    json.addProperty("m_cmakeDocset", m_cmakeDocset);
    json.addProperty("m_cssDocset", m_cssDocset);
    json.addProperty("m_jsDocset", m_jsDocset);
    json.addProperty("m_javaDocset", m_javaDocset);
    return json;
}
