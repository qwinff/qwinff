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

#include "conversionparameterdialog.h"
#include "converter/audiofilter.h"
#include "ui_conversionparameterdialog.h"
#include <QLayout>
#include <cmath>

#define TO_BYTEPERCENT(percent) ((percent) * 256 / 100)
#define TO_PERCENT(bytepercent) ((bytepercent) * 100 / 256)

#define DEFAULT_AUDIO_BITRATE 64
#define DEFAULT_VIDEO_BITRATE 200
#define DEFAULT_AUDIO_CHANNELS 2

ConversionParameterDialog::ConversionParameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConversionParameterDialog)
{
    ui->setupUi(this);

    // Setup audio sample rate selection
    ui->cbAudioSampleRate->addItem("44100");
    ui->cbAudioSampleRate->addItem("22050");
    ui->cbAudioSampleRate->addItem("11025");

    ui->timeBegin->setSelectedSection(QDateTimeEdit::SecondSection);
    ui->timeEnd->setSelectedSection(QDateTimeEdit::SecondSection);

    connect(ui->chkFromBegin, SIGNAL(toggled(bool)),
            ui->timeBegin, SLOT(setDisabled(bool)));
    connect(ui->chkToEnd, SIGNAL(toggled(bool)),
            ui->timeEnd, SLOT(setDisabled(bool)));
    connect(ui->chkToEnd, SIGNAL(toggled(bool)),
            this, SLOT(update_endtime()));
    connect(ui->timeBegin, SIGNAL(timeChanged(QTime)),
            this, SLOT(update_endtime()));

    // Hide speed-changing options if sox is not available.
    m_enableAudioProcessing = AudioFilter::available();
    if (!m_enableAudioProcessing)
        ui->groupScaling->setVisible(false);
}

ConversionParameterDialog::~ConversionParameterDialog()
{
    delete ui;
}

bool ConversionParameterDialog::exec(ConversionParameters& param)
{
    read_fields(param);
    bool accepted = (QDialog::exec() == QDialog::Accepted);
    if (accepted) {
        write_fields(param);
    }
    return accepted;
}

void ConversionParameterDialog::update_endtime()
{
    ui->timeEnd->setMinimumTime(ui->timeBegin->time().addSecs(1));
}

// read the fields from the ConversionParameters
void ConversionParameterDialog::read_fields(const ConversionParameters& param)
{
    // Additional Options
    ui->txtFFmpegOptions->setPlainText(param.ffmpeg_options);

    // Audio Options
    ui->chkDisableAudio->setChecked(param.disable_audio);

    ui->spinAudioBitrate->setValue(param.audio_bitrate);

    ui->cbAudioSampleRate->setEditText(QString::number(param.audio_sample_rate));

    ui->spinChannels->setValue(param.audio_channels);

    if (param.audio_volume)
        ui->spinVolume->setValue(TO_PERCENT(param.audio_volume));
    else
        ui->spinVolume->setValue(100);

    // Video Options
    ui->chkDisableVideo->setChecked(param.disable_video);

    ui->spinVideoBitrate->setValue(param.video_bitrate);

    ui->spinWidth->setValue(param.video_width);
    ui->spinHeight->setValue(param.video_height);

    ui->chkVideoSameQuality->setChecked(param.video_same_quality);
    ui->chkDeinterlace->setChecked(param.video_deinterlace);

    ui->spinCropTop->setValue(param.video_crop_top);
    ui->spinCropBottom->setValue(param.video_crop_bottom);
    ui->spinCropLeft->setValue(param.video_crop_left);
    ui->spinCropRight->setValue(param.video_crop_right);

    // Time Options
    if (param.time_begin > 0) {
        ui->chkFromBegin->setChecked(false);
        ui->timeBegin->setTime(QTime().addSecs(param.time_begin));
    } else {
        ui->chkFromBegin->setChecked(true);
        ui->timeBegin->setTime(QTime());
    }
    if (param.time_duration > 0) {
        ui->chkToEnd->setChecked(false);
        ui->timeEnd->setTime(QTime().addSecs(param.time_begin + param.time_duration));
    } else {
        ui->chkToEnd->setChecked(true);
        ui->timeEnd->setTime(QTime());
    }
    if (param.speed_scaling)
        ui->spinSpeedFactor->setValue(param.speed_scaling_factor * 100.0);
    else
        ui->spinSpeedFactor->setValue(100.0);

    // Subtitle Options
    //ui->chkDisableSubtitle->setChecked(param.disable_subtitle);
}

#define QTIME_TO_SECS(t) ((t.hour()) * 3600 + (t.minute()) * 60 + (t.second()))

// write the fields to the ConversionParameters
void ConversionParameterDialog::write_fields(ConversionParameters& param)
{
    // Additional Options
    param = param.fromFFmpegParameters(ui->txtFFmpegOptions->toPlainText());

    // Audio Options
    param.disable_audio = ui->chkDisableAudio->isChecked();
    param.audio_sample_rate = ui->cbAudioSampleRate->currentText().toInt();
    param.audio_bitrate = ui->spinAudioBitrate->value();
    param.audio_channels = ui->spinChannels->value();
    param.audio_volume = TO_BYTEPERCENT(ui->spinVolume->value());

    // Video Options
    param.disable_video = ui->chkDisableVideo->isChecked();
    param.video_bitrate = ui->spinVideoBitrate->value();
    param.video_same_quality = ui->chkVideoSameQuality->isChecked();
    param.video_deinterlace = ui->chkDeinterlace->isChecked();

    param.video_width = ui->spinWidth->value();
    param.video_height = ui->spinHeight->value();

    param.video_crop_top = ui->spinCropTop->value();
    param.video_crop_bottom = ui->spinCropBottom->value();
    param.video_crop_left = ui->spinCropLeft->value();
    param.video_crop_right = ui->spinCropRight->value();

    // Time Options
    if (ui->chkFromBegin->isChecked())
        param.time_begin = 0;
    else
        param.time_begin = QTIME_TO_SECS(ui->timeBegin->time());
    if (ui->chkToEnd->isChecked()) // ffmpeg accepts duration, not end time
        param.time_duration = 0;
    else
        param.time_duration = QTIME_TO_SECS(ui->timeEnd->time()) - param.time_begin;
    double speed_ratio = ui->spinSpeedFactor->value();
    if (!m_enableAudioProcessing || std::abs(speed_ratio - 100.0) <= 0.01) {
        param.speed_scaling = false;
        param.speed_scaling_factor = 1.0;
    } else {
        param.speed_scaling = true;
        param.speed_scaling_factor = speed_ratio / 100.0;
    }

}
