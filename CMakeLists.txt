cmake_minimum_required(VERSION 3.15)
project(qhycfw3_usb)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB_RECURSE X2_INTERFACES liscensedinterfaces/*.h)
FILE(GLOB HEADERS src/*.h)
FILE(GLOB SOURCES src/*.cpp)
FILE(GLOB_RECURSE INSTALLER_SOURCES installer/*)

add_library(qhycfw3_usb SHARED ${X2_INTERFACES} ${SOURCES} ${HEADERS})
include_directories(src ${PROJECT_SOURCE_DIR})

if (APPLE)
    if (CODE_SIGNING_IDENTITY)
        add_custom_command(
            TARGET qhycfw3_usb
            POST_BUILD
            COMMAND codesign -s "${CODE_SIGNING_IDENTITY}" "$<TARGET_FILE:qhycfw3_usb>"
            VERBATIM
        )
    endif()

    if (PACKAGE_SIGNING_IDENTITY)
        add_custom_command(
            OUTPUT qhycfw3_usb.pkg
            DEPENDS $<TARGET_FILE:qhycfw3_usb> ${INSALLER_SOURCES}
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/installer/build.sh "${CMAKE_CURRENT_BINARY_DIR}" "${PACKAGE_SIGNING_IDENTITY}"
            VERBATIM
        )

        add_custom_target(
            installer
            ALL
            DEPENDS qhycfw3_usb.pkg
        )
        add_dependencies(installer qhycfw3_usb)


    endif()
endif()
