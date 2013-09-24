/*  QWinFF - a qt4 gui frontend for ffmpeg
 *  Copyright (C) 2011-2013 Timothy Lin <lzh9102@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QSettings>
#include "updatedialog.h"
#include "ui_updatedialog.h"
#include "services/updatechecker.h"
#include "services/constants.h"

UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    m_updateChecker(0),
    ui(new Ui::UpdateDialog)
{
    ui->setupUi(this);
    connect(ui->btnReleaseNotes, SIGNAL(toggled(bool)),
            this, SLOT(slotToggleReleaseNotes(bool)));
    connect(ui->chkCheckUpdateOnStartup, SIGNAL(toggled(bool)),
            this, SLOT(slotToggleCheckUpdateOnStartup(bool)));
    // read check_update_on_startup setting to chkCheckUpdateOnStartup
    ui->chkCheckUpdateOnStartup->setChecked(
                QSettings().value("options/check_update_on_startup",
                                  Constants::getBool("CheckUpdateOnStartup")).toBool());
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

int UpdateDialog::exec(UpdateChecker &uc)
{
    m_updateChecker = &uc;
    return exec();
}

int UpdateDialog::exec()
{
    Q_ASSERT(m_updateChecker);
    connect(m_updateChecker, SIGNAL(receivedResult(int)),
            this, SLOT(slotReceivedUpdateResult(int)));
    if (!m_updateChecker->hasUpdate())
        checkUpdate();
    else
        updateFound();
    int status = QDialog::exec();
    disconnect(m_updateChecker, SIGNAL(receivedResult(int)),
               this, SLOT(slotReceivedUpdateResult(int)));
    return status;
}

void UpdateDialog::checkUpdate()
{
    Q_ASSERT(m_updateChecker);
    ui->btnReleaseNotes->setVisible(false);
    ui->btnReleaseNotes->setChecked(false);
    ui->txtReleaseNotes->setVisible(false);
    ui->lblStatus->setText(tr("Downloading update information..."));
    m_updateChecker->checkUpdate();
    resizeToFit();
}

void UpdateDialog::updateFound()
{
    Q_ASSERT(m_updateChecker);
    ui->btnReleaseNotes->setVisible(true);
    ui->btnReleaseNotes->setChecked(false);
    ui->txtReleaseNotes->setVisible(false);
    ui->lblStatus->setText(get_status());
    ui->txtReleaseNotes->setHtml(m_updateChecker->releaseNotes());
    resizeToFit();
}

void UpdateDialog::slotReceivedUpdateResult(int result)
{
    Q_ASSERT(m_updateChecker);
    if (m_updateChecker->hasUpdate()) {
        updateFound();
        return;
    }
    QString message;
    switch (result)
    {
    case UpdateChecker::ConnectionError:
        message = tr("Cannot connect to server.");
        break;
    case UpdateChecker::DataError:
        message = tr("Failed to parse the received data.");
        break;
    case UpdateChecker::UpdateNotFound:
        message = tr("You are already using the latest version of QWinFF.");
        break;
    default:
        message = tr("An unknown error has occurred.");
    }
    ui->lblStatus->setText(QString("%1").arg(message));
    resizeToFit();
}

void UpdateDialog::slotToggleReleaseNotes(bool checked)
{
    // show/hide release notes
    ui->txtReleaseNotes->setVisible(checked);
    resizeToFit();
}

void UpdateDialog::slotToggleCheckUpdateOnStartup(bool checked)
{
    QSettings settings;
    settings.setValue("options/check_update_on_startup", checked);
}

void UpdateDialog::resizeToFit()
{
    resize(width(), ui->lblStatus->height() + ui->btnReleaseNotes->height());
    resize(sizeHint());
}

QString UpdateDialog::get_status()
{
    QStringList result;
    result << tr("A new version of QWinFF has been released!");
    result << "<br>";
    //: %1 is version number, %2 is the project homepage
    result << tr("Version <b>%1</b> is available at %2.")
              .arg(m_updateChecker->versionName(),
                   link(m_updateChecker->downloadPage()));
    QString url = m_updateChecker->downloadUrl();
    if (!url.isEmpty()) {
        result << "<br>";
        result << tr("You can download this version using the link:");
        result << "<br><br>";
        result << link(m_updateChecker->downloadUrl());
    }
    return result.join("");
}

QString UpdateDialog::link(const QString &s)
{
    return QString("<a href=\"%1\">%1</a>").arg(s);
}
