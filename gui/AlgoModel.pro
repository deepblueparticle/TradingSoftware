SHARED_PATH = ../shared

include( build.pri )
include( dependencies.pri )

#Source files in shared directory
SOURCES += $${SHARED_PATH}/src/shared/order.cpp \
           $${SHARED_PATH}/src/shared/defines.cpp \
           $${SHARED_PATH}/src/shared/commands.cpp \
           $${SHARED_PATH}/src/shared/response.cpp \
           $${SHARED_PATH}/src/shared/strategiesreader.cpp \
           $${SHARED_PATH}/src/shared/client_utils.cpp \
           $${SHARED_PATH}/src/shared/pnl.cpp \

#Source files for gui directory
SOURCES += src/algomodel1.cpp \
           src/tcpclient.cpp \
           src/newalgodialog.cpp \
           src/usersettings.cpp \
           src/heartbeat.cpp \
           src/startlogin.cpp \
           src/mainwindow.cpp \
           src/logindialog.cpp \
           src/DatabaseConnection.cpp \
           src/GuiQuery.cpp \
           src/ReportGenerator.cpp \
           src/GUIWorkerThread.cpp \
           src/main.cpp

#Header files for shared directory
HEADERS += $${SHARED_PATH}/include/shared/defines.h \
           $${SHARED_PATH}/include/shared/commands.h \
           $${SHARED_PATH}/include/shared/response.h \
           $${SHARED_PATH}/include/shared/order.h \
           $${SHARED_PATH}/include/shared/client_utils.h \
           $${SHARED_PATH}/include/shared/strategiesreader.h \
           $${SHARED_PATH}/include/shared/pnl.h \
           $${SHARED_PATH}/include/shared/EventType.H

#Header files for gui directory
HEADERS += include/mainwindow.h \
           include/logindialog.h \
           include/tcpclient.h \
           include/newalgodialog.h \
           include/usersettings.h \
           include/symboltickmap.h \
           include/algomodel1.h \
           include/heartbeat.h \
           include/startlogin.h \
           include/DatabaseConnection.h \
           include/GuiQuery.h \
           include/ReportGenerator.h \
           include/GUIWorkerThread.h

#Gui dialog files
FORMS += ui/mainwindow.ui \
         ui/logindialog.ui \
         ui/newalgodialog.ui
