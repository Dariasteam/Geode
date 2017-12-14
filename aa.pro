TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    codified_nn.cpp \
    workable_nn.cpp \
    neuron.cpp \
    dna.cpp

HEADERS += \
    codified_nn.h \
    workable_nn.h \
    neuron.h \
    dna.h
