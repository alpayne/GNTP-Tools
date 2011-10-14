ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -pedantic -I headers

nodist_include_HEADERS = headers/growl.h

lib_LTLIBRARIES = libgrowl.la 

# libgrowl.la
libgrowl_la_CFLAGS = $(AM_CFLAGS) -Wno-format-zero-length
libgrowl_la_SOURCES = source/growl.c \
					  source/tcp.c \
					  source/md5.c

bin_PROGRAMS = gntp-send
noinst_HEADERS = headers/growl.h \
				 headers/md5.h \
				 headers/tcp.h

# gntp-notify
gntp_send_CFLAGS = $(AM_CFLAGS)
gntp_send_SOURCES = source/gntp-notify.c
gntp_send_LDADD = libgrowl.la

# gntp-register
gntp_send_CFLAGS = $(AM_CFLAGS)
gntp_send_SOURCES = source/gntp-register.c
gntp_send_LDADD = libgrowl.la

# vim: noexpandtab sw=4 ts=4
