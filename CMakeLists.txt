project(PhotoBook)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/)

find_package(KDE4 REQUIRED)
find_package(Xapian REQUIRED)
find_package(Kexiv2)

include(KDE4Defaults)
include(MacroLibrary)

set(PHOTOBOOK_VERSION "0.0.1")

# Generate config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_definitions(${QT_DEFINITIONS}
    ${KDE_DEFINITIONS}
    ${KEXIV2_DEFINITIONS} -fexceptions -fpermissive
)

include_directories(${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${KDE4_INCLUDES}
    ${XAPIAN_INCLUDE_DIR}
    ${KEXIV2_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/qmlapplicationviewer)

add_subdirectory(src)
add_subdirectory(qml)
