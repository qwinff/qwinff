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
#include "poweroffdialog.h"
#include "services/paths.h"
#include "services/notification.h"
#include "services/powermanagement.h"
#include "converter/mediaconverter.h"
#include "converter/presets.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QDesktopServices>
#include <QApplication>
#include <QSettings>
#include <QCloseEvent>
#include <QTimer>
#include <QSignalMapper>
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

    load_settings();

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

    if (PowerManagement::implemented() && m_poweroff_button->isChecked()) {
        // show poweroff dialog
        if (PoweroffDialog(this).exec(get_poweroff_behavior()) == QDialog::Accepted) {
            save_settings(); // save settings in case of power failure
        }
    }
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

    save_settings();
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

void MainWindow::update_poweroff_button(int id)
{
    const char *icon_id = "";
    QString title = "";
    QString status_tip = "";
    switch (id) {
    case PowerManagement::SHUTDOWN:
        icon_id = ":/actions/icons/system_shutdown.png";
        title = tr("Shutdown");
        status_tip = tr("Shutdown when all tasks are done.");
        break;
    case PowerManagement::SUSPEND:
        icon_id = ":/actions/icons/system_suspend.png";
        title = tr("Suspend");
        status_tip = tr("Suspend when all tasks are done.");
        break;
    case PowerManagement::HIBERNATE:
        icon_id = ":/actions/icons/system_hibernate.png";
        title = tr("Hibernate");
        status_tip = tr("Hibernate when all tasks are done.");
        break;
    default:
        Q_ASSERT(!"Incorrect id! Be sure to handle every power action in switch().");
    }
    m_poweroff_button->setIcon(QIcon(icon_id));
    m_poweroff_button->setToolTip(title);
    m_poweroff_button->setStatusTip(status_tip);
    ui->actionPoweroff->setIcon(QIcon(icon_id));
    ui->actionPoweroff->setText(title);
    ui->actionPoweroff->setStatusTip(status_tip);
}

// Private Methods

/* Check if all execute conditions are met.
   This function should return true if all the conditions are met
   and return false if any of the conditions fails.
*/
bool MainWindow::check_execute_conditions()
{
    QString errmsg;

    // check external programs
    if (!MediaConverter::checkExternalPrograms(errmsg)) {
        QMessageBox::critical(this, tr("QWinFF"), errmsg);
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
    setup_poweroff_button();
}

void MainWindow::setup_statusbar()
{
    ui->statusBar->addPermanentWidget(m_elapsedTimeLabel);
}

/*
 * Setup the poweroff button and menu.
 * The poweroff button is handled differently from other menu and buttons.
 * Its icon and title changes as the action changes.
 */
void MainWindow::setup_poweroff_button()
{
    QToolButton *button = new QToolButton(this);
    QMenu *menu = new QMenu(this);
    QList<QAction*> actionList;
    QSignalMapper *signalMapper = new QSignalMapper(this);
    QActionGroup *checkGroup = new QActionGroup(this);

    m_poweroff_button = button;
    m_poweroff_actiongroup = checkGroup;

    // Insert all actions into the list.
    for (int i=0; i<PowerManagement::ACTION_COUNT; i++) {
        const char *icon_id = "";
        QString text = "";
        switch (i) {
        case PowerManagement::SHUTDOWN:
            //: Shutdown the computer (completely poweroff)
            text = tr("Shutdown");
            icon_id = ":/actions/icons/system_shutdown.png";
            break;
        case PowerManagement::SUSPEND:
            //: Suspend the computer (sleep to ram, standby)
            text = tr("Suspend");
            icon_id = ":/actions/icons/system_suspend.png";
            break;
        case PowerManagement::HIBERNATE:
            //: Hibernate the computer (sleep to disk, completely poweroff)
            text = tr("Hibernate");
            icon_id = ":/actions/icons/system_hibernate.png";
            break;
        default:
            Q_ASSERT(!"Incorrect id! Be sure to implement every power action in switch().");
        }
        actionList.append(new QAction(QIcon(icon_id)
                                      , text, this));
    }

    // Add all actions into the menu (from list)
    foreach (QAction *action, actionList) {
        menu->addAction(action);
        action->setCheckable(true);
        action->setActionGroup(checkGroup);
    }

    button->setMenu(menu);
    button->setPopupMode(QToolButton::MenuButtonPopup);

    // ensure that the toolbutton and actionPoweroff are both checkable
    ui->actionPoweroff->setCheckable(true);
    button->setCheckable(true);
    button->setChecked(false);

    /* Synchronize the checked state of the toolbutton and actionPoweroff.
       This cyclic connection doesn't cause an infinite loop because
       toggled(bool) is only triggered when the checked() state changes.
     */
    connect(button, SIGNAL(toggled(bool))
            , ui->actionPoweroff, SLOT(setChecked(bool)));
    connect(ui->actionPoweroff, SIGNAL(toggled(bool))
            , button, SLOT(setChecked(bool)));

    // update the poweroff button when the action changes
    for (int i=0; i<actionList.size(); i++) {
        QAction *action = actionList.at(i);
        signalMapper->setMapping(action, i);
        connect(action, SIGNAL(triggered())
                , signalMapper, SLOT(map()));
        connect(signalMapper, SIGNAL(mapped(int))
                , this, SLOT(update_poweroff_button(int)));
    }

    actionList.at(0)->trigger();

    /* Check if the power management functions are available.
       If not, hide poweroff button and menus.
     */
    if (!PowerManagement::implemented()) {
        m_poweroff_button->setVisible(false);
        ui->actionPoweroff->setVisible(false);
    } else {
        ui->toolBar->addWidget(button);
    }
}

void MainWindow::set_poweroff_behavior(int action)
{
    if (action >= PowerManagement::ACTION_COUNT)
        action = PowerManagement::SHUTDOWN;
    m_poweroff_actiongroup->actions().at(action)->trigger();
}

int MainWindow::get_poweroff_behavior()
{
    for (int i=0; i<m_poweroff_actiongroup->actions().size(); i++) {
        if (m_poweroff_actiongroup->actions().at(i)->isChecked())
            return i;
    }
    return PowerManagement::SHUTDOWN;
}

bool MainWindow::load_presets()
{
    // The default preset file is located in <datapath>/presets.xml
    QString default_preset_file = QDir(Paths::dataPath()).absoluteFilePath("presets.xml");

#ifndef PORTABLE_APP
    // rename local preset file created by older versions of qwinff
    // operation: mv ~/.qwinff/presets.xml ~/.qwinff/presets.xml.old
    QString local_preset_file_old = QDir(QDir::homePath()).absoluteFilePath(".qwinff/presets.xml");
    if (QFile(local_preset_file_old).exists()) {
        QFile::remove(local_preset_file_old + ".old");
        if (QFile::rename(local_preset_file_old, local_preset_file_old + ".old")) {
            qDebug() << local_preset_file_old + " is no longer used, "
                        "rename to " + local_preset_file_old + ".old";
        }
    }

    // use global preset temporarily
    QString local_preset_file = default_preset_file;
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

void MainWindow::load_settings()
{
    QSettings settings;
    restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
    restoreState(settings.value("mainwindow/state").toByteArray());
    int poweroff_behavior = settings.value("options/poweroff_behavior"
                                         , PowerManagement::SHUTDOWN).toInt();
    set_poweroff_behavior(poweroff_behavior);

}

void MainWindow::save_settings()
{
    QSettings settings;
    settings.setValue("mainwindow/geometry", saveGeometry());
    settings.setValue("mainwindow/state", saveState());
    settings.setValue("options/poweroff_behavior", get_poweroff_behavior());
}
