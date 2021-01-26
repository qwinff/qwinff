#ifndef ADDURLDIALOG_H
#define ADDURLDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QString>

namespace Ui {
class AddUrlDialog;
}

class AddUrlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUrlDialog(QWidget *parent = nullptr);
    ~AddUrlDialog();

    QString getUrl(bool *ok);

private:
    Ui::AddUrlDialog *ui;
};

#endif // ADDURLDIALOG_H
