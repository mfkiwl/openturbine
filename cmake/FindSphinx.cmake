#----------------------------------------------------------------------------------------
# Finds the Sphinx documentation builder and its version
#----------------------------------------------------------------------------------------
find_program(SPHINX_EXECUTABLE NAMES sphinx-build
    DOC "Sphinx Documentation Builder (sphinx-doc.org)"
)

if(SPHINX_EXECUTABLE)
    execute_process(
        COMMAND ${SPHINX_EXECUTABLE} --version
        OUTPUT_VARIABLE SPHINX_VERSION_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if("${SPHINX_VERSION_OUTPUT}" MATCHES ".* ([^\n]+)\n")
      set(SPHINX_VERSION "${CMAKE_MATCH_1}")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sphinx
    REQUIRED_VARS SPHINX_EXECUTABLE
    VERSION_VAR SPHINX_VERSION
)

mark_as_advanced(SPHINX_EXECUTABLE)