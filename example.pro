TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -pthread
QMAKE_CFLAGS += -pthread
LIBS += -pthread

SOURCES += main.cpp \    
    neural_network/workable_nn.cpp \
    neural_network/neuron.cpp \
    neural_network/dna.cpp \

HEADERS += \    
    neural_network/workable_nn.h \
    neural_network/neuron.h \
    neural_network/dna.h \
    genetic_algorithm/geneticalgorithm.h
