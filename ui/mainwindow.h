#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ConvertList;

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
    void on_pushButton_clicked();
    void start_conversion();
    void stop_conversion();
    void task_finished(int);
    void all_tasks_finished();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    ConvertList *m_list;
};

#endif // MAINWINDOW_H
