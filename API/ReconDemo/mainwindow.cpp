#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <reconserver.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonStart_clicked()
{
    if (!reconServer)
    {
        QString port = ui->editListeningPort->text();
        reconServer = shared_ptr<ReconServer>(new ReconServer(this, port.toInt()));
        connect(reconServer.get(), &ReconServer::signalDataReceived, this, &MainWindow::slotDataReceived);
    }
}

void MainWindow::slotDataReceived(QString message)
{
    ui->editInfo->appendPlainText(message);
}


