cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
project(TideEngine VERSION 0.0.1)

if (WIN32)
    message(NOTICE "\n")
    message(NOTICE "********************** IMPORTANT NOTICE ****************************\n")
    message(NOTICE "Make sure you have enabled Developer Mode in your system settings!")
    message(NOTICE "To enable Developer Mode go to Settings -> Update & Security -> For developers -> Developer Mode ON")
    message(NOTICE "\n********************************************************************")
    message(NOTICE "\n")
else ()
    message(NOTICE "\n********************** IMPORTANT NOTICE ****************************\n")
    message(FATAL_ERROR "Platform not supported.\n\n********************************************************************")
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(global_settings)

# ---- Dependencies ----
add_subdirectory(thirdparty)

# ---- Main project's files ----
add_subdirectory(src)
