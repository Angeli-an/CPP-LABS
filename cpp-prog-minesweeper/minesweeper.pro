# ---- force Qt5 tools (важно для Homebrew, чтобы moc/uic/rcc были из Qt5) ----
QMAKE_MOC = $$[QT_INSTALL_BINS]/moc
QMAKE_UIC = $$[QT_INSTALL_BINS]/uic
QMAKE_RCC = $$[QT_INSTALL_BINS]/rcc

greaterThan(QT_MAJOR_VERSION, 5) {
    error("Build with Qt 5.x only")
}

# ---------------- common ----------------
QT += widgets
CONFIG += strict_c++ sdk_no_version_check
INCLUDEPATH += include

unix|macx: QMAKE_CXXFLAGS += -std=c++20
win32:     QMAKE_CXXFLAGS += /std:c++20

macx {
    # фиксы OpenGL для Qt5 на macOS
    QMAKE_LIBS_OPENGL =
    QMAKE_LIBS_OPENGL += -framework OpenGL
    QMAKE_LIBS_OPENGL_SHLIB =
    QMAKE_LIBS_OPENGL_SHLIB += -framework OpenGL
    LIBS -= -framework AGL
}

# --------------- APP (default) ---------------
!tests {
    TEMPLATE = app
    TARGET   = minesweeper

    SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
        src/minesweeper.cpp \
        src/newgamedialog.cpp \
        src/cellbutton.cpp

    HEADERS += \
        include/mainwindow.h \
        include/minesweeper.h \
        include/newgamedialog.h \
        include/cellbutton.h

    OBJECTS_DIR = obj/minesweeper
    MOC_DIR     = moc
    DESTDIR     = release
}

# --------------- TESTS (separate target) ---------------
tests {
    TEMPLATE = app
    TARGET   = minesweeper_tests
    CONFIG  += console
    CONFIG  -= app_bundle
    QT      += testlib

    # main в tests.cpp, main.cpp НЕ добавляем
    SOURCES += \
        test/tests.cpp \
        src/mainwindow.cpp \
        src/minesweeper.cpp \
        src/newgamedialog.cpp \
        src/cellbutton.cpp

    HEADERS += \
        include/mainwindow.h \
        include/minesweeper.h \
        include/newgamedialog.h \
        include/cellbutton.h

    DEFINES += UNIT_TESTS

    # ---- поиск GoogleTest ----
    isEmpty(GTEST_ROOT) {
        exists(/opt/homebrew/opt/googletest/include/gtest/gtest.h) {
            GTEST_ROOT = /opt/homebrew/opt/googletest
        } else: exists(/usr/local/opt/googletest/include/gtest/gtest.h) {
            GTEST_ROOT = /usr/local/opt/googletest
        } else: exists($$PWD/googletest/include/gtest/gtest.h) {
            GTEST_ROOT = $$PWD/googletest
        }
    }
    !exists($$GTEST_ROOT/include/gtest/gtest.h) {
        error("GoogleTest headers not found. Set GTEST_ROOT or install googletest. Tried: $$GTEST_ROOT")
    }
    INCLUDEPATH += $$GTEST_ROOT/include

    unix|macx {
        LIBS += -L$$GTEST_ROOT/lib -lgtest -lgtest_main -pthread
        QMAKE_RPATHDIR += $$GTEST_ROOT/lib
    }
    win32 {
        LIBS += $$GTEST_ROOT/lib/gtest.lib $$GTEST_ROOT/lib/gtest_main.lib
    }

    OBJECTS_DIR = obj/tests
    MOC_DIR     = moc
    DESTDIR     = debug
}
