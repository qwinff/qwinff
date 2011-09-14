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
    void start_conversion();
    void stop_conversion();
    void task_finished(int);
    void all_tasks_finished();

    // Menu Events
    void slotAddFiles();
    void slotExit();
    void slotMenuConvert();
    void slotStartConversion();
    void slotStopConversion();
    void slotSetConversionParameters();

private:
    Ui::MainWindow *ui;
    ConvertList *m_list;
    void add_files();
    void setup_menus();
    void setup_toolbar();
};

#endif // MAINWINDOW_H
