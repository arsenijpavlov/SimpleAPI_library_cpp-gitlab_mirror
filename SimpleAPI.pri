# SimpleAPI include qmake file

!contains(CONFIG, simpleapi_included) {
    CONFIG += simpleapi_included # защита от повторного включения в проект

    SIMPLEAPI_PWD = $${PWD}
    INCLUDEPATH += $${SIMPLEAPI_PWD}

    CONFIG += c++11 # начиная с Qt5.7 
    QMAKE_CXXFLAGS += -std=c++11

    SOURCES += $$files($${SIMPLEAPI_PWD}/SimpleAPI/*.cpp, true)
    HEADERS += $$files($${SIMPLEAPI_PWD}/SimpleAPI/*.h, true)

    CONFIG += thread # включение многопоточности для Qt
    unix {
        QMAKE_CXXFLAGS += -pthread
        LIBS += -pthread
    }
}

