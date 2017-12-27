TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -pthread
QMAKE_CFLAGS += -pthread
LIBS += -pthread

SOURCES += main.cpp \    
    workable_nn.cpp \
    neuron.cpp \
    dna.cpp \
    geneticalgorithm.cpp

HEADERS += \    
    workable_nn.h \
    neuron.h \
    dna.h \
    geneticalgorithm.h
