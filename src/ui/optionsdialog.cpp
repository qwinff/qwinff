#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QSettings>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->spinThreads->setMinimum(1); // at least 1 thread
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

bool OptionsDialog::exec()
{
    read_fields();
    bool accepted = (QDialog::exec() == QDialog::Accepted);
    if (accepted) {
        write_fields();
    }
    return accepted;
}

void OptionsDialog::read_fields()
{
    QSettings settings;
    ui->spinThreads->setValue(settings.value("options/threads", 1).toInt());
}

void OptionsDialog::write_fields()
{
    QSettings settings;
    settings.setValue("options/threads", ui->spinThreads->value());
}
