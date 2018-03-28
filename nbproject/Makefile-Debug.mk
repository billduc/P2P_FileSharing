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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/resources/File.o \
	${OBJECTDIR}/resources/Packet.o \
	${OBJECTDIR}/resources/Session.o \
	${OBJECTDIR}/resources/connection.o \
	${OBJECTDIR}/resources/connection_FileConn.o \
	${OBJECTDIR}/resources/connection_MainConn.o \
	${OBJECTDIR}/resources/connection_Test.o \
	${OBJECTDIR}/resources/database.o \
	${OBJECTDIR}/resources/fileHandle.o \
	${OBJECTDIR}/resources/logger.o \
	${OBJECTDIR}/resources/md5.o \
	${OBJECTDIR}/resources/servercore.o \
	${OBJECTDIR}/resources/servercore_FileConn.o \
	${OBJECTDIR}/resources/servercore_MainConn.o \
	${OBJECTDIR}/resources/servercore_Test.o \
	${OBJECTDIR}/resources/ssl.o \
	${OBJECTDIR}/srcComponent/Werror.o \
	${OBJECTDIR}/srcComponent/WopenSSL.o \
	${OBJECTDIR}/srcComponent/Wsock.o


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
LDLIBSOPTIONS=-L/usr/lib -lssl -lcrypto -lmysqlclient -pthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/fileserver ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/resources/File.o: resources/File.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/File.o resources/File.cpp

${OBJECTDIR}/resources/Packet.o: resources/Packet.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/Packet.o resources/Packet.cpp

${OBJECTDIR}/resources/Session.o: resources/Session.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/Session.o resources/Session.cpp

${OBJECTDIR}/resources/connection.o: resources/connection.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/connection.o resources/connection.cpp

${OBJECTDIR}/resources/connection_FileConn.o: resources/connection_FileConn.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/connection_FileConn.o resources/connection_FileConn.cpp

${OBJECTDIR}/resources/connection_MainConn.o: resources/connection_MainConn.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/connection_MainConn.o resources/connection_MainConn.cpp

${OBJECTDIR}/resources/connection_Test.o: resources/connection_Test.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/connection_Test.o resources/connection_Test.cpp

${OBJECTDIR}/resources/database.o: resources/database.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/database.o resources/database.cpp

${OBJECTDIR}/resources/fileHandle.o: resources/fileHandle.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/fileHandle.o resources/fileHandle.cpp

${OBJECTDIR}/resources/logger.o: resources/logger.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/logger.o resources/logger.cpp

${OBJECTDIR}/resources/md5.o: resources/md5.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/md5.o resources/md5.cpp

${OBJECTDIR}/resources/servercore.o: resources/servercore.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/servercore.o resources/servercore.cpp

${OBJECTDIR}/resources/servercore_FileConn.o: resources/servercore_FileConn.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/servercore_FileConn.o resources/servercore_FileConn.cpp

${OBJECTDIR}/resources/servercore_MainConn.o: resources/servercore_MainConn.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/servercore_MainConn.o resources/servercore_MainConn.cpp

${OBJECTDIR}/resources/servercore_Test.o: resources/servercore_Test.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/servercore_Test.o resources/servercore_Test.cpp

${OBJECTDIR}/resources/ssl.o: resources/ssl.cpp
	${MKDIR} -p ${OBJECTDIR}/resources
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/resources/ssl.o resources/ssl.cpp

${OBJECTDIR}/srcComponent/Werror.o: srcComponent/Werror.cpp
	${MKDIR} -p ${OBJECTDIR}/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/srcComponent/Werror.o srcComponent/Werror.cpp

${OBJECTDIR}/srcComponent/WopenSSL.o: srcComponent/WopenSSL.cpp
	${MKDIR} -p ${OBJECTDIR}/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/srcComponent/WopenSSL.o srcComponent/WopenSSL.cpp

${OBJECTDIR}/srcComponent/Wsock.o: srcComponent/Wsock.cpp
	${MKDIR} -p ${OBJECTDIR}/srcComponent
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/srcComponent/Wsock.o srcComponent/Wsock.cpp

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
