/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#include "memchecksettings.h"

ValgrindSettings::ValgrindSettings():clConfigItem(CONFIG_ITEM_NAME_VALGRIND),
    m_binary(VALGRIND_BINARY),
    m_outputInPrivateFolder(VALGRIND_OUTPUT_IN_PRIVATE_FOLDER),
    m_outputFile(VALGRIND_OUTPUT_FILE),
    m_mandatoryOptions(VALGRIND_MANDATORY_OPTIONS),
    m_outputFileOption(VALGRIND_OUTPUT_FILE_OPTION),
    m_suppressionFileOption(VALGRIND_SUPPRESSION_FILE_OPTION),
    m_options(VALGRIND_OPTIONS),
    m_suppFileInPrivateFolder(VALGRIND_SUPP_FILE_IN_PRIVATE_FOLDER),
    m_suppFiles()
{
}


void ValgrindSettings::FromJSON(const JSONItem & json)
{
    if (json.hasNamedObject("m_binary")) m_binary = json.namedObject("m_binary").toString();
    if (json.hasNamedObject("m_outputInPrivateFolder")) m_outputInPrivateFolder = json.namedObject("m_outputInPrivateFolder").toBool();
    if (json.hasNamedObject("m_outputFile")) m_outputFile = json.namedObject("m_outputFile").toString();
    if (json.hasNamedObject("m_mandatoryOptions")) m_mandatoryOptions = json.namedObject("m_mandatoryOptions").toString();
    if (json.hasNamedObject("m_outputFileOption")) m_outputFileOption = json.namedObject("m_outputFileOption").toString();
    if (json.hasNamedObject("m_suppressionFileOption")) m_suppressionFileOption = json.namedObject("m_suppressionFileOption").toString();
    if (json.hasNamedObject("m_options")) m_options = json.namedObject("m_options").toString();
    if (json.hasNamedObject("m_suppFileInPrivateFolder")) m_suppFileInPrivateFolder = json.namedObject("m_suppFileInPrivateFolder").toBool();
    if (json.hasNamedObject("m_suppFiles")) m_suppFiles = json.namedObject("m_suppFiles").toArrayString();
}

JSONItem ValgrindSettings::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_binary", m_binary);
    element.addProperty("m_outputInPrivateFolder", m_outputInPrivateFolder);
    element.addProperty("m_outputFile", m_outputFile);
    element.addProperty("m_mandatoryOptions", m_mandatoryOptions);
    element.addProperty("m_outputFileOption", m_outputFileOption);
    element.addProperty("m_suppressionFileOption", m_suppressionFileOption);
    element.addProperty("m_options", m_options);
    element.addProperty("m_suppFileInPrivateFolder", m_suppFileInPrivateFolder);
    element.addProperty("m_suppFiles", m_suppFiles);
    return element;
}



MemCheckSettings::MemCheckSettings():clConfigItem(CONFIG_ITEM_NAME_MEMCHECK),
    m_engine(CONFIG_ITEM_NAME_VALGRIND),
    m_availableEngines(),
    m_result_page_size(RESULT_PAGE_SIZE),
    m_result_page_size_max(RESULT_PAGE_SIZE_MAX),
    m_omitNonWorkspace(OMIT_NONWORKSPACE),
    m_omitDuplications(OMIT_DUPLICATIONS),
    m_omitSuppressed(OMIT_SUPPRESSED)
{
    m_availableEngines.Add(CONFIG_ITEM_NAME_VALGRIND);
}


void MemCheckSettings::FromJSON(const JSONItem & json)
{
    if (json.hasNamedObject("m_engine")) m_engine = json.namedObject("m_engine").toString();
    if (json.hasNamedObject("m_result_page_size")) m_result_page_size = json.namedObject("m_result_page_size").toSize_t();
    if (json.hasNamedObject("m_result_page_size_max")) m_result_page_size_max = json.namedObject("m_result_page_size_max").toSize_t();
    if (json.hasNamedObject("m_omitNonWorkspace")) m_omitNonWorkspace = json.namedObject("m_omitNonWorkspace").toBool();
    if (json.hasNamedObject("m_omitDuplications")) m_omitDuplications = json.namedObject("m_omitDuplications").toBool();
    if (json.hasNamedObject("m_omitSuppressed")) m_omitSuppressed = json.namedObject("m_omitSuppressed").toBool();

    m_valgrindSettings.FromJSON(json.namedObject(CONFIG_ITEM_NAME_VALGRIND));
}

JSONItem MemCheckSettings::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_engine", m_engine);
    element.addProperty("m_result_page_size", m_result_page_size);
    element.addProperty("m_result_page_size_max", m_result_page_size_max);
    element.addProperty("m_omitNonWorkspace", m_omitNonWorkspace);
    element.addProperty("m_omitDuplications", m_omitDuplications);
    element.addProperty("m_omitSuppressed", m_omitSuppressed);

    element.append(m_valgrindSettings.ToJSON());
    return element;
}
