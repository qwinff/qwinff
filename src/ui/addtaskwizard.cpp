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

#include "addtaskwizard.h"
#include "ui_addtaskwizard.h"

#include "converter/presets.h"
#include "conversionparameterdialog.h"
#include "services/extensions.h"
#include "services/paths.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <cassert>

#define NUM_RECENT_PATHS 5

#define PAGEID_SELECTFILES 0
#define PAGEID_PARAMS 1

AddTaskWizard::AddTaskWizard(Presets *presets, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::AddTaskWizard),
    m_presets(presets),
    m_current_param(new ConversionParameters)
{
    ui->setupUi(this);

    // setup signals/slots
    connect(ui->btnAdd, SIGNAL(clicked())
            , this, SLOT(slotAddFilesToList()));
    connect(ui->btnRemove, SIGNAL(clicked())
            , this, SLOT(slotRemoveFilesFromList()));
    connect(ui->cbExtension, SIGNAL(currentIndexChanged(int))
            , this, SLOT(slotExtensionSelected(int)));
    connect(ui->cbPreset, SIGNAL(currentIndexChanged(int))
            , this, SLOT(slotPresetSelected(int)));
    connect(ui->btnEditPreset, SIGNAL(clicked())
            , this, SLOT(slotEditPresetButton()));
    connect(ui->btnBrowseOutputPath, SIGNAL(clicked())
            , this, SLOT(slotBrowseOutputPathButton()));
    connect(this, SIGNAL(accepted())
            , this, SLOT(slotFinished()));

    ui->lstFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);

    load_extensions();
    ui->cbOutputPath->setEditText(QDir::homePath());

    ui->cbOutputPath->setEditable(true);

    load_settings();

    QSettings settings;
    // Load Geometry
    restoreGeometry(settings.value("addtaskwizard/geometry").toByteArray());

    // Hide "auto adjust bitrate" checkbox.
    ui->chkAutoAdjustBitrate->setVisible(false);
}

AddTaskWizard::~AddTaskWizard()
{
    QSettings settings;
    // Save Geometry
    settings.setValue("addtaskwizard/geometry", saveGeometry());
    delete ui;
}

const QList<ConversionParameters>&
AddTaskWizard::getConversionParameters() const
{
    return m_params;
}

int AddTaskWizard::exec_openfile()
{
    ui->lstFiles->clear();

    if (startId() == PAGEID_SELECTFILES) { // popup select file dialog
        slotAddFilesToList();
        if (ui->lstFiles->count() == 0)
            return QWizard::Rejected;
    }
    return QWizard::exec();
}

int AddTaskWizard::exec(QList<QUrl> &files)
{
    ui->lstFiles->clear();

    foreach (QUrl url, files) {
        ui->lstFiles->addItem(url.toLocalFile());
    }

    int prev_id = startId();
    setStartId(PAGEID_PARAMS);
    int ret = QWizard::exec();
    setStartId(prev_id);

    return ret;
}

bool AddTaskWizard::validateCurrentPage()
{
    switch (currentId()) {
    case 0: // Select input files
        // check if the list is empty
        if (ui->lstFiles->count() != 0) { // complete
            return true;
        } else {
            QMessageBox::information(this, this->windowTitle()
                                     , tr("Please select at least one file."));
            return false;
        }
        break;
    case 1: // Select conversion parameters
        // check if output directory exists
        QDir output_dir(ui->cbOutputPath->currentText());
        if (output_dir.exists()) {
            return true;
        } else { // The folder doesn't exist.
            // Prompt the user to create new folder.
            QMessageBox::StandardButton reply =
                    QMessageBox::warning(this, this->windowTitle()
                                 , tr("Folder does not exist. Create a new folder?")
                                 , QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // The user chooses to create folder.
                qDebug() << "Try to create folder " << output_dir.path();
                bool succeed = QDir().mkpath(output_dir.path());
                if (!succeed) { // failed to create folder
                    QMessageBox::critical(this, this->windowTitle()
                                          , tr("Failed to create folder. "
                                               "Please select another output folder.")
                                          , QMessageBox::Ok);
                }
                return succeed;
            }
            else {
                return false;
            }

        }
        return true;
        break;
    }
    return true;
}

