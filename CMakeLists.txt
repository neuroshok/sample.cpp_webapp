cmake_minimum_required(VERSION 3.13)
project(webapp)

set(CMAKE_CXX_STANDARD 17)

set(WEBAPP_ROOT ${CMAKE_SOURCE_DIR})

#-------------------------------------------------------
#                       config
#-------------------------------------------------------
if (QT_ROOT)
    set(CMAKE_PREFIX_PATH ${QT_ROOT})
    set(CMAKE_FIND_PATH ${QT_ROOT})
    message(STATUS "QT_ROOT: ${QT_ROOT}")
endif()

if (WEBAPP_CLIENT)
    set(Qt5_DIR ${QT_ROOT}/lib/cmake/Qt5)
    set(Qt5Core_DIR ${QT_ROOT}/lib/cmake/Qt5Core)
    set(Qt5Charts_DIR ${QT_ROOT}/lib/cmake/Qt5Charts)
    set(Qt5Widgets_DIR ${QT_ROOT}/lib/cmake/Qt5Widgets)
    set(Qt5Gui_DIR ${QT_ROOT}/lib/cmake/Qt5Gui)
    set(Qt5Svg_DIR ${QT_ROOT}/lib/cmake/Qt5Svg)
    set(Qt5Network_DIR ${QT_ROOT}/lib/cmake/Qt5Network)
    set(Qt5WebSockets_DIR ${QT_ROOT}/lib/cmake/Qt5WebSockets)
    set(Qt5Zlib_DIR ${QT_ROOT}/lib/cmake/Qt5Zlib)
    set(Qt5EventDispatcherSupport_DIR ${QT_ROOT}/lib/cmake/Qt5EventDispatcherSupport)
    set(Qt5FontDatabaseSupport_DIR ${QT_ROOT}/lib/cmake/Qt5FontDatabaseSupport)
    set(Qt5EglSupport_DIR ${QT_ROOT}/lib/cmake/Qt5EglSupport)
endif()

#-------------------------------------------------------
#                       sources
#-------------------------------------------------------
# server
file(GLOB_RECURSE WEBAPP_SERVER_SOURCES
        "server/include/controller/*.hpp"
        "server/source/controller/*.cpp"
        "server/source/*.cpp"
        "server/resource/data/style.css")

# client
file(GLOB_RECURSE WEBAPP_CLIENT_SOURCES
        "client/include/*.hpp"
        "client/source/*.cpp")

#-------------------------------------------------------
#                       targets
#-------------------------------------------------------
if (WEBAPP_SERVER)
    add_executable(webapp_server ${WEBAPP_SERVER_SOURCES})
    target_include_directories(webapp_server PUBLIC server/include)

    if (NOT WEBAPP_OUTPUT)
        set(WEBAPP_OUTPUT ${CMAKE_BINARY_DIR}/output)
    endif()

    set_target_properties(webapp_server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${WEBAPP_OUTPUT}
    )
endif()

if (WEBAPP_CLIENT)
    add_executable(webapp_client ${WEBAPP_CLIENT_SOURCES})
    target_include_directories(webapp_client PUBLIC client/include)
endif()

#-------------------------------------------------------
#                    dependencies
#-------------------------------------------------------
# drogon
if (WEBAPP_SERVER)
    set(BUILD_EXAMPLES OFF)
    add_subdirectory(third_party/drogon)
    add_dependencies(webapp_server drogon_ctl)
endif()

# qt
if (WEBAPP_CLIENT)
    find_package(Qt5 COMPONENTS Core Widgets WebSockets REQUIRED)
    set(QT_LIBS Qt5::Core Qt5::Widgets Qt5::WebSockets)
endif()

#-------------------------------------------------------
#                        link
#-------------------------------------------------------
if (WEBAPP_SERVER)
    drogon_create_views(webapp_server ${WEBAPP_ROOT}/view/ ${WEBAPP_ROOT}/source/generated/view true)
    target_link_libraries(webapp_server PRIVATE drogon)
endif()

if (WEBAPP_CLIENT)
    target_link_libraries(webapp_client PUBLIC ${QT_LIBS})
    target_link_options(webapp_client PRIVATE --bind -s EXIT_RUNTIME=1)
    set_target_properties(webapp_client PROPERTIES LINK_FLAGS "-s WASM=1 -s FULL_ES2=1 -s FULL_ES3=1 -s USE_WEBGL2=1 -s EXIT_RUNTIME=1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=[\"UTF16ToString\",\"stringToUTF16\"] --bind -s FETCH=1 -O2 -s ALLOW_MEMORY_GROWTH=1" )
endif()

#-------------------------------------------------------
#                       deploy
#-------------------------------------------------------
if (WEBAPP_CLIENT)
    set_target_properties(webapp_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${WEBAPP_ROOT}/client/app/"
        OUTPUT_NAME "webapp_client"
        )
endif()

if (WEBAPP_SERVER)
    # copy app data
    add_custom_command(
        TARGET webapp_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${WEBAPP_ROOT}/client/app ${WEBAPP_OUTPUT}/app
        COMMENT "Copy ${WEBAPP_ROOT}/client/app to ${WEBAPP_ROOT}/app")

    # copy server resources
    add_custom_command(
        TARGET webapp_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${WEBAPP_ROOT}/server/resource ${WEBAPP_OUTPUT}/resource
        COMMENT "Copy ${WEBAPP_ROOT}/resource to ${WEBAPP_OUTPUT}/resource")
endif()