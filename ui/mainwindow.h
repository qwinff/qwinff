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
    void task_finished(int);
    void all_tasks_finished();

    // Menu Events
    void slotAddFiles();
    void slotExit();
    void slotMenuConvert();
    void slotStartConversion();
    void slotStopConversion();
    void slotSetConversionParameters();
    void slotOpenOutputFolder();
    void slotAboutQt();

    void slotListContextMenu(QPoint);

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    ConvertList *m_list;
    bool check_execute_conditions();
    void add_files();
    void setup_menus();
    void setup_toolbar();
    void refresh_action_states();
};

#endif // MAINWINDOW_H
