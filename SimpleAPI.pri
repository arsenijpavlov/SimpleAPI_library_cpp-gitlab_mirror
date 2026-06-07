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
    # Предварительное заполнение
    SIMPLEAPI_VERSION = "UNKNOWN(git/.git not found)"
    SIMPLEAPI_VERSION_NAME = "UNKNOWN(git/.git not found)"
    SIMPLEAPI_VERSION_COMMIT = "UNKNOWN(git/.git not found)"

    # Выяснение версии
    unix {
        # проверка, что git установлен в систему
        system(git --version) {
            GIT_FIND_MASK = "v[0-9.-a-zA-Z_\(\)]*"
#            GIT_OUTPUT = $$system(git -C $${PWD} describe --tags --match=\"$${GIT_FIND_MASK}\" --long)
#            message("GIT_OUTPUT: $${GIT_OUTPUT}")

            # т.к. старые Qt не умеют работать с regex, используем sed
            SED_MASK = "^(v[0-9.-]*)[_]*([_a-zA-Z0-9.,-]*)-([0-9]+)-g([a-fA-F0-9]+)"
            GIT_SED_OUTPUT = $$system(git -C $${PWD} describe --tags --match=\"$${GIT_FIND_MASK}\" --long \
                                | sed -E \"s/$${SED_MASK}/\1-b\3 \\\"\\2\\\" \\4/\" | sed \"s/-b0//\")

            message("GIT_SED_OUTPUT: $${GIT_SED_OUTPUT}")
            VERSION_INFO_LIST = $$split(GIT_SED_OUTPUT, " ")
            SIMPLEAPI_VERSION = $$member(VERSION_INFO_LIST, 0)
            SIMPLEAPI_VERSION_NAME = $$member(VERSION_INFO_LIST, 1)
            SIMPLEAPI_VERSION_NAME = $$replace(SIMPLEAPI_VERSION_NAME, "_", " ")
            SIMPLEAPI_VERSION_COMMIT = $$member(VERSION_INFO_LIST, 2)
        }
    }

    message("SIMPLEAPI_VERSION:        $${SIMPLEAPI_VERSION}")
    message("SIMPLEAPI_VERSION_NAME:   $${SIMPLEAPI_VERSION_NAME}")
    message("SIMPLEAPI_VERSION_COMMIT: $${SIMPLEAPI_VERSION_COMMIT}")
    # ========================================= Определить версию библиотеки

    # передача внутренних кавычек иным способом не работает
    DEFINES += "SIMPLEAPI_VERSION=\\\"$${SIMPLEAPI_VERSION}\\\""
    DEFINES += "SIMPLEAPI_VERSION_NAME=\\\"$${SIMPLEAPI_VERSION_NAME}\\\""
    DEFINES += "SIMPLEAPI_VERSION_COMMIT=\\\"$${SIMPLEAPI_VERSION_COMMIT}\\\""
}

