# Run this makefile only on RaspberryPi.
# This example program should be built for robot, with DC motor connected to RaspberryPi.
# The WiringPi library is required to install on RaspberryPi for this example program to be successfully built.
# To install WiringPi library, visit the following address: https://github.com/WiringPi/WiringPi
# For more details about the robot hardware setup, visit the following address: https://github.com/Filip7700/Remote-Robot-Control

CC = g++ -std=c++20
LIBS = -lpthread -lwiringPi

Target = RaspberryPiRemoteRobotMotorControl.out

ObjDir = ./Objs

R2CPDir = ../..
LoggerDir = ${R2CPDir}/Logger
Base64Dir = ${R2CPDir}/Base64
ASIODir = ${R2CPDir}/ASIO/asio/include
WebSocketPPDir = ${R2CPDir}/WebSocketPP

DEPS = -I${R2CPDir} -I${LoggerDir} -I${Base64Dir} -I${ASIODir} -I${WebSocketPPDir}
LOGGERDEPS = -I${LoggerDir}

MainObj = ${ObjDir}/Main.o
RobotMotorControlModuleObj = ${ObjDir}/RobotMotorControlModule.o

#R2CP Objects
Base64Obj = ${ObjDir}/Base64.o
LoggerObj = ${ObjDir}/Logger.o
R2CPCommandObj = ${ObjDir}/R2CPCommand.o
R2CPCommandDictionaryObj = ${ObjDir}/R2CPCommandDictionary.o
R2CPPacketProcessorObj = ${ObjDir}/R2CPPacketProcessor.o
R2CPParameterObj = ${ObjDir}/R2CPParameter.o
R2CPParserObj = ${ObjDir}/R2CPParser.o
R2CPRuntimeBrokerObj = ${ObjDir}/R2CPRuntimeBroker.o
R2CPSerializerObj = ${ObjDir}/R2CPSerializer.o
R2CPTransceiverQueueObj = ${ObjDir}/R2CPTransceiverQueue.o
R2CPWebSocketServerObj = ${ObjDir}/R2CPWebSocketServer.o

AllObjs = ${MainObj} ${RobotMotorControlModuleObj} ${Base64Obj} ${LoggerObj} ${R2CPCommandObj} ${R2CPCommandDictionaryObj} ${R2CPPacketProcessorObj} ${R2CPParameterObj} ${R2CPParserObj} ${R2CPRuntimeBrokerObj} ${R2CPSerializerObj} ${R2CPTransceiverQueueObj} ${R2CPWebSocketServerObj}

# Link object into executable
${Target}: ${AllObjs}
	${CC} -Wall $^ -o $@ ${LIBS}

# Compile Main.cpp
${MainObj}: Main.cpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile RobotMotorControlModule.cpp
${RobotMotorControlModuleObj}: RobotMotorControlModule.cpp RobotMotorControlModule.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile Base64.cpp
${Base64Obj}: ${Base64Dir}/Base64.cpp ${Base64Dir}/Base64.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile Logger.cpp
${LoggerObj}: ${LoggerDir}/Logger.cpp ${LoggerDir}/Logger.hpp
	${CC} -c -Wall ${LOGGERDEPS} $< -o $@

# Compile R2CPCommand.cpp
${R2CPCommandObj}: ${R2CPDir}/R2CPCommand.cpp ${R2CPDir}/R2CPCommand.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile R2CPCommandDictionary.cpp
${R2CPCommandDictionaryObj}: ${R2CPDir}/R2CPCommandDictionary.cpp ${R2CPDir}/R2CPCommandDictionary.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile R2CPPacketProcessor.cpp
${R2CPPacketProcessorObj}: ${R2CPDir}/R2CPPacketProcessor.cpp ${R2CPDir}/R2CPPacketProcessor.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile R2CPParameter.cpp
${R2CPParameterObj}: ${R2CPDir}/R2CPParameter.cpp ${R2CPDir}/R2CPParameter.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile R2CPParser.cpp
${R2CPParserObj}: ${R2CPDir}/R2CPParser.cpp ${R2CPDir}/R2CPParser.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile R2CPRuntimeBroker.cpp
${R2CPRuntimeBrokerObj}: ${R2CPDir}/R2CPRuntimeBroker.cpp ${R2CPDir}/R2CPRuntimeBroker.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile R2CPSerializer.cpp
${R2CPSerializerObj}: ${R2CPDir}/R2CPSerializer.cpp ${R2CPDir}/R2CPSerializer.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile R2CPTransceiverQueue.cpp
${R2CPTransceiverQueueObj}: ${R2CPDir}/R2CPTransceiverQueue.cpp ${R2CPDir}/R2CPTransceiverQueue.hpp
	${CC} -c -Wall ${DEPS} $< -o $@

# Compile R2CPWebSocketServer.cpp
${R2CPWebSocketServerObj}: ${R2CPDir}/R2CPWebSocketServer.cpp ${R2CPDir}/R2CPWebSocketServer.hpp
	${CC} -c -Wall ${DEPS} $< -o $@



# Create object directory if it doesn't exist
${AllObjs}: | ${ObjDir}
${ObjDir}:
	mkdir $@

.PHONY: clean

clean:
	rm ${Target}
	rm -rf ${ObjDir}
