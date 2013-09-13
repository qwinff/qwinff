#include "mediaplayerwidget.h"
#include "previewdialog.h"
#include "ui_previewdialog.h"

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

PreviewDialog::PreviewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewDialog),
    m_player(new MediaPlayerWidget(this))
{
    ui->setupUi(this);
    ui->layoutPlayer->addWidget(m_player);
    connect(ui->btnPlayRange, SIGNAL(clicked()), SLOT(playSelectedRange()));
}

PreviewDialog::~PreviewDialog()
{
    delete ui;
}

int PreviewDialog::exec(const QString &filename,
                               bool from_begin, int begin_sec,
                               bool to_end, int end_sec)
{
    m_beginTime = from_begin ? -1 : begin_sec;
    m_endTime = to_end ? -1 : end_sec;
    m_player->load(filename);
    playSelectedRange();
    refreshRange();
    return exec();
}

int PreviewDialog::exec()
{
    return QDialog::exec();
}

void PreviewDialog::playSelectedRange()
{
    m_player->playRange(m_beginTime, m_endTime);
}

void PreviewDialog::refreshRange()
{
    //: noun, the beginning of the video
    QString begin_time = tr("Begin");
    //: noun, the end of the video
    QString end_time = tr("End");
    if (m_beginTime >= 0)
        begin_time = sec2hms(m_beginTime);
    if (m_endTime >= 0)
        end_time = sec2hms(m_endTime);
    //: play the video from time %1 to time %2. %1 and %2 are time in hh:mm:ss format.
    ui->btnPlayRange->setText(tr("Play %1~%2").arg(begin_time).arg(end_time));
}
