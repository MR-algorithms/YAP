#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void on_buttonStart_clicked();
    void slotDataReceived(QString message);
private:
    Ui::MainWindow *ui;
    std::shared_ptr<ReconServer> reconServer;

};

#endif // MAINWINDOW_H
