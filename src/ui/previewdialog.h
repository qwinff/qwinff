#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>

namespace Ui {
class PreviewDialog;
}

class MediaPlayerWidget;

class PreviewDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PreviewDialog(QWidget *parent = 0);
    ~PreviewDialog();

    static bool available();

public slots:
    int exec(const QString &filename,
             bool from_begin, int begin_sec,
             bool to_end, int end_sec);

private slots:
    int exec();
    void playSelectedRange();
    void refreshRange();
    
private:
    Ui::PreviewDialog *ui;
    MediaPlayerWidget *m_player;
    int m_beginTime;
    int m_endTime;
};

#endif // PREVIEWDIALOG_H
