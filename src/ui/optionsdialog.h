#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

    bool exec();

private:
    Ui::OptionsDialog *ui;

    void read_fields();
    void write_fields();
};

#endif // OPTIONSDIALOG_H
