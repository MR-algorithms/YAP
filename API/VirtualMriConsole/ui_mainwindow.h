/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *editReconPort;
    QSpinBox *editTR;
    QLabel *label_5;
    QPushButton *browsedatabutton;
    QPlainTextEdit *editInfo;
    QLabel *label_6;
    QLineEdit *editMaskFile;
    QLabel *label_2;
    QLineEdit *editReconHost;
    QLineEdit *editDataFile;
    QLabel *label_3;
    QComboBox *maskComboBox;
    QLabel *label_4;
    QLabel *label_7;
    QLineEdit *editPhaseCount;
    QHBoxLayout *horizontalLayout;
    QPushButton *connectButton;
    QPushButton *scanButton;
    QPushButton *stopButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(475, 368);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        editReconPort = new QLineEdit(centralWidget);
        editReconPort->setObjectName(QStringLiteral("editReconPort"));

        gridLayout->addWidget(editReconPort, 4, 1, 1, 1);

        editTR = new QSpinBox(centralWidget);
        editTR->setObjectName(QStringLiteral("editTR"));
        editTR->setMaximum(99999);
        editTR->setValue(1000);

        gridLayout->addWidget(editTR, 5, 1, 1, 1);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        browsedatabutton = new QPushButton(centralWidget);
        browsedatabutton->setObjectName(QStringLiteral("browsedatabutton"));

        gridLayout->addWidget(browsedatabutton, 0, 2, 1, 1);

        editInfo = new QPlainTextEdit(centralWidget);
        editInfo->setObjectName(QStringLiteral("editInfo"));

        gridLayout->addWidget(editInfo, 6, 1, 1, 1);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);

        gridLayout->addWidget(label_6, 6, 0, 1, 1);

        editMaskFile = new QLineEdit(centralWidget);
        editMaskFile->setObjectName(QStringLiteral("editMaskFile"));

        gridLayout->addWidget(editMaskFile, 2, 1, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        editReconHost = new QLineEdit(centralWidget);
        editReconHost->setObjectName(QStringLiteral("editReconHost"));

        gridLayout->addWidget(editReconHost, 3, 1, 1, 1);

        editDataFile = new QLineEdit(centralWidget);
        editDataFile->setObjectName(QStringLiteral("editDataFile"));

        gridLayout->addWidget(editDataFile, 0, 1, 1, 1);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        maskComboBox = new QComboBox(centralWidget);
        maskComboBox->setObjectName(QStringLiteral("maskComboBox"));

        gridLayout->addWidget(maskComboBox, 2, 2, 1, 1);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 1, 0, 1, 1);

        editPhaseCount = new QLineEdit(centralWidget);
        editPhaseCount->setObjectName(QStringLiteral("editPhaseCount"));

        gridLayout->addWidget(editPhaseCount, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        connectButton = new QPushButton(centralWidget);
        connectButton->setObjectName(QStringLiteral("connectButton"));

        horizontalLayout->addWidget(connectButton);

        scanButton = new QPushButton(centralWidget);
        scanButton->setObjectName(QStringLiteral("scanButton"));

        horizontalLayout->addWidget(scanButton);

        stopButton = new QPushButton(centralWidget);
        stopButton->setObjectName(QStringLiteral("stopButton"));

        horizontalLayout->addWidget(stopButton);


        verticalLayout->addLayout(horizontalLayout);


        gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        maskComboBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Virtual MRI Console", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Data File: ", Q_NULLPTR));
        editReconPort->setText(QApplication::translate("MainWindow", "10", Q_NULLPTR));
        label_5->setText(QApplication::translate("MainWindow", "TR (ms):", Q_NULLPTR));
        browsedatabutton->setText(QApplication::translate("MainWindow", "Browse...", Q_NULLPTR));
        label_6->setText(QApplication::translate("MainWindow", "Info:", Q_NULLPTR));
        editMaskFile->setText(QApplication::translate("MainWindow", "0.1", Q_NULLPTR));
        editMaskFile->setPlaceholderText(QApplication::translate("MainWindow", "sample rate", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Mask:", Q_NULLPTR));
        editReconHost->setText(QApplication::translate("MainWindow", "127.0.0.1", Q_NULLPTR));
        editDataFile->setText(QApplication::translate("MainWindow", "D:/Data/cmr_data/RawData_256/RawData", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "Recon Host:", Q_NULLPTR));
        maskComboBox->clear();
        maskComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Sequential", Q_NULLPTR)
         << QApplication::translate("MainWindow", "FromCenter", Q_NULLPTR)
         << QApplication::translate("MainWindow", "FromSide", Q_NULLPTR)
         << QApplication::translate("MainWindow", "EqualSampling", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Customization", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Random", Q_NULLPTR)
         << QApplication::translate("MainWindow", "None", Q_NULLPTR)
        );
        label_4->setText(QApplication::translate("MainWindow", "Recon Port:", Q_NULLPTR));
        label_7->setText(QApplication::translate("MainWindow", "Phase Count", Q_NULLPTR));
        editPhaseCount->setText(QApplication::translate("MainWindow", "256", Q_NULLPTR));
        editPhaseCount->setPlaceholderText(QApplication::translate("MainWindow", "Please enter the total number of phase in the data.", Q_NULLPTR));
        connectButton->setText(QApplication::translate("MainWindow", "Connect", Q_NULLPTR));
        scanButton->setText(QApplication::translate("MainWindow", "Scan", Q_NULLPTR));
        stopButton->setText(QApplication::translate("MainWindow", "Stop", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
