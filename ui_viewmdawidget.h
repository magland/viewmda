/********************************************************************************
** Form generated from reading UI file 'viewmdawidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWMDAWIDGET_H
#define UI_VIEWMDAWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "viewmdawidget1d.h"
#include "viewmdawidget2d.h"

QT_BEGIN_NAMESPACE

class Ui_ViewmdaWidget
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    ViewmdaWidget2D *widget2d;
    ViewmdaWidget1D *widget1d;
    QScrollBar *scrollbar;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *ViewmdaWidget)
    {
        if (ViewmdaWidget->objectName().isEmpty())
            ViewmdaWidget->setObjectName(QStringLiteral("ViewmdaWidget"));
        ViewmdaWidget->resize(394, 274);
        centralwidget = new QWidget(ViewmdaWidget);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        centralwidget->setGeometry(QRect(0, 33, 394, 222));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        widget2d = new ViewmdaWidget2D(splitter);
        widget2d->setObjectName(QStringLiteral("widget2d"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget2d->sizePolicy().hasHeightForWidth());
        widget2d->setSizePolicy(sizePolicy);
        splitter->addWidget(widget2d);
        widget1d = new ViewmdaWidget1D(splitter);
        widget1d->setObjectName(QStringLiteral("widget1d"));
        widget1d->setMinimumSize(QSize(0, 100));
        splitter->addWidget(widget1d);

        verticalLayout->addWidget(splitter);

        scrollbar = new QScrollBar(centralwidget);
        scrollbar->setObjectName(QStringLiteral("scrollbar"));
        scrollbar->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(scrollbar);

        ViewmdaWidget->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ViewmdaWidget);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 394, 21));
        ViewmdaWidget->setMenuBar(menubar);
        statusbar = new QStatusBar(ViewmdaWidget);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        statusbar->setGeometry(QRect(0, 255, 394, 19));
        ViewmdaWidget->setStatusBar(statusbar);
        toolBar = new QToolBar(ViewmdaWidget);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        toolBar->setGeometry(QRect(0, 21, 394, 12));
        ViewmdaWidget->addToolBar(Qt::TopToolBarArea, toolBar);

        retranslateUi(ViewmdaWidget);

        QMetaObject::connectSlotsByName(ViewmdaWidget);
    } // setupUi

    void retranslateUi(QMainWindow *ViewmdaWidget)
    {
        ViewmdaWidget->setWindowTitle(QApplication::translate("ViewmdaWidget", "ViewMda", 0));
        toolBar->setWindowTitle(QApplication::translate("ViewmdaWidget", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class ViewmdaWidget: public Ui_ViewmdaWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWMDAWIDGET_H
