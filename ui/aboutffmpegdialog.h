#ifndef ABOUTFFMPEGDIALOG_H
#define ABOUTFFMPEGDIALOG_H

#include <QDialog>

namespace Ui {
    class AboutFFmpegDialog;
}

class AboutFFmpegDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutFFmpegDialog(QWidget *parent = 0);
    ~AboutFFmpegDialog();

private:
    Ui::AboutFFmpegDialog *ui;
};

#endif // ABOUTFFMPEGDIALOG_H
