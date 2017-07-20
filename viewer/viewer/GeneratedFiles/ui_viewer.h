/********************************************************************************
** Form generated from reading UI file 'viewer.ui'
**
** Created: Thu Jul 20 15:38:18 2017
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWER_H
#define UI_VIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_viewerClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *viewerClass)
    {
        if (viewerClass->objectName().isEmpty())
            viewerClass->setObjectName(QString::fromUtf8("viewerClass"));
        viewerClass->resize(600, 400);
        menuBar = new QMenuBar(viewerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        viewerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(viewerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        viewerClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(viewerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        viewerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(viewerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        viewerClass->setStatusBar(statusBar);

        retranslateUi(viewerClass);

        QMetaObject::connectSlotsByName(viewerClass);
    } // setupUi

    void retranslateUi(QMainWindow *viewerClass)
    {
        viewerClass->setWindowTitle(QApplication::translate("viewerClass", "viewer", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class viewerClass: public Ui_viewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWER_H
