#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ConvertList;

namespace Ui {
    class MainWindow;
}

class Presets;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*! Construct the main window
     *  @param parent the parent of the QObject
     *  @param fileList the input files from argv
     */
    explicit MainWindow(QWidget *parent = 0, const QStringList& fileList = QStringList());
    ~MainWindow();

private slots:
    void window_ready(); //!< The main window is completely loaded.
    void task_finished(int);
    void all_tasks_finished();

    // Menu Events
    void slotAddFiles();
    void slotExit();
    void slotStartConversion();
    void slotStopConversion();
    void slotSetConversionParameters();
    void slotOpenOutputFolder();
    void slotAboutQt();
    void slotAboutFFmpeg();

    void slotListContextMenu(QPoint);

    void refresh_action_states();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    Presets *m_presets; //!< the preset loader that lives throughout the program
    ConvertList *m_list;
    const QStringList m_argv_input_files;
    bool check_execute_conditions();
    void add_files();
    void add_files(const QStringList& files);
    void setup_menus();
    void setup_toolbar();
};

#endif // MAINWINDOW_H
