cmake_minimum_required( VERSION 2.6 )

include_directories ( ./headers )

if( WIN32 )
  link_libraries ( ws2_32 )
endif( WIN32 )

set( GROWL_NTP_C_SRC
        source/growl.c
        source/tcp.c
        source/md5.c )

add_library( growl STATIC  ${GROWL_NTP_C_SRC} )
set_target_properties( growl PROPERTIES COMPILE_FLAGS "" )

add_executable( gntp-notify source/gntp-notify.c )
target_link_libraries ( gntp-notify growl)

add_executable( gntp-register source/gntp-register.c )
target_link_libraries ( gntp-register growl)

