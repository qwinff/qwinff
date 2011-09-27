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

    /*! Fill in the files and execute the wizard.
     *  The wizard will skip the file-selecting page.
     *  @param files files to convert
     */
    int exec(QList<QUrl>& files);

protected:
    bool validateCurrentPage();

private slots:
    void add_files();
    void remove_files();
    void edit_preset();
    void browse_output_path();
    void load_presets(int);
    void preset_selected(int);
    void all_finished();

private:
    Ui::AddTaskWizard *ui;
    QString m_prev_path; //!< the most recently opened path of the file dialog.
    Presets *m_presets;
    QList<ConversionParameters> m_params;
    QScopedPointer<ConversionParameters> m_current_param;
    QVector<QVariant> m_ext_preset; //!< the mapping between extension and the last selected preset.
    bool load_extensions(const char *);
    void load_settings();
    void save_settings();
};

#endif // ADDTASKWIZARD_H
