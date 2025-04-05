# Add source files
file(GLOB_RECURSE SOURCE_FILES
        *.c
        *.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
        *.h
        *.hpp)

# Do not include imgui in non editor builds.
if (NOT DEFINED ENV{Editor})
    list(FILTER SOURCE_FILES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/imgui_impl/*")
    list(FILTER HEADER_FILES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/imgui_impl/*")
endif ()

# Search for the assets files
file(GLOB_RECURSE ASSETS_FILES
        ${CMAKE_SOURCE_DIR}/res/*.*)

set_source_files_properties(${ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${ASSETS_FILES})

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        ${glad_SOURCE_DIR}
        ${stb_image_SOURCE_DIR}
        ${miniaudio_SOURCE_DIR})

# include imgui only in editor.
if (DEFINED ENV{Editor})
    target_include_directories(${PROJECT_NAME} AFTER PUBLIC ${imgui_SOURCE_DIR})
endif ()

if (DEFINED ENV{Editor})
    find_package(imgui CONFIG REQUIRED)
    find_package(imguizmo CONFIG REQUIRED)
endif ()

# Link libraries
find_package(rapidjson CONFIG REQUIRED)
find_package(Tracy CONFIG REQUIRED)
find_package(Freetype REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE stb_image)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE rapidjson)
target_link_libraries(${PROJECT_NAME} PRIVATE miniaudio)
target_link_libraries(${PROJECT_NAME} PRIVATE Tracy::TracyClient)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)

if (DEFINED ENV{Editor})
    target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
    target_link_libraries(${PROJECT_NAME} PRIVATE imguizmo::imguizmo)
endif ()


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res)

if (DEFINED ENV{Editor})
    target_compile_definitions(${PROJECT_NAME} PRIVATE EDITOR=1)
else ()
    target_compile_definitions(${PROJECT_NAME} PRIVATE EDITOR=0)
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)


if (MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
endif ()
