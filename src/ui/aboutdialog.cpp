#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"
#include <QtGlobal>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QLabel *info = ui->lblInfo;

    info->setText(
         "<b>QWinFF</b> &copy; 2011 Timothy Lin &lt;lzh9102@gmail.com&gt;<br>"
         + tr("Version: %1").arg(VERSION_STRING)
         + "<br><br>"
         + tr("QWinFF is a gui frontend for FFmpeg.")
         + "<br><br>"
         + tr("Programming: %1").arg("Timothy Lin") + "<br>"
         + tr("Logo Design: %1").arg("KuanYui") + "<br><br>"
         /*: Qt version */
         + tr("Compiled with Qt %1").arg(QT_VERSION_STR)
         + "<br><br>"
         + tr("FFmpeg presets were taken from WinFF.")

         );

    // Make the window size fixed.
    this->adjustSize();
    this->setMinimumWidth(this->width());
    this->setMinimumHeight(this->height());
    this->setMaximumWidth(this->width());
    this->setMaximumHeight(this->height());

}

AboutDialog::~AboutDialog()
{
    delete ui;
}
