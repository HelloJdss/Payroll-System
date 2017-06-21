#-------------------------------------------------
#
# Project created by QtCreator 2017-05-16T20:50:15
#
#-------------------------------------------------

QT       += core gui sql axcontainer

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
    modifypersonalinformation.cpp \
    baseinfo.cpp \
    noeditdelegate.cpp \
    comboboxdelegate.cpp \
    attendance.cpp \
    addbonus.cpp \
    bonus.cpp \
    addsalary.cpp \
    salary.cpp \
    annualbonus.cpp

HEADERS  += payrollsystem.h \
    login.h \
    addcraftinfo.h \
    craft.h \
    addstaffinfo.h \
    dept.h \
    privilege.h \
    modifypersonalinformation.h \
    baseinfo.h \
    noeditdelegate.h \
    comboboxdelegate.h \
    attendance.h \
    addbonus.h \
    bonus.h \
    addsalary.h \
    salary.h \
    annualbonus.h

FORMS    += payrollsystem.ui \
    login.ui \
    addcraftinfo.ui \
    craft.ui \
    addstaffinfo.ui \
    dept.ui \
    privilege.ui \
    modifypersonalinformation.ui \
    baseinfo.ui \
    attendance.ui \
    addbonus.ui \
    bonus.ui \
    addsalary.ui \
    salary.ui \
    annualbonus.ui

RESOURCES += \
    icon.qrc

RC_FILE = icon.rc
