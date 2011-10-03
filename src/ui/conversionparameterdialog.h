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

private:
    Ui::ConversionParameterDialog *ui;
    void read_fields(const ConversionParameters& param);
    void write_fields(ConversionParameters& param);
};

#endif // CONVERSIONPARAMETERDIALOG_H
