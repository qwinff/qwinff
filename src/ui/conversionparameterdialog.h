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

#ifndef CONVERSIONPARAMETERDIALOG_H
#define CONVERSIONPARAMETERDIALOG_H

#include <QDialog>
#include "converter/conversionparameters.h"

namespace Ui {
    class ConversionParameterDialog;
}

class ConversionParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConversionParameterDialog(QWidget *parent = 0);
    ~ConversionParameterDialog();

    /*! This function blocks until the dialog is closed.
     *  @param param If the user presses OK, the modified parameter is written back.
     *  @return If the user presses OK, the function returns true.
     *  Otherwise, it returns false.
     */
    bool exec(ConversionParameters& param);

private slots:
    void update_endtime();

private:
    Ui::ConversionParameterDialog *ui;
    void read_fields(const ConversionParameters& param);
    void write_fields(ConversionParameters& param);
    bool m_enableAudioProcessing;
};

#endif // CONVERSIONPARAMETERDIALOG_H
