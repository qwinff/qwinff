#include "addtaskwizard.h"
#include "ui_addtaskwizard.h"
#include "converter/presets.h"
#include "conversionparameterdialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <cassert>

#define NUM_RECENT_PATHS 5

#define PAGEID_SELECTFILES 0
#define PAGEID_PARAMS 1

AddTaskWizard::AddTaskWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::AddTaskWizard),
    m_presets(new Presets(this)),
    m_current_param(new ConversionParameters)
{
    ui->setupUi(this);

    connect(ui->btnAdd, SIGNAL(clicked())
            , this, SLOT(add_files()));
    connect(ui->btnRemove, SIGNAL(clicked())
            , this, SLOT(remove_files()));
    connect(ui->cbExtension, SIGNAL(currentIndexChanged(int))
            , this, SLOT(load_presets(int)));
    connect(ui->cbPreset, SIGNAL(currentIndexChanged(int))
            , this, SLOT(preset_selected(int)));
    connect(ui->btnEditPreset, SIGNAL(clicked())
            , this, SLOT(edit_preset()));
    connect(ui->btnBrowseOutputPath, SIGNAL(clicked())
            , this, SLOT(browse_output_path()));
    connect(this, SIGNAL(accepted())
            , this, SLOT(all_finished()));

    ui->lstFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);

    load_extensions("presets.xml");
    ui->cbOutputPath->setEditText(QDir::homePath());

    ui->cbOutputPath->setEditable(true);

    load_settings();

    QSettings settings;
    // Load Geometry
    restoreGeometry(settings.value("addtaskwizard/geometry").toByteArray());
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
        add_files();
        if (ui->lstFiles->count() == 0)
            return QWizard::Rejected;
    }
    return QWizard::exec();
}

int AddTaskWizard::exec(QList<QUrl> &files)
{
    ui->lstFiles->clear();

    foreach (QUrl url, files) {
        ui->lstFiles->addItem(url.path());
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
                                               "Please select another output folder")
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

void AddTaskWizard::add_files()
{
    /*: This text is the title of an openfile dialog. */
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Files"),
                                  m_prev_path,  // default path
                                  QString()    // TODO: filter
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

void AddTaskWizard::remove_files()
{
    QList<QListWidgetItem*> itemList = ui->lstFiles->selectedItems();
    foreach (QListWidgetItem *item, itemList) {
        ui->lstFiles->takeItem(ui->lstFiles->row(item));
    }
}

void AddTaskWizard::edit_preset()
{
    ConversionParameterDialog dialog(this->parentWidget());
    dialog.setGeometry(this->x(), this->y(), dialog.width(), dialog.height());
    dialog.exec(*m_current_param);
}

void AddTaskWizard::browse_output_path()
{
    /*: This text is the title of an open directory dialog. */
    ui->cbOutputPath->setEditText(
                QFileDialog::getExistingDirectory(this, tr("Select Directory")
                        , ui->cbOutputPath->currentText())
                );
}

// When the user selects an extension, insert all possible presets
// into the preset combobox.
void AddTaskWizard::load_presets(int index)
{
    if (index == -1) return;

    QString extension = ui->cbExtension->itemData(index).toString();
    QList<Preset> presetList;

    ui->cbPreset->clear();
    if (m_presets->getPresets(extension, presetList)) {
        qSort(presetList);  // Sort the presets by the order in the xml file.
        foreach (Preset preset, presetList) {
            ui->cbPreset->addItem(preset.label, preset.id);
        }
    }
}

void AddTaskWizard::preset_selected(int index)
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
void AddTaskWizard::all_finished()
{
    const int size = ui->lstFiles->count();
    m_params.clear();

    // All files share the same settings.
    ConversionParameters param(*m_current_param);
    QDir output_dir(ui->cbOutputPath->currentText());
    const int ext_index = ui->cbExtension->currentIndex();
    QString ext = ui->cbExtension->itemData(ext_index).toString();

    // Write conversion parameters to m_params.
    for (int i=0; i<size; i++) {

        QString input_filename = ui->lstFiles->item(i)->text();
        QString input_file_basename = QFileInfo(input_filename).completeBaseName();

        // Fill in input filename.
        param.source = input_filename;

        // Fill in output filename.
        param.destination =
                output_dir.absoluteFilePath(input_file_basename)   // filename
                + '.'
                + ext; // extension

        // Save the configuration for the file.
        m_params.append(param);
    }

    save_settings();
}

bool AddTaskWizard::load_extensions(const char *file)
{
    if (!m_presets->readFromFile(file)) {
        QMessageBox::critical(this, this->windowTitle(),
                              tr("Failed to load preset file."));
        return false;
    }

    // update extension combo bar
    QList<QString> extensions;
    m_presets->getExtensions(extensions);
    QList<QString>::iterator it = extensions.begin();
    ui->cbExtension->clear();
    for (; it!=extensions.end(); ++it) {
        ui->cbExtension->addItem(it->toUpper(), QVariant(*it));
    }

    return true;
}

void AddTaskWizard::load_settings()
{
    QSettings settings;

    // extension combobox
    QString ext = settings.value("addtaskwizard/extension").toString();
    for (int i=0; i<ui->cbExtension->count(); i++) {
        if (ui->cbExtension->itemText(i) == ext) {
            ui->cbExtension->setCurrentIndex(i);
            break;
        }
    }

    // preset combobox
    QApplication::processEvents();
    QString preset = settings.value("addtaskwizard/preset").toString();
    for (int i=0; i<ui->cbPreset->count(); i++) {
        if (ui->cbPreset->itemText(i) == preset) {
            ui->cbPreset->setCurrentIndex(i);
            break;
        }
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
    settings.setValue("addtaskwizard/extension", ui->cbExtension->currentText());
    settings.setValue("addtaskwizard/preset", ui->cbPreset->currentText());

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
