#-------------------------------------------------
#
# Project created by QtCreator 2017-05-16T20:50:15
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PayrollSystemWindow
TEMPLATE = app


SOURCES += main.cpp\
        payrollsystem.cpp \
    login.cpp \
    addcraftinfo.cpp \
    craft.cpp \
    addstaffinfo.cpp \
    dept.cpp \
    privilege.cpp \
    modifypersonalinformation.cpp

HEADERS  += payrollsystem.h \
    login.h \
    addcraftinfo.h \
    craft.h \
    addstaffinfo.h \
    dept.h \
    privilege.h \
    modifypersonalinformation.h

FORMS    += payrollsystem.ui \
    login.ui \
    addcraftinfo.ui \
    craft.ui \
    addstaffinfo.ui \
    dept.ui \
    privilege.ui \
    modifypersonalinformation.ui
