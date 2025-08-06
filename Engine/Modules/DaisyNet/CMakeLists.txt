set(DAISY_NET_SOURCES
    Source/DaisyNet.cpp
)

set(DAISY_NET_HEADERS
    Include/DaisyNet.h
)

add_library(DaisyNet STATIC ${DAISY_NET_SOURCES} ${DAISY_NET_HEADERS})

target_include_directories(DaisyNet 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(DaisyNet 
    PUBLIC 
        DaisyCore
)

if(WIN32)
    target_link_libraries(DaisyNet PRIVATE ws2_32)
endif()

source_group("Header Files" FILES ${DAISY_NET_HEADERS})
source_group("Source Files" FILES ${DAISY_NET_SOURCES})