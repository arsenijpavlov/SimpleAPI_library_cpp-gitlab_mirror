# SimpleAPI include qmake file

!contains(CONFIG, simpleapi_included) {
    message("Start including SimpleAPI")

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

    # Определить версию библиотеки =========================================
    # вшиваем запуск генерации информации о версии в каждый вызов компилятора
    QMAKE_CC = $${SIMPLEAPI_PWD}/scripts/check_version_from_git.sh && $${QMAKE_CC}
    QMAKE_CXX = $${SIMPLEAPI_PWD}/scripts/check_version_from_git.sh && $${QMAKE_CXX}
    # ========================================= Определить версию библиотеки
}

