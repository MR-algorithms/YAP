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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <imagelayoutwidget.h>
#include <plot1d.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *editPipeline;
    QPushButton *buttonBrowsePipeline;
    QLabel *label_5;
    QLineEdit *editListeningPort;
    QLineEdit *editOutput;
    QLabel *label_3;
    QPlainTextEdit *editInfo;
    QPushButton *buttoinBrowseOuput;
    QPushButton *buttonStart;
    QPushButton *buttonFinish;
    QSpacerItem *verticalSpacer;
    Plot1D *plot1d_widget;
    ImageLayoutWidget *imagelayout_widget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1286, 792);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 2, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 1, 2, 1, 1);

        editPipeline = new QLineEdit(centralWidget);
        editPipeline->setObjectName(QStringLiteral("editPipeline"));

        gridLayout->addWidget(editPipeline, 1, 3, 1, 1);

        buttonBrowsePipeline = new QPushButton(centralWidget);
        buttonBrowsePipeline->setObjectName(QStringLiteral("buttonBrowsePipeline"));

        gridLayout->addWidget(buttonBrowsePipeline, 1, 4, 1, 1);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_5, 3, 2, 1, 1);

        editListeningPort = new QLineEdit(centralWidget);
        editListeningPort->setObjectName(QStringLiteral("editListeningPort"));

        gridLayout->addWidget(editListeningPort, 0, 3, 1, 1);

        editOutput = new QLineEdit(centralWidget);
        editOutput->setObjectName(QStringLiteral("editOutput"));

        gridLayout->addWidget(editOutput, 2, 3, 1, 1);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 2, 2, 1, 1);

        editInfo = new QPlainTextEdit(centralWidget);
        editInfo->setObjectName(QStringLiteral("editInfo"));

        gridLayout->addWidget(editInfo, 3, 3, 1, 1);

        buttoinBrowseOuput = new QPushButton(centralWidget);
        buttoinBrowseOuput->setObjectName(QStringLiteral("buttoinBrowseOuput"));

        gridLayout->addWidget(buttoinBrowseOuput, 2, 4, 1, 1);

        buttonStart = new QPushButton(centralWidget);
        buttonStart->setObjectName(QStringLiteral("buttonStart"));

        gridLayout->addWidget(buttonStart, 4, 3, 1, 1);

        buttonFinish = new QPushButton(centralWidget);
        buttonFinish->setObjectName(QStringLiteral("buttonFinish"));

        gridLayout->addWidget(buttonFinish, 4, 4, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 108, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        plot1d_widget = new Plot1D(centralWidget);
        plot1d_widget->setObjectName(QStringLiteral("plot1d_widget"));

        verticalLayout->addWidget(plot1d_widget);

        verticalLayout->setStretch(0, 2);
        verticalLayout->setStretch(1, 2);
        verticalLayout->setStretch(2, 3);

        horizontalLayout->addLayout(verticalLayout);

        imagelayout_widget = new ImageLayoutWidget(centralWidget);
        imagelayout_widget->setObjectName(QStringLiteral("imagelayout_widget"));

        horizontalLayout->addWidget(imagelayout_widget);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 5);

        gridLayout_2->addLayout(horizontalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1286, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("MainWindow", "Open(&O)", Q_NULLPTR));
        actionSave->setText(QApplication::translate("MainWindow", "Save", Q_NULLPTR));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", Q_NULLPTR));
        actionAbout->setText(QApplication::translate("MainWindow", "About", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Listening Port:", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Pipeline:", Q_NULLPTR));
        editPipeline->setText(QApplication::translate("MainWindow", "D:/Projects/YAP/API/ReconDemo/config/pipelines/realtime_recon.pipeline", Q_NULLPTR));
        buttonBrowsePipeline->setText(QApplication::translate("MainWindow", "Browse...", Q_NULLPTR));
        label_5->setText(QApplication::translate("MainWindow", "Info:", Q_NULLPTR));
        editListeningPort->setText(QApplication::translate("MainWindow", "10", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "Output Dir:", Q_NULLPTR));
        buttoinBrowseOuput->setText(QApplication::translate("MainWindow", "Browse...", Q_NULLPTR));
        buttonStart->setText(QApplication::translate("MainWindow", "Start", Q_NULLPTR));
        buttonFinish->setText(QApplication::translate("MainWindow", "Finish", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
