#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Interface/Interfaces.h"
#include <memory>

class ReconServer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //void on_loadpushButton_clicked();

    void on_actionOpen_triggered();
    void slotDataReceived(QByteArray dataArray);
    void on_buttonStart_clicked();

private:
    Ui::MainWindow *ui;
    Yap::SmartPtr<Yap::IData> CreateDemoData();
    void SetImportedProcessors();
    std::shared_ptr<ReconServer> reconServer;
};

#endif // MAINWINDOW_H
