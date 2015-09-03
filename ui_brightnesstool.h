/********************************************************************************
** Form generated from reading UI file 'brightnesstool.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BRIGHTNESSTOOL_H
#define UI_BRIGHTNESSTOOL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BrightnessTool
{
public:
    QVBoxLayout *vboxLayout;
    QScrollBar *minimum;
    QLabel *min_label;
    QScrollBar *maximum;
    QLabel *max_label;

    void setupUi(QWidget *BrightnessTool)
    {
        if (BrightnessTool->objectName().isEmpty())
            BrightnessTool->setObjectName(QStringLiteral("BrightnessTool"));
        BrightnessTool->resize(184, 80);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(BrightnessTool->sizePolicy().hasHeightForWidth());
        BrightnessTool->setSizePolicy(sizePolicy);
        vboxLayout = new QVBoxLayout(BrightnessTool);
        vboxLayout->setSpacing(2);
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        minimum = new QScrollBar(BrightnessTool);
        minimum->setObjectName(QStringLiteral("minimum"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(minimum->sizePolicy().hasHeightForWidth());
        minimum->setSizePolicy(sizePolicy1);
        minimum->setOrientation(Qt::Horizontal);

        vboxLayout->addWidget(minimum);

        min_label = new QLabel(BrightnessTool);
        min_label->setObjectName(QStringLiteral("min_label"));
        min_label->setAlignment(Qt::AlignCenter);

        vboxLayout->addWidget(min_label);

        maximum = new QScrollBar(BrightnessTool);
        maximum->setObjectName(QStringLiteral("maximum"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(maximum->sizePolicy().hasHeightForWidth());
        maximum->setSizePolicy(sizePolicy2);
        maximum->setOrientation(Qt::Horizontal);

        vboxLayout->addWidget(maximum);

        max_label = new QLabel(BrightnessTool);
        max_label->setObjectName(QStringLiteral("max_label"));
        max_label->setAlignment(Qt::AlignCenter);

        vboxLayout->addWidget(max_label);


        retranslateUi(BrightnessTool);

        QMetaObject::connectSlotsByName(BrightnessTool);
    } // setupUi

    void retranslateUi(QWidget *BrightnessTool)
    {
        BrightnessTool->setWindowTitle(QApplication::translate("BrightnessTool", "Brightness", 0));
        min_label->setText(QApplication::translate("BrightnessTool", "Minimum", 0));
        max_label->setText(QApplication::translate("BrightnessTool", "Maximum", 0));
    } // retranslateUi

};

namespace Ui {
    class BrightnessTool: public Ui_BrightnessTool {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BRIGHTNESSTOOL_H
