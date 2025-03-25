# CPM - package manager
include(CPM)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/glad)
set(glad_SOURCE_DIR ${GLAD_DIR}/include CACHE INTERNAL "")
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)

# stb_image
set(STB_IMAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_image)
set(stb_image_SOURCE_DIR ${STB_IMAGE_DIR} CACHE INTERNAL "")
add_library(stb_image STATIC ${STB_IMAGE_DIR}/stb_image.cpp)
target_include_directories(stb_image PRIVATE ${STB_IMAGE_DIR})

# SoLoud
set(SOLOUD_DIR ${CMAKE_CURRENT_LIST_DIR}/soloud)
set(SOLOUD_INCLUDE_DIR ${SOLOUD_DIR}/include)
set(SOLOUD_SOURCE_DIR ${SOLOUD_DIR}/src)
file(GLOB SOLOUD_SOURCES
        ${SOLOUD_SOURCE_DIR}/audiosource/wav/*.cpp
        ${SOLOUD_SOURCE_DIR}/audiosource/wav/*.c
        ${SOLOUD_SOURCE_DIR}/backend/miniaudio/*.cpp
        ${SOLOUD_SOURCE_DIR}/c_api/*.cpp
        ${SOLOUD_SOURCE_DIR}/core/*.cpp
        ${SOLOUD_SOURCE_DIR}/filter/*.cpp
        ${SOLOUD_SOURCE_DIR}/tools/*.cpp
)
add_library(soloud STATIC ${SOLOUD_SOURCES})
target_include_directories(soloud PUBLIC ${SOLOUD_INCLUDE_DIR})
target_compile_definitions(soloud PUBLIC WITH_MINIAUDIO)

# other
CPMAddPackage("gh:assimp/assimp@5.2.5")
CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage("gh:gabime/spdlog@1.10.0")
						 
set_target_properties(glad
                      stb_image 
                      assimp 
                      glfw 
                      glm
                      spdlog
                      soloud PROPERTIES FOLDER "thirdparty")

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()