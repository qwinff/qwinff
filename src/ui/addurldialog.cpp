#include "ui/addurldialog.h"
#include "ui_addurldialog.h"

AddUrlDialog::AddUrlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUrlDialog)
{
    ui->setupUi(this);
}

AddUrlDialog::~AddUrlDialog()
{
    delete ui;
}

QString AddUrlDialog::getUrl(bool *ok)
{
    show();

    int ret = exec();

    if (ok) {
        *ok = ret;
    }

    if (ret) {
        return ui->url->text();
    }

    return QString();
}
