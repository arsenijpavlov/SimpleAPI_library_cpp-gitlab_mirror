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
    # TODO: решить через QMAKE_RUN_CXX = ./script.h && ${QMAKE_RUN_CXX}

    # должно быть совместимо с Qt 4.7 и выше
    VERSION_INFO_FILE_ABS = "$${PWD}/SimpleAPI/VersionInfo.h"
    VERSION_INFO_FILE_REL = $$relative_path("$${VERSION_INFO_FILE_ABS}")

    version_info_target.target = "$${VERSION_INFO_FILE_REL}"
    version_info_target.commands = bash \"$${PWD}/scripts/check_version_from_git.sh\"
    version_info_compiler.input = $$relative_path($${PWD}/scripts/TEMPLATE_VersionInfo.h)
    version_info_compiler.output = $${VERSION_INFO_FILE_REL}

    version_info_target.CONFIG += no_link target_predeps

    # возможно, лишняя команда
    version_info_target.depends = FORCE

    # регистрация цели
#    QMAKE_EXTRA_TARGETS += version_info_target
    QMAKE_EXTRA_COMPILERS += version_info_target

    # правило выполнения скрипта перед каждой компиляцией
    QMAKE_DISTCLEAN += $${VERSION_INFO_FILE_REL} # доп. правило при использовании старого Qt
    PRE_TARGETDEPS += $${VERSION_INFO_FILE_REL}

    INCLUDEPATH += $${SIMPLEAPI_PWD}/SimpleAPI
}

