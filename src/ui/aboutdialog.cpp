#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QtGlobal>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QLabel *info = ui->lblInfo;

    info->setText(
         "<b>QWinFF</b> &copy; 2011 Timothy Lin &lt;lzh9102@gmail.com&gt;<br><br>"
         + tr("An intuitive GUI frontend to FFmpeg.")
         + "<br><br>"
         // TODO: add version info
         + tr("Programming: %1").arg("Timothy Lin") + "<br>"
         + tr("Logo Design: %1").arg("KuanYui") + "<br><br>"
         /*: Qt version */
         + tr("Compiled with Qt %1").arg(QT_VERSION_STR)
         + "<br><br>"
         + tr("FFmpeg presets are taken from WinFF.")

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
