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
#if defined(Q_WS_X11) && defined(USE_LIBNOTIFY)
 #include "services/notificationservice-libnotify.h"
#endif

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

    QTextBrowser *info = ui->txtInfo;
    QTextBrowser *translators = ui->txtTranslators;

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
#if defined(Q_WS_X11) && defined(USE_LIBNOTIFY) /* libnotify version */
                + "<br>" + tr("Compiled with libnotify %1")
                .arg(NotificationService_libnotify::getVersion())
#endif
         + "<br><br>"
         + tr("QWinFF is a gui frontend for FFmpeg.")
         + "<br><br>"
         + tr("Programming: %1").arg("Timothy Lin &lt;lzh9102@gmail.com&gt;") + "<br>"
         + tr("Logo Design: %1").arg("KuanYui &lt;azazabc123@gmail.com&gt;") + "<br><br>"
         + tr("This program is free software; you can redistribute it and/or modify it "
              "under the terms of the GNU General Public License version 2 or 3.")
         + "<br><br>"
         + tr("QWinFF Project Page: %1").arg(url("http://code.google.com/p/qwinff/"))
         + "<br><br>"
         + tr("FFmpeg presets were taken from WinFF.")
         + "<br><br>"
         );
    translators->setHtml(getTranslators());
    //translators->setText(getTranslators());

    // Constraint the width of text area to the width of the banner.
    //info->setMaximumWidth(ui->lblBanner->pixmap()->width());

    // Set the background color of the textbox to the color of the window.
    QPalette p = info->palette();
    p.setColor(QPalette::Base, this->palette().color(QPalette::Window));

    info->setPalette(p);
    info->setFrameShape(QTextBrowser::NoFrame);  // Hide textbox border.

    translators->setPalette(p);
    translators->setFrameShape(QTextBrowser::NoFrame);

    // Make the window size fixed.
    this->adjustSize();
    this->setMinimumWidth(this->width());
    this->setMinimumHeight(this->height());
    this->setMaximumWidth(this->width());
    this->setMaximumHeight(this->height());

    ui->tabInfo->setAutoFillBackground(true);
    ui->tabTranslators->setAutoFillBackground(true);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

QString AboutDialog::getTranslators()
{
    return QString(
        trad(tr("Japanese"), "Tilt <tiltstr@gmail.com>")
        );
}

QString AboutDialog::trad(const QString& lang, const QString& author)
{
    return trad(lang, QStringList() << author);
}

QString AboutDialog::trad(const QString& lang, const QStringList& authors)
{
    QString s = "<ul>";
    for (int n = 0; n < authors.count(); n++) {
        QString author = authors[n];
        s += "<li>"+ author.replace("<", "&lt;").replace(">", "&gt;")
                + "</li>";
    }
    s+= "</ul>";

    return QString("<b>%1</b>: %2").arg(lang).arg(s);
}
