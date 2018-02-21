#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/lib/Werror.o \
	${OBJECTDIR}/lib/Wsock.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/resources/Packet.o \
	${OBJECTDIR}/resources/Session.o \
	${OBJECTDIR}/resources/connection.o \
	${OBJECTDIR}/resources/database.o \
	${OBJECTDIR}/resources/fileHandle.o \
	${OBJECTDIR}/resources/md5.o \
	${OBJECTDIR}/resources/servercore.o \
	${OBJECTDIR}/resources/ssl.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/lib/Werror.o: lib/Werror.cpp
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/Werror.o lib/Werror.cpp

${OBJECTDIR}/lib/Wsock.o: lib/Wsock.cpp
	${MKDIR} -p ${OBJECTDIR}/lib
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/Wsock.o lib/Wsock.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/resources/Packet.o: resources/Packet.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/Packet.o resources/Packet.cpp

${OBJECTDIR}/resources/Session.o: resources/Session.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/Session.o resources/Session.cpp

${OBJECTDIR}/resources/connection.o: resources/connection.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/connection.o resources/connection.cpp

${OBJECTDIR}/resources/database.o: resources/database.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/database.o resources/database.cpp

${OBJECTDIR}/resources/fileHandle.o: resources/fileHandle.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/fileHandle.o resources/fileHandle.cpp

${OBJECTDIR}/resources/md5.o: resources/md5.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/md5.o resources/md5.cpp

${OBJECTDIR}/resources/servercore.o: resources/servercore.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/servercore.o resources/servercore.cpp

${OBJECTDIR}/resources/ssl.o: resources/ssl.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/ssl.o resources/ssl.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
