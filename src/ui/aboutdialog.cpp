/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"
#include <QtGlobal>

namespace {
QString url(QString lnk)
{
    return QString("<a href=\"%1\">%1</a>").arg(lnk);
}
}

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QLabel *info = ui->lblInfo;

    info->setOpenExternalLinks(true);

    info->setText(
         "<b>QWinFF</b> &copy; 2011 Timothy Lin &lt;lzh9102@gmail.com&gt;<br>"
         + tr("Version: %1").arg(VERSION_STRING)
#ifdef VERSION_ID_STRING
         + QString(" %1").arg(QString(VERSION_ID_STRING))
#endif
         + "<br>"
         /*: Qt version */
         + tr("Compiled with Qt %1").arg(QT_VERSION_STR)
         + "<br><br>"
         + tr("QWinFF is a gui frontend for FFmpeg.")
         + "<br><br>"
         + tr("Programming: %1").arg("Timothy Lin") + "<br>"
         + tr("Logo Design: %1").arg("KuanYui") + "<br><br>"
         + tr("This program is free software; you can redistribute it and/or modify it "
              "under the terms of the GNU General Public License version 2 or 3.")
         + "<br><br>"
         + tr("QWinFF Project Page: %1").arg(url("http://code.google.com/p/qwinff/"))
         + "<br>"
         + tr("FFmpeg presets were taken from WinFF.")
         + "<br><br>"
         /* TODO: add project page information */
         );

    // Constraint the width of text area to the width of the banner.
    info->setMaximumWidth(ui->lblBanner->pixmap()->width());
    info->setWordWrap(true);

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
