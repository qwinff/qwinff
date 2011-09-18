#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "convertlist.h"
#include "addtaskwizard.h"
#include "converter/ffmpeginterface.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QApplication>
#include <QSettings>
#include <QCloseEvent>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSettings settings;

    ui->setupUi(this);

    m_list = new ConvertList();
    this->centralWidget()->layout()->addWidget(m_list);
    m_list->adjustSize();

    connect(m_list, SIGNAL(task_finished(int)),
            this, SLOT(task_finished(int)));
    connect(m_list, SIGNAL(all_tasks_finished()),
            this, SLOT(all_tasks_finished()));
    connect(m_list, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotListContextMenu(QPoint)));
    m_list->setContextMenuPolicy(Qt::CustomContextMenu);

    setup_menus();
    setup_toolbar();

    restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
    restoreState(settings.value("mainwindow/state").toByteArray());

    refresh_action_states();

    if (!check_execute_conditions()) {
        // Close the window immediately after it has started.
        QTimer::singleShot(0, this, SLOT(close()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::task_finished(int /*exitcode*/)
{
//    if (exitcode == 0) { // succeed
//        QMessageBox::information(this, this->windowTitle()
//                                 , tr("Conversion finished successfully.")
//                                 , QMessageBox::Ok);
//    } else { // failed
//        QMessageBox::critical(this, this->windowTitle()
//                              , tr("Conversion failed.")
//                              , QMessageBox::Ok);
//    }
}

void MainWindow::all_tasks_finished()
{
    QMessageBox::information(this, this->windowTitle(),
                             tr("All tasks has finished."), QMessageBox::Ok);
    refresh_action_states();
}

// Menu Events

void MainWindow::slotAddFiles()
{
    add_files();
}

void MainWindow::slotExit()
{
    this->close();
}

void MainWindow::slotMenuConvert()
{
    refresh_action_states();
}

void MainWindow::slotStartConversion()
{
    if (m_list->isEmpty()) {
        QMessageBox::information(this, this->windowTitle(),
                                 tr("Nothing to convert."), QMessageBox::Ok);
    } else {
        m_list->start();
    }

    refresh_action_states();
}

void MainWindow::slotStopConversion()
{
    m_list->stop();

    refresh_action_states();
}

void MainWindow::slotSetConversionParameters()
{

}

// Open the output folder of the file.
void MainWindow::slotOpenOutputFolder()
{
    const ConversionParameters *param = m_list->getCurrentIndexParameter();
    if (param) {
        QString folder_path = QFileInfo(param->destination).path();
        if (QFileInfo(folder_path).exists()) {
            QDesktopServices::openUrl(QUrl(folder_path));
        }
    }
}

void MainWindow::slotAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::slotListContextMenu(QPoint /*pos*/)
{
    refresh_action_states();

    QMenu menu(this);
    menu.addAction(ui->actionOpenOutputFolder);
    menu.addSeparator();
    menu.addAction(ui->actionStartConversion);
    menu.addAction(ui->actionStopConversion);

    menu.exec(QCursor::pos());
}

// Events

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_list->isBusy()) {
        int reply = QMessageBox::warning(this, this->windowTitle(),
                             tr("Conversion is still in progress. Abort?"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }
    }

    m_list->stop();
    QSettings settings;
    settings.setValue("mainwindow/geometry", saveGeometry());
    settings.setValue("mainwindow/state", saveState());
}

// Private Methods

/* Check if necessary external programs exist.
   (1) Check ffmpeg
*/
bool MainWindow::check_execute_conditions()
{
    if (!FFmpegInterface::hasFFmpeg())
        return false;
    return true;
}

// Popup wizard to add tasks.
void MainWindow::add_files()
{
    ConversionParameters param;

    AddTaskWizard wizard;

    if (wizard.exec_openfile() == QDialog::Accepted) {
        // Add all input files to the list.
        const QList<ConversionParameters> &paramList = wizard.getConversionParameters();
        m_list->addTasks(paramList);
    }
}

void MainWindow::setup_menus()
{
    /* === Menu Events === */

    // File
    connect(ui->actionAddFiles, SIGNAL(triggered()),
            this, SLOT(slotAddFiles()));
    connect(ui->actionExit, SIGNAL(triggered()),
            this, SLOT(slotExit()));

    // Convert
    connect(ui->menuConvert, SIGNAL(aboutToShow()),
            this, SLOT(slotMenuConvert()));
    connect(ui->actionStartConversion, SIGNAL(triggered()),
            this, SLOT(slotStartConversion()));
    connect(ui->actionStopConversion, SIGNAL(triggered()),
            this, SLOT(slotStopConversion()));
    connect(ui->actionSetParameters, SIGNAL(triggered()),
            this, SLOT(slotSetConversionParameters()));
    connect(ui->actionOpenOutputFolder, SIGNAL(triggered()),
            this, SLOT(slotOpenOutputFolder()));

    // About
    connect(ui->actionAboutQt, SIGNAL(triggered()),
            this, SLOT(slotAboutQt()));


    // hide actionSetParameters because the function is incomplete.
    ui->actionSetParameters->setVisible(false);
}

void MainWindow::setup_toolbar()
{
    QToolBar *toolbar = ui->toolBar;
    toolbar->addAction(ui->actionAddFiles);
    toolbar->addAction(ui->actionStartConversion);
    toolbar->addAction(ui->actionStopConversion);
}

// Hide unused actions
void MainWindow::refresh_action_states()
{
    // Hide actionSetParameters if no item in m_list is selected.
    bool hide_SetParameters = !m_list->selectedItems().isEmpty();

    // Hide actionStartConversion if the conversion is in progress.
    bool hide_StartConversion = m_list->isBusy();

    // Hide actionStopConversion if nothing is being converted.
    bool hide_StopConversion = !m_list->isBusy();

    // Show actionOpenOutputFolder only if 1 file is selected.
    bool hide_OpenFolder = (m_list->selectedItems().size() != 1);

    ui->actionSetParameters->setDisabled(hide_SetParameters);
    ui->actionStartConversion->setDisabled(hide_StartConversion);
    ui->actionStopConversion->setDisabled(hide_StopConversion);
    ui->actionOpenOutputFolder->setDisabled(hide_OpenFolder);
}
