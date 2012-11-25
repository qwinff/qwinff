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

#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QSettings>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->spinThreads->setMinimum(1); // at least 1 thread
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

bool OptionsDialog::exec()
{
    read_fields();
    bool accepted = (QDialog::exec() == QDialog::Accepted);
    if (accepted) {
        write_fields();
    }
    return accepted;
}

void OptionsDialog::read_fields()
{
    QSettings settings;
    ui->spinThreads->setValue(settings.value("options/threads", DEFAULT_THREAD_COUNT).toInt());
    ui->chkHideFormats->setChecked(settings.value("options/hideformats", true).toBool());
}

void OptionsDialog::write_fields()
{
    QSettings settings;
    settings.setValue("options/threads", ui->spinThreads->value());
    settings.setValue("options/hideformats", ui->chkHideFormats->isChecked());
}
