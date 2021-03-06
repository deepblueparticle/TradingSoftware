CONFIG(debug, debug|release) {
    SUFFIX = d
    BUILD_TYPE="debug"
}
else {
    SUFFIX =
    BUILD_TYPE="release"
}

TEMPLATE = app
CONFIG += c++11

gcc: QMAKE_CXXFLAGS += -fno-tree-vectorize -fpermissive

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

win32 {
    QMAKE_CXXFLAGS *= /MP /FS
    QMAKE_CXXFLAGS_WARN_ON = -wd4138
}

DESTDIR = bin
OBJECTS_DIR = build
UI_DIR = ui
MOC_DIR = moc
UI_HEADERS_DIR = ui
UI_SOURCES_DIR = ui

VERSION_MAJOR = 1
VERSION_MINOR = 2
VERSION_BUILD = 0

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR" \
           "VERSION_MINOR=$$VERSION_MINOR" \
           "VERSION_BUILD=$$VERSION_BUILD" \

TARGET = AlgoModel_v$${VERSION}$${SUFFIX}
