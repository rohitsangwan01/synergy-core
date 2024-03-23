cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(hello_library VERSION 1.0.0 LANGUAGES C)

set(sources
    hello.c
    synergy_flutter.cpp
)

list(APPEND sources ${arch_sources})
list(APPEND headers ${arch_headers})

if (SYNERGY_ADD_HEADERS)
    list(APPEND sources ${headers})
endif()

add_executable(hello_test ${sources})
add_library(hello_library SHARED hello.c hello.def)

target_link_libraries(hello_test
    arch base client common io mt net ipc platform server synlib ${libs})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    install (TARGETS hello_test DESTINATION ${SYNERGY_BUNDLE_BINARY_DIR})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install (TARGETS hello_test DESTINATION bin)
endif()


set_target_properties(hello_library PROPERTIES
    PUBLIC_HEADER hello.h
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME "hello"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Hex_Identity_ID_Goes_Here"
)




