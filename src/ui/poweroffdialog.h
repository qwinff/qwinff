#ifndef POWEROFFDIALOG_H
#define POWEROFFDIALOG_H

#include <QDialog>

namespace Ui {
    class PoweroffDialog;
}

class QTimer;

class PoweroffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PoweroffDialog(QWidget *parent = 0);
    ~PoweroffDialog();

public slots:
    int exec(int action);

private slots:
    int exec();
    void show();
    void btnExecute_click();
    void btnCancel_click();
    void dialog_accepted();
    void dialog_rejected();
    void timer_event();
    void refresh_message();

private:
    Ui::PoweroffDialog *ui;
    QTimer *m_timer;
    int m_action;
    bool m_success;
    int m_time;
};

#endif // POWEROFFDIALOG_H
