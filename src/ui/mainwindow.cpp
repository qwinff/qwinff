/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "convertlist.h"
#include "addtaskwizard.h"
#include "aboutffmpegdialog.h"
#include "optionsdialog.h"
#include "aboutdialog.h"
#include "services/paths.h"
#include "services/notification.h"
#include "converter/ffmpeginterface.h"
#include "converter/mediaprobe.h"
#include "converter/presets.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QDesktopServices>
#include <QApplication>
#include <QSettings>
#include <QCloseEvent>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent, const QStringList& fileList) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_presets(new Presets(this)),
    m_list(new ConvertList(m_presets, this)),
    m_argv_input_files(fileList),
    m_elapsedTimeLabel(new QLabel(this)),
    m_timer(new QTimer(this))
{
    QSettings settings;

    ui->setupUi(this);

    this->centralWidget()->layout()->addWidget(m_list);
    m_list->adjustSize();

    connect(m_list, SIGNAL(task_finished(int)),
            this, SLOT(task_finished(int)));
    connect(m_list, SIGNAL(all_tasks_finished()),
            this, SLOT(all_tasks_finished()));
    connect(m_list, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotListContextMenu(QPoint)));
    connect(m_list, SIGNAL(itemSelectionChanged()),
            this, SLOT(refresh_action_states()));
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(timerEvent()));
    connect(m_list, SIGNAL(started()),
            this, SLOT(conversion_started()));
    connect(m_list, SIGNAL(stopped()),
            this, SLOT(conversion_stopped()));

    m_list->setContextMenuPolicy(Qt::CustomContextMenu);

    setup_menus();
    setup_toolbar();
    setup_statusbar();

    restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
    restoreState(settings.value("mainwindow/state").toByteArray());

    refresh_action_states();

    if (!check_execute_conditions()) {
        // Close the window immediately after it has started.
        QTimer::singleShot(0, this, SLOT(close()));
    } else {
        QTimer::singleShot(0, this, SLOT(window_ready()));
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::window_ready()
{
    if (!m_argv_input_files.isEmpty()) {
        add_files(m_argv_input_files);
    }
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
    Notification::send("QWinFF", tr("All tasks has finished."), NotifyLevel::INFO);
    refresh_action_states();
}

// Menu Events

void MainWindow::slotAddFiles()
{
    add_files();
}

void MainWindow::slotOptions()
{
    OptionsDialog dialog;
    dialog.exec();
}

void MainWindow::slotExit()
{
    this->close();
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
    if (m_list->selectedCount() > 0) {
        m_list->editSelectedParameters();
    }
}

// Open the output folder of the file.
void MainWindow::slotOpenOutputFolder()
{
    const ConversionParameters *param = m_list->getCurrentIndexParameter();
    if (param) {
        QString folder_path = QFileInfo(param->destination).path();
        if (QFileInfo(folder_path).exists()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(folder_path));
        }
    }
}

void MainWindow::slotAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::slotAboutFFmpeg()
{
    AboutFFmpegDialog().exec();
}

void MainWindow::slotAbout()
{
    AboutDialog().exec();
}

