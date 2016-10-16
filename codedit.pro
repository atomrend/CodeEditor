SOURCES  = main.cpp

SOURCES += ActionEditor.cpp
HEADERS += ActionEditor.h

SOURCES += Document.cpp
HEADERS += Document.h

SOURCES += Editor.cpp
HEADERS += Editor.h

SOURCES += FileInfoDialog.cpp
HEADERS += FileInfoDialog.h

SOURCES += FileSystemLineEdit.cpp
HEADERS += FileSystemLineEdit.h

SOURCES += MainWindow.cpp
HEADERS += MainWindow.h

SOURCES += PreferenceDialog.cpp
HEADERS += PreferenceDialog.h

SOURCES += PreferenceFile.cpp
HEADERS += PreferenceFile.h

SOURCES += QuickOpenDialog.cpp
HEADERS += QuickOpenDialog.h

SOURCES += RemoteSaveWorker.cpp
HEADERS += RemoteSaveWorker.h

SOURCES += SaveListDialog.cpp
HEADERS += SaveListDialog.h

SOURCES += SearchReplaceDialog.cpp
HEADERS += SearchReplaceDialog.h

SOURCES += StatusBar.cpp
HEADERS += StatusBar.h

SOURCES += SyntaxHighlighter.cpp
HEADERS += SyntaxHighlighter.h

# debug is required for sigsegv backtrace!
CONFIG += qt silent thread warn_on release
QT += xml svg
RESOURCES = images.qrc

CONFIG(release, debug | release) {
    DEFINES += INLINE
    QMAKE_CXXFLAGS += -O3 -Wno-error=unused-variable
    QMAKE_CXXFLAGS_RELEASE -= -O2 -Wno-error=unused-variable
} else {
    LIBS += -rdynamic
}

QMAKE_CXXFLAGS += -Werror

CONFIG(static) {
    message(IMPORTANT: Remove Qt shared library paths from LIBS!!!)
    QMAKE_CFLAGS += -static -Wno-error=unused-variable
    QMAKE_CXXFLAGS += -static -Wno-error=unused-variable
    LIBS += -lSM -lX11 -lXm -lXt -lXrender -lfontconfig
    LIBS += -lpng -ljpeg -lmng -ltiff -lrt -L/usr/lib/X11 -L/usr/X11R6/lib64
    LIBS += "-L${QTDIR}/lib-static"
}

BuildDirectory = build
DESTDIR     = "$$BuildDirectory"
OBJECTS_DIR = "$$BuildDirectory"
MOC_DIR     = "$$BuildDirectory"
RCC_DIR     = "$$BuildDirectory"
UI_DIR      = "$$BuildDirectory"
