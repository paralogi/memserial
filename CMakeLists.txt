cmake_minimum_required( VERSION 3.0 )

project( MEMSERIAL VERSION 1.0.0 LANGUAGES CXX )
set( CMAKE_CXX_STANDARD 14 )

add_library( memserial SHARED 
    src/serial_precompile.cpp )

set_target_properties( memserial PROPERTIES 
    LINKER_LANGUAGE CXX 
    VERSION ${PROJECT_VERSION} )
    
target_compile_definitions( memserial PRIVATE 
    -DDISABLE_STATIC_HASH )
    
target_include_directories( memserial PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty )
    
find_package( Qt5 QUIET COMPONENTS Core )
if( TARGET Qt5::Core )
    target_link_libraries( memserial PUBLIC Qt5::Core )
endif()
