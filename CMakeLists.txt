cmake_minimum_required( VERSION 2.6 )

include_directories ( ./headers )

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS                  "${CMAKE_CXX_FLAGS}  -Wall -Wextra -Wnon-virtual-dtor -fno-strict-aliasing")
  set(CMAKE_CXX_FLAGS_RELEASE          "-O2 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO "-g -O2 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG            "-DDEBUG -g -ggdb -fno-reorder-blocks -fno-schedule-insns -fno-inline ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUGFULL        "-g3 ${CMAKE_CXX_FLAGS_DEBUG}")
endif(CMAKE_COMPILER_IS_GNUCXX)

if( WIN32 )
  link_libraries ( ws2_32 )
endif( WIN32 )

set( GROWL_NTP_C_SRC
        source/growl.c
        source/tcp.c
        source/md5.c )

add_library( growl SHARED  ${GROWL_NTP_C_SRC} )
set_target_properties( growl PROPERTIES COMPILE_FLAGS "-DGROWL_DLL" )

if( WITH_STATIC )
  add_library( growl-static  STATIC ${GROWL_NTP_C_SRC} )
  set_target_properties( growl-static PROPERTIES COMPILE_FLAGS "-DGROWL_DLL" )
endif( WITH_STATIC )

add_executable( gntp-notify source/gntp-notify.c )
target_link_libraries ( gntp-notify growl)

add_executable( gntp-register source/gntp-register.c )
target_link_libraries ( gntp-register growl)

