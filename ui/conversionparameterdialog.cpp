#include "conversionparameterdialog.h"
#include "ui_conversionparameterdialog.h"
#include <QLayout>


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

// read the fields from the ConversionParameters
void ConversionParameterDialog::read_fields(const ConversionParameters& param)
{
    // Additional Options
    ui->txtFFmpegOptions->setPlainText(param.ffmpeg_options);

    // Audio Options
    ui->chkDisableAudio->setChecked(param.disable_audio);

    if (param.audio_bitrate)
        ui->spinAudioBitrate->setValue(param.audio_bitrate);
    else
        ui->spinAudioBitrate->setValue(DEFAULT_AUDIO_BITRATE);

    if (param.audio_sample_rate)
        ui->cbAudioSampleRate->setEditText(QString("%1").arg(param.audio_sample_rate));
    else
        ui->cbAudioSampleRate->setCurrentIndex(0);

    if (param.audio_channels)
        ui->spinChannels->setValue(param.audio_channels);
    else
        ui->spinChannels->setValue(DEFAULT_AUDIO_CHANNELS);

    if (param.audio_volume)
        ui->spinVolume->setValue(TO_PERCENT(param.audio_volume));
    else
        ui->spinVolume->setValue(100);

    // Video Options
    ui->chkDisableVideo->setChecked(param.disable_video);

    if (param.video_bitrate)
        ui->spinVideoBitrate->setValue(param.video_bitrate);
    else
        ui->spinVideoBitrate->setValue(DEFAULT_VIDEO_BITRATE);

    ui->spinWidth->setValue(param.video_width);
    ui->spinHeight->setValue(param.video_height);

    ui->chkVideoSameQuality->setChecked(param.video_same_quality);
    ui->chkDeinterlace->setChecked(param.video_deinterlace);

    ui->spinCropTop->setValue(param.video_crop_top);
    ui->spinCropBottom->setValue(param.video_crop_bottom);
    ui->spinCropLeft->setValue(param.video_crop_left);
    ui->spinCropRight->setValue(param.video_crop_right);

    // Subtitle Options
    //ui->chkDisableSubtitle->setChecked(param.disable_subtitle);
}

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

}
