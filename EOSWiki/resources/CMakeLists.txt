
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "%CMAKE_MODULE_PATH%")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

################################
## Cmake dependencies
################################

include(EosioCDTMacros)
include(eosio.cdt-config)
include(EosioWasmToolchain)

add_contract(%CONTRACT_NAME% %CONTRACT_NAME% %CONTRACT_NAME%.cpp)
