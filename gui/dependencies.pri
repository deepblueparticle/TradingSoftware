QT  += network widgets sql

BOOST_DIR="C:\Users\soumukil\Desktop\externals\boost-install"
QTXLSX_DIR="C:\Users\soumukil\Desktop\externals\QtXlsxWriter-master\build-qtxlsx-Desktop_Qt_5_6_0_MSVC2013_64bit-Debug"
JSON_DIR="C:\Users\soumukil\Desktop\externals\json"

win32 {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    LIBS += -lws2_32
}

INCLUDEPATH += include \
               $${SHARED_PATH}/include \
               $${BOOST_DIR}\include \
               $${QTXLSX_DIR}\include \
               $${JSON_DIR}

LIBS += -L$${QTXLSX_DIR}\lib -lQt5Xlsx$${SUFFIX}

DEFINES += XLSX_NO_LIB
DEFINES += FRONTEND
DEFINES += NOMINMAX
