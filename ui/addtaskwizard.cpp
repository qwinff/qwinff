#include "addtaskwizard.h"
#include "ui_addtaskwizard.h"
#include "converter/presets.h"
#include "conversionparameterdialog.h"

#include <QMessageBox>
#include <QFileDialog>
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

    ui->lstFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);

    load_extensions("presets.xml");
    ui->txtOutputPath->setText(QDir::homePath());
}

AddTaskWizard::~AddTaskWizard()
{
    delete ui;
}

void AddTaskWizard::add_files_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files"),
                                  m_prev_path,  // default path
                                  QString()    // TODO: filter
                                  );
    if (!files.isEmpty()) {
        for (int i=0; i<files.size(); i++) {
            ui->lstFiles->addItem(files[i]);
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

}

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
