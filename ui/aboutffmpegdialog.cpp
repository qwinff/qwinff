#include "aboutffmpegdialog.h"
#include "ui_aboutffmpegdialog.h"
#include "converter/ffmpeginterface.h"

AboutFFmpegDialog::AboutFFmpegDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutFFmpegDialog)
{
    ui->setupUi(this);

    ui->txtFFmpegVersionInfo->setText(FFmpegInterface::getFFmpegVersionInfo());
    ui->txtFFmpegVersionInfo->setReadOnly(true);

    ui->txtFFmpegCodecInfo->setText(FFmpegInterface::getFFmpegCodecInfo());
    ui->txtFFmpegCodecInfo->setReadOnly(true);

}

AboutFFmpegDialog::~AboutFFmpegDialog()
{
    delete ui;
}
