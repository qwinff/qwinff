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

#include <QMessageBox>
#include <QSettings>
#include <cmath>
#include "interactivecuttingdialog.h"
#include "ui_interactivecuttingdialog.h"
#include "mediaplayerwidget.h"
#include "rangeselector.h"
#include "timerangeedit.h"
#include "rangewidgetbinder.h"
#include "converter/exepath.h"
#include "converter/conversionparameters.h"

InteractiveCuttingDialog::InteractiveCuttingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InteractiveCuttingDialog),
    player(new MediaPlayerWidget(this)),
    m_rangeSel(new RangeSelector(this)),
    m_rangeEdit(new TimeRangeEdit(this))
{
    ui->setupUi(this);
    ui->layoutPlayer->addWidget(player);
    ui->layoutRangeSelector->addWidget(m_rangeSel);
    ui->layoutRangeEdit->addWidget(m_rangeEdit);

    // automatically sync between m_rangeSel and m_rangeEdit
    new RangeWidgetBinder(m_rangeSel, m_rangeEdit, this);

    connect(player, SIGNAL(stateChanged()), SLOT(playerStateChanged()));
    connect(ui->btnAsBegin, SIGNAL(clicked()), SLOT(set_as_begin()));
    connect(ui->btnAsEnd, SIGNAL(clicked()), SLOT(set_as_end()));
    connect(ui->btnToBegin, SIGNAL(clicked()), SLOT(seek_to_selection_begin()));
    connect(ui->btnToEnd, SIGNAL(clicked()), SLOT(seek_to_selection_end()));
    connect(ui->btnPlaySelection, SIGNAL(clicked()), SLOT(play_selection()));

    setFromBegin(true);
    setToEnd(true);
}

InteractiveCuttingDialog::~InteractiveCuttingDialog()
{
    delete ui;
}

bool InteractiveCuttingDialog::available()
{
    return ExePath::checkProgramAvailability("mplayer");
}

bool InteractiveCuttingDialog::fromBegin() const
{
    return m_rangeEdit->fromBegin();
}

bool InteractiveCuttingDialog::toEnd() const
{
    return m_rangeEdit->toEnd();
}

int InteractiveCuttingDialog::beginTime() const
{
    return m_rangeEdit->beginTime();
}

int InteractiveCuttingDialog::endTime() const
{
    return m_rangeEdit->endTime();
}

void InteractiveCuttingDialog::setFromBegin(bool from_begin)
{
    m_rangeEdit->setFromBegin(from_begin);
}

void InteractiveCuttingDialog::setToEnd(bool to_end)
{
    m_rangeEdit->setToEnd(to_end);
}

void InteractiveCuttingDialog::setBeginTime(int sec)
{
    m_rangeEdit->setBeginTime(sec);
}

void InteractiveCuttingDialog::setEndTime(int sec)
{
    m_rangeEdit->setEndTime(sec);
}

int InteractiveCuttingDialog::exec(const QString &filename)
{
    if (available()) {
        player->load(filename);
        return exec();
    } else {
        QMessageBox::critical(this, windowTitle(), tr("%1 not found").arg("mplayer"));
        return QDialog::Rejected;
    }
}

int InteractiveCuttingDialog::exec(const QString &filename, TimeRangeEdit *range)
{
    setBeginTime(range->beginTime());
    setEndTime(range->endTime());
    setFromBegin(range->fromBegin());
    setToEnd(range->toEnd());
    int status = exec(filename);
    if (status == QDialog::Accepted) {
        range->setBeginTime(beginTime());
        range->setEndTime(endTime());
        range->setFromBegin(fromBegin());
        range->setToEnd(toEnd());
    }
    return status;
}

int InteractiveCuttingDialog::exec(ConversionParameters *param)
{
    // TODO: extract the conversion logic to getter and setter in ConversionParameters
    // convert begin and duration to begin and end time
    setBeginTime(param->time_begin);
    setFromBegin(param->time_begin == 0);
    if (param->time_end > 0) { // time_end == 0 means "to end"
        setEndTime(param->time_end);
        setToEnd(false);
    } else {
        setToEnd(true);
    }
    int status = exec(param->source);
    // convert from begin and end time back to begin and duration
    if (status == QDialog::Accepted) {
        param->time_begin = fromBegin() ? 0 : beginTime();
        param->time_end = toEnd() ? 0 : endTime();
    }
    return status;
}

int InteractiveCuttingDialog::exec()
{
    load_settings();
    int status = QDialog::exec();
    save_settings();
    return status;
}

void InteractiveCuttingDialog::playerStateChanged()
{
    int duration = ceil(player->duration());
    if (player->ok() && duration != m_rangeEdit->maxTime()) {
        // get media duration and set limits
        // change range edit after visual selection
        m_rangeSel->setMaxValue(duration);
        m_rangeEdit->setMaxTime(duration);
    }
}

void InteractiveCuttingDialog::set_as_begin()
{
    m_rangeEdit->setBeginTime(floor(player->position()));
}

void InteractiveCuttingDialog::set_as_end()
{
    m_rangeEdit->setEndTime(ceil(player->position()));
}

void InteractiveCuttingDialog::seek_to_selection_begin()
{
    int begin_time = m_rangeEdit->beginTime();
    player->seek_and_pause(begin_time);
}

void InteractiveCuttingDialog::seek_to_selection_end()
{
    int end_time = m_rangeEdit->endTime();
    player->seek_and_pause(end_time);
}

void InteractiveCuttingDialog::play_selection()
{
    player->playRange(m_rangeEdit->beginTime(), m_rangeEdit->endTime());
}

void InteractiveCuttingDialog::load_settings()
{
    QSettings settings;
    restoreGeometry(settings.value("cutting_dialog/geometry").toByteArray());
}

void InteractiveCuttingDialog::save_settings()
{
    QSettings settings;
    settings.setValue("cutting_dialog/geometry", saveGeometry());
}
