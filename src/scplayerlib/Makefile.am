# Process this file with automake to generate a Makefile
#
lib_LTLIBRARIES         = libscplayer.la
libscplayer_la_CFLAGS   = -I${srcdir}/Z80
libscplayer_la_CXXFLAGS = -I${srcdir}/Z80 -I${srcdir}/SAASound
libscplayer_la_SOURCES  = SCPlayer.cpp etracker_bin.h
include_HEADERS         = SCPlayer.h
libscplayer_la_SOURCES += Z80/Codes.h Z80/CodesCB.h Z80/CodesED.h \
			  Z80/CodesXCB.h Z80/CodesXX.h Z80/ConDebug.c \
			  Z80/Debug.c Z80/Tables.h Z80/Z80.c Z80/Z80.h
libscplayer_la_SOURCES += SAASound/SAAAmp.cpp SAASound/SAAAmp.h \
			  SAASound/SAAEnv.cpp SAASound/SAAEnv.h \
			  SAASound/SAAFreq.cpp SAASound/SAAFreq.dat \
			  SAASound/SAAFreq.h SAASound/SAAImpl.cpp \
			  SAASound/SAAImpl.h SAASound/SAANoise.cpp \
			  SAASound/SAANoise.h SAASound/SAASndC.cpp \
			  SAASound/SAASound.cpp SAASound/SAASound.h \
			  SAASound/types.h
EXTRA_DIST              = SAASound/LICENCE SAASound/README
