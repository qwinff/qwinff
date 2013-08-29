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
#include "converter/mediaprobe.h"
#include "services/ffplaypreviewer.h"
#include "rangeselector.h"
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
    ui(new Ui::ConversionParameterDialog),
    m_selTime(new RangeSelector(this)),
    m_ffplay(new FFplayPreviewer(this))
{
    ui->setupUi(this);

    ui->layoutTimeSel->addWidget(m_selTime);

    // Setup audio sample rate selection
    ui->cbAudioSampleRate->addItem("44100");
    ui->cbAudioSampleRate->addItem("22050");
    ui->cbAudioSampleRate->addItem("11025");

    ui->timeBegin->setSelectedSection(QDateTimeEdit::SecondSection);
    ui->timeEnd->setSelectedSection(QDateTimeEdit::SecondSection);

    // enable/disable time control widgets
    connect(ui->chkFromBegin, SIGNAL(toggled(bool)),
            this, SLOT(from_begin_toggled(bool)));
    connect(ui->chkToEnd, SIGNAL(toggled(bool)),
            this, SLOT(to_end_toggled(bool)));

    // synchronize time sliders and time selectors
    connect(ui->timeBegin, SIGNAL(timeChanged(QTime)),
            this, SLOT(sync_time_text_to_view()));
    connect(ui->timeEnd, SIGNAL(timeChanged(QTime)),
            this, SLOT(sync_time_text_to_view()));
    connect(m_selTime, SIGNAL(valueChanged()),
            this, SLOT(sync_time_view_to_text()));

    connect(ui->chkToEnd, SIGNAL(toggled(bool)),
            this, SLOT(update_endtime()));
    connect(ui->timeBegin, SIGNAL(timeChanged(QTime)),
            this, SLOT(update_endtime()));

    connect(ui->btnPreview, SIGNAL(clicked()),
            this, SLOT(preview_time_selection()));

    // Hide speed-changing options if sox is not available.
    m_enableAudioProcessing = AudioFilter::available();
    if (!m_enableAudioProcessing)
        ui->groupScaling->setVisible(false);
}

ConversionParameterDialog::~ConversionParameterDialog()
{
    delete ui;
}

bool ConversionParameterDialog::exec(ConversionParameters& param, bool single_file)
{
    m_singleFile = single_file;
    m_param = &param;
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

#define SECS_TO_QTIME(s) QTime(0, 0).addSecs(s)
#define QTIME_TO_SECS(t) (t).hour()*3600 + (t).minute()*60 + (t).second()

void ConversionParameterDialog::sync_time_view_to_text()
{
    bool fromBegin = (m_selTime->beginValue() == m_selTime->minValue());
    bool toEnd = (m_selTime->endValue() == m_selTime->maxValue());
    if (!fromBegin)
        ui->timeBegin->setTime(SECS_TO_QTIME(m_selTime->beginValue()));
    if (!toEnd)
        ui->timeEnd->setTime(SECS_TO_QTIME(m_selTime->endValue()));
    ui->chkFromBegin->setChecked(fromBegin);
    ui->chkToEnd->setChecked(toEnd);
}

void ConversionParameterDialog::sync_time_text_to_view()
{
    int beginSecs = QTIME_TO_SECS(ui->timeBegin->time());
    int endSecs = QTIME_TO_SECS(ui->timeEnd->time());
    if (ui->chkFromBegin->isChecked())
        beginSecs = m_selTime->minValue();
    if (ui->chkToEnd->isChecked())
        endSecs = m_selTime->maxValue();
    m_selTime->setBeginValue(beginSecs);
    m_selTime->setEndValue(endSecs);
}

void ConversionParameterDialog::from_begin_toggled(bool value)
{
    ui->timeBegin->setDisabled(value);
    if (value == true) {
        // from begin
        m_selTime->setBeginValue(m_selTime->minValue());
    } else {
        sync_time_text_to_view();
    }
}

void ConversionParameterDialog::to_end_toggled(bool value)
{
    ui->timeEnd->setDisabled(value);
    if (value == true) {
        // to end
        m_selTime->setEndValue(m_selTime->maxValue());
    } else {
        sync_time_text_to_view();
    }
}

void ConversionParameterDialog::preview_time_selection()
{
    int timeBegin = -1, timeEnd = -1;
    if (!ui->chkFromBegin->isChecked())
        timeBegin = QTIME_TO_SECS(ui->timeBegin->time());
    if (!ui->chkToEnd->isChecked())
        timeEnd = QTIME_TO_SECS(ui->timeEnd->time());
    m_ffplay->play(m_param->source, timeBegin, timeEnd);
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
    bool show_slider = false;
    if (m_singleFile) {
        // time slider: only show in single file mode
        MediaProbe probe;
        if (probe.run(param.source)) { // probe the source file, blocking call
            // success, show the slider
            m_selTime->setMaxValue((int)probe.mediaDuration());
            show_slider = true;
        }
    }
    m_selTime->setVisible(show_slider);
    bool show_preview_button = show_slider && m_ffplay->available();
    ui->btnPreview->setVisible(show_preview_button);

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

//#define QTIME_TO_SECS(t) ((t.hour()) * 3600 + (t.minute()) * 60 + (t.second()))

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
