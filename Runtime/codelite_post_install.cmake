message(">>> INSTALL DIR IS: ${INSTALL_DIR}")
message(">>> Running: ${SOURCE_DIR}/Runtime/codelite-fix-rpath ${INSTALL_DIR}")
execute_process(COMMAND ${INSTALL_DIR}/codelite-fix-rpath ${INSTALL_DIR} 
                WORKING_DIRECTORY ${INSTALL_DIR}
                OUTPUT_VARIABLE DUMMY_VAR)
