find_package(SAMRAI CONFIG QUIET)
if (NOT SAMRAI_FOUND)

  if(DEFINED SAMRAI_ROOT)
    find_package(SAMRAI PATHS ${SAMRAI_ROOT} REQUIRED)
  else()
    set(SAMRAI_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/subprojects/samrai)
    set(SAMRAI_BIN ${CMAKE_CURRENT_BINARY_DIR}/subprojects/samrai)

    if (NOT EXISTS ${SAMRAI_SRCDIR})
      execute_process(
        COMMAND ${Git} clone https://github.com/LLNL/SAMRAI ${SAMRAI_SRCDIR} -b master --recursive --depth 10
        )
    endif()

    option(ENABLE_TESTS "Enable Samrai Test" OFF ) # disable SAMRAI Test so that we can use the googletest pulled after

    add_subdirectory(${SAMRAI_SRCDIR})
    include_directories(${CMAKE_BINARY_DIR}/include) # this is needed to find build-dir/include/SAMRAI/SAMRAI_config.h
  endif()

endif()
