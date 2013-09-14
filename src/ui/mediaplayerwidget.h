#ifndef MEDIAPLAYERWIDGET_H
#define MEDIAPLAYERWIDGET_H

#include <QWidget>

namespace Ui {
class MediaPlayerWidget;
}

class MyQMPwidget;

class MediaPlayerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MediaPlayerWidget(QWidget *parent = 0);
    ~MediaPlayerWidget();

    int duration() const;
    int position() const;

signals:
    void stateChanged();

public slots:

    /**
     * @brief Load media for playing from @a url.
     *
     * @param url path of the file
     */
    void load(const QString& url);

    /**
     * @brief Reload previously loaded file.
     */
    void reload();

    /**
     * @brief Start playing the loaded file.
     */
    void play();

    /**
     * @brief Play the media file from @a begin_sec to @a end_sec.
     * The playback will pause as soon as @c position reaches @a end_sec.
     * The range of the slider is not modified, as opposed to @c setRangeLimit().
     *
     * @param begin_sec the beginning point in seconds. -1 means play from beginning
     * @param end_sec the end point in seconds. -1 means play until end
     */
    void playRange(int begin_sec, int end_sec);

    /**
     * @brief Seek to @a sec and start playing.
     *
     * @param sec position in seconds
     */
    void seek(int sec);

    /**
     * @brief Seek to @a sec and pause.
     * @param sec
     */
    void seek_and_pause(int sec);

    /**
     * @brief Pause the media playback.
     */
    void pause();

    /**
     * @brief Toggle between Pause and Play.
     */
    void togglePlayPause();

private slots:
    void refreshTimeDisplay();
    void refreshButtonState();
    void playerStateChanged();
    void seekSliderChanged();
    void seekBack();
    void seekForward();
    void resetPosition();
    
private:
    Ui::MediaPlayerWidget *ui;
    MyQMPwidget *mplayer;
    QString m_file;

    // media playback will stop when position() reaches m_playUntil
    int m_playUntil;

    Q_DISABLE_COPY(MediaPlayerWidget)
};

#endif // MEDIAPLAYERWIDGET_H
