######################################################################
# Automatically generated by qmake (3.0) ?? 9? 29 14:02:39 2013
######################################################################

TEMPLATE = app
TARGET = fyyoscillometer
INCLUDEPATH += . src
RESOURCES       += data/fyyoscillometer.qrc
QT      +=      widgets network
QMAKE_CXXFLAGS  +=      -Wall
TRANSLATIONS    += translations/fyyoscillometer_zh_CN.ts

# Input
HEADERS += src/dataitem.h \
           src/fyych.h \
           src/fyymon.h \
           src/fyynet.h \
           src/fyyosimeter.h \
           src/fyystateboard.h
SOURCES += src/fyych.cc \
           src/fyymon.cc \
           src/fyynet.cc \
           src/fyyosimeter.cc \
           src/fyystateboard.cc \
           src/main.cc
