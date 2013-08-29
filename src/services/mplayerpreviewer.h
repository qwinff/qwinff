#ifndef MPLAYERPREVIEWER_H
#define MPLAYERPREVIEWER_H

#include <QProcess>
#include "abstractpreviewer.h"

class MPlayerPreviewer : public AbstractPreviewer
{
    Q_OBJECT
public:
    explicit MPlayerPreviewer(QObject *parent = 0);

    bool available() const;

    /** Play the media file with ffplay.
     *  If a media file is being played, it will be stopped before
     *  playing the new one.
     */
    void play(const QString& filename);
    void play(const QString &filename, int t_begin, int t_end);

    void playFrom(const QString &filename, int t_begin);
    void playUntil(const QString &filename, int t_end);

    void stop();

private:
    QProcess *m_proc;

};

#endif // MPLAYERPREVIEWER_H
