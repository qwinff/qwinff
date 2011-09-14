#ifndef ADDTASKWIZARD_H
#define ADDTASKWIZARD_H

#include <QWizard>
#include <QUrl>
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

    /*! Returns conversion parameters for all files
     *  including input/output filenames.
     */
    const QList<ConversionParameters>& getConversionParameters() const;

    /*! Popup open file dialog on the file selection page.
     */
    int exec_openfile();

    /*! Popup without asking the user to select files.
     *  @param files files to convert
     */
    int exec(QList<QUrl>& files);

protected:
    bool validateCurrentPage();

private slots:
    void add_files_clicked();
    void remove_files_clicked();
    void edit_preset_clicked();
    void set_output_path_clicked();
    void load_presets(int);
    void preset_selected(int);
    void all_finished();

private:
    Ui::AddTaskWizard *ui;
    QString m_prev_path;
    Presets *m_presets;
    QList<ConversionParameters> m_params;
    QScopedPointer<ConversionParameters> m_current_param;
    void load_extensions(const char *);

};

#endif // ADDTASKWIZARD_H
