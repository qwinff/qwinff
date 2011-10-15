#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QtGlobal>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QTextBrowser *info = ui->txtInfo;

    info->setText(
         "<b>QWinFF</b> &copy; 2011 Timothy Lin &lt;lzh9102@gmail.com&gt;<br><br>"
         // TODO: add version info
         + tr("Compiled with Qt %1").arg(QT_VERSION_STR)

         );

    info->setFrameShape(QTextBrowser::NoFrame);

    ui->tabInfo->setAutoFillBackground(true);

    QPalette p = info->palette();
    p.setColor(QPalette::Base, ui->tabInfo->palette().color(QPalette::Window));

    info->setPalette(p);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