void MainWindow::slotListContextMenu(QPoint /*pos*/)
{
    refresh_action_states();

    QMenu menu;
    menu.addAction(ui->actionOpenOutputFolder);
    menu.addSeparator();
    menu.addAction(ui->actionRemoveSelectedItems);
    menu.addSeparator();
    menu.addAction(ui->actionRetry);
    menu.addAction(ui->actionRetryAll);
    menu.addSeparator();
    menu.addAction(ui->actionChangeOutputFilename);
    menu.addAction(ui->actionChangeOutputDirectory);
    menu.addAction(ui->actionSetParameters);

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

void MainWindow::timerEvent()
{
    if (m_list->isBusy()) { // update elapsed time
        int total_seconds = m_list->elapsedTime() / 1000;
        int hours = total_seconds / 3600;
        int minutes = (total_seconds / 60) % 60;
        int seconds = total_seconds % 60;

        m_elapsedTimeLabel->setText(
                    tr("Elapsed Time: %1h %2m %3s")
                    .arg(hours).arg(minutes).arg(seconds)
                    );
    }
}

void MainWindow::conversion_started()
{
    m_elapsedTimeLabel->clear();
    m_timer->start(1000);
}

void MainWindow::conversion_stopped()
{
    m_timer->stop();
}

// Private Methods

/* Check if necessary external programs exist.
   (1) Check ffmpeg
   (2) Check ffprobe
   (3) Load presets
   This function should return true if all the conditions are met
   and return false if any of the conditions fails.
*/
bool MainWindow::check_execute_conditions()
{
    // check ffmpeg
    if (!FFmpegInterface::hasFFmpeg()) {
        QMessageBox::critical(this, tr("FFmpeg Error"),
                              tr("FFmpeg not found. "
                                 "The application will quit now."));
        return false;
    }

    // check ffprobe
    if (!MediaProbe::available()) { // The probe failed to start.
        QMessageBox::critical(this, tr("FFprobe Error"),
                              tr("FFprobe not found. "
                                 "The application will quit now."));
        return false;
    }

    // load presets
    if (!load_presets())
        return false;

    return true;
}

// Popup wizard to add tasks.
void MainWindow::add_files()
{
    AddTaskWizard wizard(m_presets);

    if (wizard.exec_openfile() == QDialog::Accepted) {
        // Add all input files to the list.
        const QList<ConversionParameters> &paramList = wizard.getConversionParameters();
        m_list->addTasks(paramList);
    }
}

void MainWindow::add_files(const QStringList &fileList)
{
    QList<QUrl> urlList;

    foreach (QString file, fileList) {
        urlList.push_back(QUrl::fromLocalFile(file));
    }

    AddTaskWizard wizard(m_presets);

    if (wizard.exec(urlList) == QDialog::Accepted) {
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
    connect(ui->actionOptions, SIGNAL(triggered()),
            this, SLOT(slotOptions()));
    connect(ui->actionExit, SIGNAL(triggered()),
            this, SLOT(slotExit()));

    // Edit
    connect(ui->menuEdit, SIGNAL(aboutToShow()),
            this, SLOT(refresh_action_states()));
    connect(ui->actionRemoveSelectedItems, SIGNAL(triggered()),
            m_list, SLOT(removeSelectedItems()));
    connect(ui->actionRemoveCompletedItems, SIGNAL(triggered()),
            m_list, SLOT(removeCompletedItems()));
    connect(ui->actionClearList, SIGNAL(triggered()),
            m_list, SLOT(clear()));
    connect(ui->actionSetParameters, SIGNAL(triggered()),
            this, SLOT(slotSetConversionParameters()));
    connect(ui->actionOpenOutputFolder, SIGNAL(triggered()),
            this, SLOT(slotOpenOutputFolder()));
    connect(ui->actionChangeOutputFilename, SIGNAL(triggered()),
            m_list, SLOT(changeSelectedOutputFile()));
    connect(ui->actionChangeOutputDirectory, SIGNAL(triggered()),
            m_list, SLOT(changeSelectedOutputDirectory()));

    // Convert
    connect(ui->menuConvert, SIGNAL(aboutToShow()),
            this, SLOT(refresh_action_states()));
    connect(ui->actionStartConversion, SIGNAL(triggered()),
            this, SLOT(slotStartConversion()));
    connect(ui->actionStopConversion, SIGNAL(triggered()),
            this, SLOT(slotStopConversion()));
    connect(ui->actionRetry, SIGNAL(triggered()),
            m_list, SLOT(retrySelectedItems()));
    connect(ui->actionRetry, SIGNAL(triggered()),
            this, SLOT(refresh_action_states()));
    connect(ui->actionRetryAll, SIGNAL(triggered()),
            m_list, SLOT(retryAll()));
    connect(ui->actionRetryAll, SIGNAL(triggered()),
            this, SLOT(refresh_action_states()));

    // About
    connect(ui->actionAboutQt, SIGNAL(triggered()),
            this, SLOT(slotAboutQt()));
    connect(ui->actionAboutFFmpeg, SIGNAL(triggered()),
            this, SLOT(slotAboutFFmpeg()));
    connect(ui->actionAbout, SIGNAL(triggered()),
            this, SLOT(slotAbout()));

}

void MainWindow::setup_toolbar()
{
    QToolBar *toolbar = ui->toolBar;
    toolbar->addAction(ui->actionAddFiles);
    toolbar->addAction(ui->actionStartConversion);
    toolbar->addAction(ui->actionStopConversion);
    toolbar->addSeparator();
    toolbar->addAction(ui->actionRetry);
    toolbar->addAction(ui->actionRetryAll);
    toolbar->addSeparator();
    toolbar->addAction(ui->actionOpenOutputFolder);
}

void MainWindow::setup_statusbar()
{
    ui->statusBar->addPermanentWidget(m_elapsedTimeLabel);
}

bool MainWindow::load_presets()
{
    // The default preset file is located in <datapath>/presets.xml
    QString default_preset_file = QDir(Paths::dataPath()).absoluteFilePath("presets.xml");

#ifndef PORTABLE_APP
    // Each user has his/her own preset file located in ${HOME}/.qwinff/presets.xml
    QString local_preset_file = QDir(QDir::homePath()).absoluteFilePath(".qwinff/presets.xml");

    // If there's no preset file in the home directory, then copy the default preset file.
    if (!QFile(local_preset_file).exists()) {
        if (!QFile::copy(default_preset_file, local_preset_file)) {
            QMessageBox::critical(this, this->windowTitle(),
                                  tr("Failed to create preset file: %1").arg(local_preset_file) + "\n"
                                  + tr("The application will quit now"));
            return false;
        }
        qDebug() << QString("Created preset file: %1").arg(local_preset_file);
    }
#else // PORTABLE_APP
    QString local_preset_file = default_preset_file;
#endif

    // Load the preset file from the user's home directory
    // The presets are loaded once and shared between objects
    // that need the information.
    if (!m_presets->readFromFile(local_preset_file)) {
        QMessageBox::critical(this, this->windowTitle(),
                              tr("Failed to load preset file. "
                                 "The application will quit now."));
        return false;
    }
    return true;
}

// Hide unused actions
void MainWindow::refresh_action_states()
{
    int selected_file_count = m_list->selectedCount();

    // Hide actionSetParameters if no item in m_list is selected.
    bool hide_SetParameters = (selected_file_count == 0);

    // Hide actionStartConversion if the conversion is in progress.
    bool hide_StartConversion = m_list->isBusy();

    // Hide actionStopConversion if nothing is being converted.
    bool hide_StopConversion = !m_list->isBusy();

    // Show actionOpenOutputFolder only if 1 file is selected.
    bool hide_OpenFolder = (selected_file_count <= 0);

    // Hide actionRemoveSelectedItems if no file is selected.
    bool hide_RemoveSelectedItems = (selected_file_count == 0);

    bool hide_Retry = (selected_file_count == 0);
    bool hide_RetryAll = (m_list->isEmpty());

    bool hide_ClearList = (m_list->isEmpty());

    bool hide_ChangeOutputFilename = m_list->selectedCount() != 1;
    bool hide_ChangeOutputDirectory = m_list->selectedCount() <= 0;

    ui->actionSetParameters->setDisabled(hide_SetParameters);
    ui->actionStartConversion->setDisabled(hide_StartConversion);
    ui->actionStopConversion->setDisabled(hide_StopConversion);
    ui->actionOpenOutputFolder->setDisabled(hide_OpenFolder);
    ui->actionRemoveSelectedItems->setDisabled(hide_RemoveSelectedItems);
    ui->actionRetry->setDisabled(hide_Retry);
    ui->actionRetryAll->setDisabled(hide_RetryAll);
    ui->actionClearList->setDisabled(hide_ClearList);
    ui->actionChangeOutputFilename->setDisabled(hide_ChangeOutputFilename);
    ui->actionChangeOutputDirectory->setDisabled(hide_ChangeOutputDirectory);
}
