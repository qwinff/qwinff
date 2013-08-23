#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateDialog;
}

class UpdateChecker;

class UpdateDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();

public slots:
    int exec(UpdateChecker& uc);

private slots:
    int exec();
    void checkUpdate();
    void updateFound();
    void slotReceivedUpdateResult(int result);
    void slotToggleReleaseNotes(bool checked);
    void slotToggleCheckUpdateOnStartup(bool checked);
    void resizeToFit();
    
private:
    UpdateChecker *m_updateChecker;
    Ui::UpdateDialog *ui;
    QString get_status();
    QString link(const QString &s);
};

#endif // UPDATEDIALOG_H
