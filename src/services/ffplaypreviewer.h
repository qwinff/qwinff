#ifndef FFPLAYPREVIEWER_H
#define FFPLAYPREVIEWER_H

#include "abstractpreviewer.h"

class QProcess;

class FFplayPreviewer : public AbstractPreviewer
{
    Q_OBJECT
public:
    explicit FFplayPreviewer(QObject *parent = 0);
    virtual ~FFplayPreviewer();

    /** Play the media file with ffplay.
     *  If a media file is being played, it will be stopped before
     *  playing the new one.
     */
    void play(const QString& filename);
    void play(const QString &filename, int t_begin, int t_end);

    void playFrom(const QString &filename, int t_begin);
    void playUntil(const QString &filename, int t_end);

    void stop();

    /** Set the window size of ffplay.
     *  This option takes effect after invoking start() again.
     */
    void setWindowSize(int w, int h);

    /** Set the ffplay window title.
     *  This option takes effect after invoking start() again.
     *  If str is empty, default title is displayed (filename).
     */
    void setWindowTitle(QString str);

    static bool FFplayAvailable();

signals:

public slots:

private:
    QProcess *m_proc;
    int m_w, m_h;
    QString m_title;

    void ffplay_start(const QString&, int, int);
};

#endif // FFPLAYPREVIEWER_H
