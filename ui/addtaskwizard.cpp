#include "addtaskwizard.h"
#include "ui_addtaskwizard.h"
#include "converter/presets.h"
#include "conversionparameterdialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <cassert>

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
    ui->txtOutputPath->setText(QDir::homePath());

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
                                     , tr("Please select at least one file.")
                                     , QMessageBox::Ok);
            return false;
        }
        break;
    case 1: // Select conversion parameters
        // check if output directory exists
        QDir output_dir(ui->txtOutputPath->text());
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
                                               "Please select another folder")
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
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files"),
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

        if (!incorrect_files.isEmpty()) { // TODO: Use a messagebox with a textbox.
            QMessageBox::warning(this, this->windowTitle(),
                         tr("Cannot find the following files:") + "\n\n"
                          + incorrect_files.join("\n"), QMessageBox::Ok);
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
    ui->txtOutputPath->setText(
                QFileDialog::getExistingDirectory(this, tr("Select Directory")
                        , ui->txtOutputPath->text())
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

    // Write conversion parameters to m_params.
    for (int i=0; i<size; i++) {

        QString input_filename = ui->lstFiles->item(i)->text();
        QString input_file_basename = QFileInfo(input_filename).completeBaseName();

        // Fill in input filename.
        param.source = input_filename;

        // Fill in output filename.
        QDir output_dir(ui->txtOutputPath->text());
        const int ext_index = ui->cbExtension->currentIndex();
        param.destination =
                output_dir.absoluteFilePath(input_file_basename)   // filename
                + '.'
                + ui->cbExtension->itemData(ext_index).toString(); // extension

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
        ui->cbExtension->addItem(*it, QVariant(*it));
    }

    return true;
}

void AddTaskWizard::load_settings()
{
    QSettings settings;

    // output path textbox
    QString output_path = settings.value("addtaskwizard/output_path").toString();
    if (output_path.isEmpty()) {
        output_path = QDir::homePath();
    }
    ui->txtOutputPath->setText(output_path);

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
    m_prev_path = settings.value("addtaskwizard/openfilepath").toString();
    if (m_prev_path.isEmpty()) {
        m_prev_path = QDir::homePath();
    }

}

void AddTaskWizard::save_settings()
{
    QSettings settings;
    settings.setValue("addtaskwizard/output_path", ui->txtOutputPath->text());
    settings.setValue("addtaskwizard/extension", ui->cbExtension->currentText());
    settings.setValue("addtaskwizard/preset", ui->cbPreset->currentText());
}
