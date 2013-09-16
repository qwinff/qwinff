#include <cmath>
#include "mediaplayerwidget.h"
#include "ui_mediaplayerwidget.h"
#include "myqmpwidget.h"
#include "services/constants.h"

#define DEFAULT_VOLUME 20
#define MAX_VOLUME 100

#define SLIDER_STYLESHEET Constants::getString("MediaPlayerPositionSlider/style")

namespace {
QString sec2hms(int seconds)
{
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = (seconds % 60);
    QString result;
    result.sprintf("%02d:%02d:%02d", h, m, s);
    return result;
}
}

MediaPlayerWidget::MediaPlayerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaPlayerWidget),
    m_playUntil(-1)
{
    ui->setupUi(this);
    ui->slideVolume->setRange(0, MAX_VOLUME);
    mplayer = new MyQMPwidget(this);
    mplayer->start();
    mplayer->setAcceptDrops(false);
    mplayer->setSeekSlider(ui->slideSeek);
    mplayer->setVolumeSlider(ui->slideVolume);
    ui->layoutPlayer->addWidget(mplayer);
    ui->slideSeek->setStyleSheet(SLIDER_STYLESHEET);

    connect(mplayer, SIGNAL(stateChanged(int)), SLOT(playerStateChanged()));
    connect(ui->slideSeek, SIGNAL(valueChanged(int)), SLOT(seekSliderChanged()));
    connect(ui->btnPlayPause, SIGNAL(clicked()), SLOT(togglePlayPause()));
    connect(ui->btnBack, SIGNAL(clicked()), SLOT(seekBack()));
    connect(ui->btnForward, SIGNAL(clicked()), SLOT(seekForward()));
    connect(ui->btnReset, SIGNAL(clicked()), SLOT(resetPosition()));
}

MediaPlayerWidget::~MediaPlayerWidget()
{
    delete ui;
}

int MediaPlayerWidget::duration() const
{
    MyQMPwidget::MediaInfo info = mplayer->mediaInfo();
    if (info.ok)
        return ceil(info.length);
    else
        return 0;
}

int MediaPlayerWidget::position() const
{
    return floor(mplayer->tell());
}

// public slots

void MediaPlayerWidget::load(const QString &url)
{
    m_file = url;
    mplayer->load(url);
    ui->slideVolume->setValue(DEFAULT_VOLUME);
    mplayer->pause();
}

void MediaPlayerWidget::reload()
{
    load(m_file);
}

void MediaPlayerWidget::play()
{
    mplayer->play();
    ui->btnPlayPause->setFocus();
    refreshButtonState();
}

void MediaPlayerWidget::playRange(int begin_sec, int end_sec)
{
    if (mplayer->state() == MyQMPwidget::IdleState)
        reload();
    if (begin_sec >= 0)
        seek(begin_sec);
    else
        seek(0);
    if (end_sec >= 0)
        m_playUntil = end_sec;
    else
        m_playUntil = -1;
    play();
}

void MediaPlayerWidget::pause()
{
    mplayer->pause();
    ui->btnPlayPause->setFocus();
    refreshButtonState();
}

void MediaPlayerWidget::seek(int sec)
{
    mplayer->seek(sec);
}

void MediaPlayerWidget::seek_and_pause(int sec)
{
    // seek() is asynchrous. If call mplayer->seek(); mplayer->pause(); in a row,
    // the latter command will be ignored because seek() is not done yet.
    mplayer->seek(sec);
    m_playUntil = sec; // seek() is asynchrouse
}

void MediaPlayerWidget::togglePlayPause()
{
    switch (mplayer->state()) {
    case MyQMPwidget::IdleState:
        reload();
        if (ui->slideSeek->value() < ui->slideSeek->maximum()) { // user seeks
            mplayer->seek(ui->slideSeek->value());
        } else { // otherwise rewind to begin
            ui->slideSeek->setValue(0);
        }
        pause();
        break;
    case MyQMPwidget::PlayingState:
        pause();
        break;
    case MyQMPwidget::PausedState:
        play();
        break;
    default:
        break;
    }
}

// private slots

void MediaPlayerWidget::refreshTimeDisplay()
{
    MyQMPwidget::MediaInfo info = mplayer->mediaInfo();
    int duration, position, remaining;
    if (info.ok) {
        duration = info.length;
        position = mplayer->tell();
        if (position < 0) position = 0;
        remaining = duration - position;
    } else {
        duration = position = remaining = 0;
    }
    ui->lblPosition->setText(QString("%1 / %2")
                             .arg(sec2hms(position))
                             .arg(sec2hms(duration)));
    ui->lblRemaining->setText(QString("-%1").arg(sec2hms(remaining)));
}

void MediaPlayerWidget::refreshButtonState()
{
    QString text = "";
    switch (mplayer->state()) {
    case MyQMPwidget::PlayingState:
        text = tr("Pause"); break;
    case MyQMPwidget::PausedState:
        text = tr("Play"); break;
    case MyQMPwidget::StoppedState:
    case MyQMPwidget::IdleState:
        text = tr("Play"); break;
    default:
        text = tr("Play");
    }
    ui->btnPlayPause->setText(text);
}

void MediaPlayerWidget::playerStateChanged()
{
    refreshTimeDisplay();
    refreshButtonState();
    emit stateChanged();
}

void MediaPlayerWidget::seekSliderChanged()
{
    refreshTimeDisplay();
    if (m_playUntil >= 0 && position() >= m_playUntil) {
        // reaches temporary pause position
        m_playUntil = -1;
        pause();
    }
}

void MediaPlayerWidget::seekBack()
{
    mplayer->seek(-3, MyQMPwidget::RelativeSeek);
}

void MediaPlayerWidget::seekForward()
{
    mplayer->seek(3, MyQMPwidget::RelativeSeek);
}

void MediaPlayerWidget::resetPosition()
{
    mplayer->seek(0);
}
