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
#include <QLocale>
#ifdef USE_LIBNOTIFY
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
#ifdef PORTABLE_APP
         /*: Portable version (no installation, no writing registry) */
         + " " + tr("Portable")
#endif
         + "<br>"
         /*: Qt version */
         + tr("Compiled with Qt %1").arg(QT_VERSION_STR)
#ifdef USE_LIBNOTIFY /*: libnotify version */
                + "<br>" + tr("Compiled with libnotify %1")
                .arg(NotificationService_libnotify::getVersion())
#endif
         + "<br><br>"
         + tr("QWinFF is a gui frontend for FFmpeg.")
         + "<br><br>"
         //: %1 is the name and email of the programmer
         + tr("Programming: %1").arg("Timothy Lin &lt;lzh9102@gmail.com&gt;") + "<br>"
         //: %1 is the name and email of the logo designer
         + tr("Logo Design: %1").arg("kuanyui &lt;azazabc123@gmail.com&gt;") + "<br><br>"
         + tr("This program is free software; you can redistribute it and/or modify it "
              "under the terms of the GNU General Public License version 2 or 3.")
         + "<br><br>"
         + tr("QWinFF Homepage: %1").arg(url("http://qwinff.blogspot.com"))
         + "<br><br>"
         + tr("Some audio-processing functionalities are provided by SoX.")
         + " (" + url("http://sox.sourceforge.net/") + ")<br><br>"
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
    struct {QString locale; QString translator;} table[] = {
        //: Japanese Language
        {"ja_JP",trad(tr("Japanese"), "Tilt <tiltstr@gmail.com>")},
        //: Italian Language
        {"it_IT",trad(tr("Italian"), QStringList()
               << "TheJoe (http://thejoe.it/)"
               << "Francesco Marinucci <framarinucci@gmail.com>")},
        //: Czech Language
        {"cs_CZ", trad(tr("Czech"), "Petr Ga&#271;orek <hahihula@gmail.com>")},
        //: Chinese character set used in China
        {"zh_CN", trad(tr("Simplified Chinese")
                       , "&#26472;&#27704;&#26126; <linuxfedora17@gmail.com>")},
        //: Russian Language
        {"ru", trad(tr("Russian")
                       , "Stanislav Hanzhin <stanislav.hanzhin@rosalab.ru>")},
    };
    const int size = sizeof(table) / sizeof(table[0]);

    QStringList translators;
    QString current_locale = QLocale::system().name();
    for (int i=0; i<size; i++) {
        if (current_locale.startsWith(table[i].locale))
            translators.push_front(table[i].translator);
        else
            translators.push_back(table[i].translator);
    }

    return translators.join("");
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
