#include "addtaskwizard.h"
#include "ui_addtaskwizard.h"

AddTaskWizard::AddTaskWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::AddTaskWizard)
{
    ui->setupUi(this);
}

AddTaskWizard::~AddTaskWizard()
{
    delete ui;
}
