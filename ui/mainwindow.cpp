#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "convertlist.h"
#include "addtaskwizard.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QSettings>

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

    setup_menus();
    setup_toolbar();

    restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
    restoreState(settings.value("mainwindow/state").toByteArray());

    refresh_action_states();
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

// Events

void MainWindow::closeEvent(QCloseEvent *)
{
    QSettings settings;
    settings.setValue("mainwindow/geometry", saveGeometry());
    settings.setValue("mainwindow/state", saveState());
}

// Private Methods

// Popup wizard to add tasks.
void MainWindow::add_files()
{
    ConversionParameters param;

    AddTaskWizard wizard;

    if (wizard.exec_openfile() == QDialog::Accepted) {
        // Add all input files to the list.
        const QList<ConversionParameters> &params = wizard.getConversionParameters();
        QList<ConversionParameters>::const_iterator it = params.begin();
        for (; it!=params.end(); ++it) {
            m_list->addTask(*it);
        }
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

    ui->actionSetParameters->setDisabled(hide_SetParameters);
    ui->actionStartConversion->setDisabled(hide_StartConversion);
    ui->actionStopConversion->setDisabled(hide_StopConversion);
}
