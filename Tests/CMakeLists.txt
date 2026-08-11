include(CTest)

if(BUILD_TESTING)
  add_subdirectory(CodeliteTest)
  add_subdirectory(CxxParserTests)
  add_subdirectory(PluginTest)
endif()
