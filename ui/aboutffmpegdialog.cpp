#include "aboutffmpegdialog.h"
#include "ui_aboutffmpegdialog.h"
#include "converter/ffmpeginterface.h"

AboutFFmpegDialog::AboutFFmpegDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutFFmpegDialog)
{
    ui->setupUi(this);

    ui->lblVersionInfo->setText(FFmpegInterface::getFFmpegVersionInfo());

    ui->txtCodecInfo->setText(FFmpegInterface::getFFmpegCodecInfo());
    ui->txtCodecInfo->setReadOnly(true);

}

AboutFFmpegDialog::~AboutFFmpegDialog()
{
    delete ui;
}
