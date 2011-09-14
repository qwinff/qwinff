#include "addtaskwizard.h"
#include "ui_addtaskwizard.h"
#include "converter/presets.h"
#include "conversionparameterdialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <cassert>

AddTaskWizard::AddTaskWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::AddTaskWizard),
    m_presets(new Presets(this)),
    m_current_param(new ConversionParameters)
{
    ui->setupUi(this);
    m_prev_path = QDir::homePath();

    connect(ui->btnAdd, SIGNAL(clicked())
            , this, SLOT(add_files_clicked()));
    connect(ui->btnRemove, SIGNAL(clicked())
            , this, SLOT(remove_files_clicked()));
    connect(ui->cbExtension, SIGNAL(currentIndexChanged(int))
            , this, SLOT(load_presets(int)));
    connect(ui->cbPreset, SIGNAL(currentIndexChanged(int))
            , this, SLOT(preset_selected(int)));
    connect(ui->btnEditPreset, SIGNAL(clicked())
            , this, SLOT(edit_preset_clicked()));
    connect(ui->btnBrowseOutputPath, SIGNAL(clicked())
            , this, SLOT(set_output_path_clicked()));
    connect(this, SIGNAL(accepted())
            , this, SLOT(all_finished()));

    ui->lstFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);

    load_extensions("presets.xml");
    ui->txtOutputPath->setText(QDir::homePath());

}

AddTaskWizard::~AddTaskWizard()
{
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

    if (startId() == 0) { // popup select file dialog
        add_files_clicked();
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

    setStartId(1);
    int ret = QWizard::exec();
    setStartId(0);

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

void AddTaskWizard::add_files_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files"),
                                  m_prev_path,  // default path
                                  QString()    // TODO: filter
                                  );
    if (!files.isEmpty()) {
        foreach (QString file, files) {
            QListWidgetItem *item = new QListWidgetItem(file);
            item->setToolTip(file);
            ui->lstFiles->addItem(item);
        }
    } else {
        // no file selected
    }
}

void AddTaskWizard::remove_files_clicked()
{
    QList<QListWidgetItem*> items = ui->lstFiles->selectedItems();
    QList<QListWidgetItem*>::iterator it = items.begin();
    for (; it!=items.end(); ++it) {
        ui->lstFiles->takeItem(ui->lstFiles->row(*it));
    }
}

void AddTaskWizard::edit_preset_clicked()
{
    ConversionParameterDialog dialog(this->parentWidget());
    dialog.setGeometry(this->x(), this->y(), dialog.width(), dialog.height());
    dialog.exec(*m_current_param);
}

void AddTaskWizard::set_output_path_clicked()
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
    QList<Preset> presets;

    ui->cbPreset->clear();
    if (m_presets->getPresets(extension, presets)) {
        QList<Preset>::iterator it = presets.begin();
        for (; it!=presets.end(); ++it) {
            ui->cbPreset->addItem(it->label, it->id);
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

    // all files share the same settings
    ConversionParameters param(*m_current_param);

    for (int i=0; i<size; i++) {

        QString input_filename = ui->lstFiles->item(i)->text();
        QString input_file_basename = QFileInfo(input_filename).completeBaseName();
        param.source = input_filename;

        // copy output filename to the parameter
        QDir output_dir(ui->txtOutputPath->text());
        const int ext_index = ui->cbExtension->currentIndex();
        param.destination =
                output_dir.absoluteFilePath(input_file_basename)   // filename
                + '.'
                + ui->cbExtension->itemData(ext_index).toString(); // extension

        m_params.append(param);
    }

}

void AddTaskWizard::load_extensions(const char *file)
{
    if (!m_presets->readFromFile(file)) {
        QMessageBox::critical(this, this->windowTitle(),
                              tr("Failed to load preset file."));
        return;
    }

    // update extension combo bar
    QList<QString> extensions;
    m_presets->getExtensions(extensions);
    QList<QString>::iterator it = extensions.begin();
    ui->cbExtension->clear();
    for (; it!=extensions.end(); ++it) {
        ui->cbExtension->addItem(*it, QVariant(*it));
    }


}
