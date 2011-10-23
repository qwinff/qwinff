/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    explicit AddTaskWizard(Presets *presets, QWidget *parent = 0);

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
    void slotAddFilesToList();
    void slotRemoveFilesFromList();
    void slotEditPresetButton();
    void slotBrowseOutputPathButton();
    void slotExtensionSelected(int);
    void slotPresetSelected(int);
    void slotFinished();

private:
    Ui::AddTaskWizard *ui;
    QString m_prev_path; //!< the most recently opened path of the file dialog.
    Presets *m_presets;
    QList<ConversionParameters> m_params;
    QScopedPointer<ConversionParameters> m_current_param;
    QVector<QVariant> m_ext_preset; //!< the mapping between extension and the last selected preset.
    int m_cbpreset_index; //!< saves the index of the preset combobox
    bool load_extensions();
    void load_settings();
    void save_settings();
};

#endif // ADDTASKWIZARD_H
