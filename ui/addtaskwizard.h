#ifndef ADDTASKWIZARD_H
#define ADDTASKWIZARD_H

#include <QWizard>
#include <QScopedPointer>

namespace Ui {
    class AddTaskWizard;
}

class Presets;
class ConversionParameters;
class ConversionParameterDialog;

class AddTaskWizard : public QWizard
{
    Q_OBJECT

public:
    explicit AddTaskWizard(QWidget *parent = 0);
    ~AddTaskWizard();

private slots:
    void add_files_clicked();
    void remove_files_clicked();
    void edit_preset_clicked();
    void set_output_path_clicked();
    void load_presets(int);
    void preset_selected(int);

private:
    Ui::AddTaskWizard *ui;
    QString m_prev_path;
    Presets *m_presets;
    QScopedPointer<ConversionParameters> m_current_param;
    void load_extensions(const char *);
};

#endif // ADDTASKWIZARD_H
