cmake_minimum_required( VERSION 3.0 )

project( MEMSERIAL VERSION 1.0.0 LANGUAGES CXX )
set( CMAKE_CXX_STANDARD 14 )

add_library( memserial SHARED src/serial_precompile.cpp )

find_package( Qt5 QUIET COMPONENTS Core )
if( TARGET Qt5::Core )
    target_link_libraries( memserial PUBLIC Qt5::Core )
    target_compile_definitions( memserial PUBLIC -DUSE_QT )
endif()

set_target_properties( memserial PROPERTIES 
    LINKER_LANGUAGE CXX VERSION ${PROJECT_VERSION} )
   
target_include_directories( memserial PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/detail
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty )

target_include_directories( memserial PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src )
    