void AddTaskWizard::slotAddFilesToList()
{
    Extensions exts;
    /*: This text is the title of an openfile dialog. */
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Files"),
              m_prev_path,  // default path
              tr("Multimedia") + exts.multimedia().forFilter() + ";;" +
              tr("Video") + exts.video().forFilter() + ";;" +
              tr("Audio") + exts.audio().forFilter() + ";;" +
              tr("All files") + " (*)"
              );

    if (!files.isEmpty()) {
        QStringList incorrect_files; // Record files that are not valid for conversion.

        foreach (QString file, files) {
            if (QFileInfo(file).isFile()) {       // The file exists.
                QListWidgetItem *item = new QListWidgetItem(file);
                item->setToolTip(file);
                ui->lstFiles->addItem(item);

                m_prev_path = QFileInfo(file).path(); // save file path
            } else if (QFileInfo(file).isDir()) { // The filename is a directory.
                incorrect_files.append(file);
            } else {                              // The file does not exist.
                incorrect_files.append(file);
            }
        }

        if (!incorrect_files.isEmpty()) {
            QMessageBox msgBox;
            msgBox.setText(tr("Some files could not be found."));
            msgBox.setDetailedText(incorrect_files.join("\n"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }

        // Save open file path.
        QSettings settings;
        settings.setValue("addtaskwizard/openfilepath", m_prev_path);

    } else {
        // no file selected
    }
}

void AddTaskWizard::slotRemoveFilesFromList()
{
    QList<QListWidgetItem*> itemList = ui->lstFiles->selectedItems();
    foreach (QListWidgetItem *item, itemList) {
        ui->lstFiles->takeItem(ui->lstFiles->row(item));
    }
}

void AddTaskWizard::slotEditPresetButton()
{
    ConversionParameterDialog dialog(this->parentWidget());
    dialog.setGeometry(this->x(), this->y(), dialog.width(), dialog.height());

    m_current_param->audio_auto_bitrate = ui->chkAutoAdjustBitrate->isChecked();

    if (dialog.exec(*m_current_param)) {
        m_cbpreset_index = ui->cbPreset->currentIndex();
        ui->cbPreset->setCurrentIndex(-1); // select no item
    }

    ui->chkAutoAdjustBitrate->setChecked(m_current_param->audio_auto_bitrate);
}

void AddTaskWizard::slotBrowseOutputPathButton()
{
    /*: This text is the title of an open directory dialog. */
    QString selected_path = QFileDialog::getExistingDirectory(this, tr("Select Directory")
                                        , ui->cbOutputPath->currentText());
    if (!selected_path.isEmpty())
        ui->cbOutputPath->setEditText(selected_path);
}

// When the user selects an extension, insert all possible presets
// into the preset combobox.
void AddTaskWizard::slotExtensionSelected(int ext_index)
{
    if (ext_index == -1) return;

    QString extension = ui->cbExtension->itemData(ext_index).toString();
    QList<Preset> presetList;

    ui->cbPreset->clear();
    if (m_presets->getPresets(extension, presetList)) {
        qSort(presetList);  // Sort the presets by the order in the xml file.
        foreach (Preset preset, presetList) {
            ui->cbPreset->addItem(preset.label, preset.id);
        }
    }

    // Restore the last used preset of the extension.
    if (ext_index >= 0 && ext_index < m_ext_preset.size())
        ui->cbPreset->setCurrentIndex(m_ext_preset[ext_index].toInt());
}

void AddTaskWizard::slotPresetSelected(int index)
{
    if (index == -1) return;
    int id = ui->cbPreset->itemData(index).toUInt();
    Preset preset;
    if (!m_presets->findPresetById(id, preset)) {
        return; // assert false
    }
    *m_current_param = ConversionParameters::fromFFmpegParameters(preset.parameters);
}

// This function is executed when the users presses "Finish"
void AddTaskWizard::slotFinished()
{
    const int size = ui->lstFiles->count();
    m_params.clear();

    // This variable is reused in the loop.
    // Only "source" and "destination" should be modified in the loop.
    ConversionParameters param(*m_current_param);
    const QDir output_dir(ui->cbOutputPath->currentText());
    const int ext_index = ui->cbExtension->currentIndex();
    const QString ext = ui->cbExtension->itemData(ext_index).toString();

    param.audio_auto_bitrate = ui->chkAutoAdjustBitrate->isChecked();

    // Write conversion parameters to m_params.
    for (int i=0; i<size; i++) {

        QString input_filename = ui->lstFiles->item(i)->text();
        QString input_file_basename = QFileInfo(input_filename).completeBaseName();

        // Fill in input filename.
        param.source = input_filename;

        // Generate output filename.
        param.destination = output_dir.absoluteFilePath(input_file_basename + "." + ext);

        // Save the configuration for the file.
        m_params.append(param);
    }

    save_settings();
}

bool AddTaskWizard::load_extensions()
{
    // update extension combo bar
    QList<QString> extensions;
    m_presets->getExtensions(extensions);
    ui->cbExtension->clear();
    foreach (QString ext, extensions) {
        ui->cbExtension->addItem(ext.toUpper(), QVariant(ext));
    }

    return true;
}

void AddTaskWizard::load_settings()
{
    QSettings settings;

    // extension combobox
    int ext_index = settings.value("addtaskwizard/extension").toInt();
    if (ext_index < 0 || ext_index >= ui->cbExtension->count())
        ext_index = 0;
    ui->cbExtension->setCurrentIndex(ext_index);

    m_ext_preset = settings.value("addtaskwizard/selected_presets").toList().toVector();
    m_ext_preset.resize(ui->cbExtension->count());

    //ui->chkAutoAdjustBitrate->setChecked(
    //      settings.value("addtaskwizard/auto_audio_bitrate", false).toBool());
    ui->chkAutoAdjustBitrate->setChecked(false);

    if (ext_index >= 0 && ext_index < m_ext_preset.size()) {
        // preset combobox
        QApplication::processEvents();

        int preset_index = m_ext_preset[ext_index].toInt();
        if (preset_index < 0 || preset_index >= ui->cbPreset->count())
            preset_index = 0;

        ui->cbPreset->setCurrentIndex(preset_index);
    }

    // open file dialog default path
    m_prev_path = settings.value("addtaskwizard/openfilepath"
                                 , QDir::homePath()).toString();

    // Load recent output paths.
    QStringList recent_paths = settings.value("addtaskwizard/recentpaths").toStringList();
    ui->cbOutputPath->clear();
    ui->cbOutputPath->addItems(recent_paths);
    if (ui->cbOutputPath->count() == 0) {
        ui->cbOutputPath->addItem(QDir::homePath());
    }
    ui->cbOutputPath->setCurrentIndex(0); // Select the most recent path.


}

void AddTaskWizard::save_settings()
{
    QSettings settings;
    settings.setValue("addtaskwizard/extension", ui->cbExtension->currentIndex());

    const int ext_index = ui->cbExtension->currentIndex();
    int preset_index = ui->cbPreset->currentIndex();

    if (preset_index < 0) // The user has edited the preset, so no preset is selected.
        preset_index = m_cbpreset_index; // Save the last selected preset instead.

    if (ext_index >= 0 && ext_index < m_ext_preset.size())
        m_ext_preset[ext_index] = preset_index;

    //settings.setValue("addtaskwizard/auto_audio_bitrate", ui->chkAutoAdjustBitrate->isChecked());

    // the last used preset of each extension
    settings.setValue("addtaskwizard/selected_presets", m_ext_preset.toList());

    // Save recent output paths
    QStringList recent_paths;
    recent_paths.push_back(ui->cbOutputPath->currentText()); // Save current text.
    for (int i=0; i<ui->cbOutputPath->count(); i++) {
        recent_paths.push_back(ui->cbOutputPath->itemText(i));
    }
    recent_paths.removeDuplicates();
    if (recent_paths.size() > NUM_RECENT_PATHS) {
        // Make the list contain at most NUM_RECENT_PATHS items.
        recent_paths = recent_paths.mid(0, NUM_RECENT_PATHS);
    }
    settings.setValue("addtaskwizard/recentpaths", recent_paths);
}
