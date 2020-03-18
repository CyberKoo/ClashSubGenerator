if (CSG_DEVELOPMENT)
    find_package(Git QUIET)
    if (GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --all --long --dirty
                ${SRC_DIR}
                OUTPUT_VARIABLE "GIT_VERSION"
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
        string(REGEX REPLACE "^heads\\/(.*)-0-(g.*)$" "\\1~\\2" GIT_VERSION "${GIT_VERSION}")
    endif ()
endif ()
