#ifndef MEDIAPROBE_H
#define MEDIAPROBE_H

#include <QObject>
#include <QScopedPointer>

class MediaProbe : public QObject
{
    Q_OBJECT
public:
    explicit MediaProbe(QObject *parent = 0);
    virtual ~MediaProbe();

    /*! Start the probe on a file.
     * @param filename the name of the file to probe
     * @return If the process is started successfully, the function returns true.
     *  Otherwise, it returns false.
     */
    bool start(const QString& filename);
    bool start(const char* filename);

    /*! Block until the process has finished or until msecs milliseconds has passed.
     * @param msecs timeout
     * @return If the function exits because the process has finished, the function returns true.
     *  Otherwise, it returns false.
     */
    bool wait(int msecs = 3000);

    /*! Force the probe to stop.
     * The function will block until the probe is stopped.
     */
    void stop();

    /*! Returns whether the previous probing process has an error.
     *  For example, if the file is not recognized by ffprobe, the
     *  function returns false. If ffprobe has recognized the file
     *  and successfully extracts information from it, the function
     *  returns true.
     */
    bool error() const;

    /*! Returns the **hour** part of the duration */
    int hours() const;

    /*! Returns the **minutes** part of the duration
     *  The value is within 0 and 59(inclusive)
     */
    int minutes() const;

    /*! Returns the **seconds** part of the duration
     *  The value is within 0.0 and 60.0(non-inclusive)
     */
    double seconds() const;

    double mediaDuration() const;
    double mediaStartTime() const;
    int mediaBitRate() const;

    bool hasAudio() const;
    int audioSampleRate() const;
    int audioBitRate() const;
    int audioChannels() const;
    const QString& audioCodec() const;

    bool hasVideo() const;
    int videoWidth() const;
    int videoHeight() const;
    int videoBitRate() const;
    double videoFrameRate() const;
    const QString& videoCodec() const;

    bool hasSubtitle() const;

signals:
    /*! This signal is fired when the probe finishes.
     */
    void process_finished();

public slots:

private slots:
    void m_proc_finished(int);

private:
    Q_DISABLE_COPY(MediaProbe)
    struct Private;
    QScopedPointer<Private> p;
};

#endif // MEDIAPROBE_H
