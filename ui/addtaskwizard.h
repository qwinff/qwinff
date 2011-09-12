#ifndef ADDTASKWIZARD_H
#define ADDTASKWIZARD_H

#include <QWizard>

namespace Ui {
    class AddTaskWizard;
}

class AddTaskWizard : public QWizard
{
    Q_OBJECT

public:
    explicit AddTaskWizard(QWidget *parent = 0);
    ~AddTaskWizard();

private:
    Ui::AddTaskWizard *ui;
};

#endif // ADDTASKWIZARD_H
