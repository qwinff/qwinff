#ifndef ABSTRACTPREVIEWER_H
#define ABSTRACTPREVIEWER_H

#include <QObject>

class AbstractPreviewer : public QObject
{
    Q_OBJECT
public:
    explicit AbstractPreviewer(QObject *parent = 0);
    virtual ~AbstractPreviewer();

    /** @brief Determine whether this previewer can play files.
     */
    virtual bool available() const = 0;

    /** @brief Play the media file named @a filename.
     *
     *  This function must be asynchronous, i.e. it must return
     *  immediately without waiting for the player to exit.
     */
    virtual void play(const QString& filename) = 0;

    /** @brief Play a portion of the file from @a t_begin to @a t_end (seconds).
     *
     *  @see play(const QString&)
     */
    virtual void play(const QString &filename, int t_begin, int t_end) = 0;

    virtual void playFrom(const QString &filename, int t_begin) = 0;
    virtual void playUntil(const QString &filename, int t_end) = 0;

    virtual void stop() = 0;

private:
    Q_DISABLE_COPY(AbstractPreviewer)
};

#endif // ABSTRACTPREVIEWER_H
