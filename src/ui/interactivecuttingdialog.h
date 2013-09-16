#ifndef INTERACTIVECUTTINGDIALOG_H
#define INTERACTIVECUTTINGDIALOG_H

#include <QDialog>

namespace Ui {
class InteractiveCuttingDialog;
}

class MediaPlayerWidget;
class TimeRangeEdit;
class RangeSelector;
class ConversionParameters;

class InteractiveCuttingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit InteractiveCuttingDialog(QWidget *parent = 0);
    ~InteractiveCuttingDialog();

    static bool available();

    bool fromBegin() const;
    bool toEnd() const;

    int beginTime() const;
    int endTime() const;

public slots:
    void setFromBegin(bool);
    void setToEnd(bool);
    void setBeginTime(int);
    void setEndTime(int);

    /**
     * @brief Start the dialog to cut a file
     * @param filename the file to process
     * @return QDialog::Accepted if the user selects a range.
     */
    int exec(const QString& filename);

    /**
     * @brief Start the dialog to cut a file and modify @a range if the user presses OK.
     * @param filename the file to process
     * @param range [in,out] the range widget to be modified
     * @return
     */
    int exec(const QString& filename, TimeRangeEdit *range);

    /**
     * @brief Start the dialog to cut @c param->source and modify time_begin and time_end
     *  if the user presses OK.
     * @param param [in,out] pointer to the ConversionParameters object to be modified
     * @return
     */
    int exec(ConversionParameters *param);

private slots:
    int exec();
    void playerStateChanged();
    void set_as_begin();
    void set_as_end();
    void seek_to_selection_begin();
    void seek_to_selection_end();
    void play_selection();
    
private:
    Ui::InteractiveCuttingDialog *ui;
    MediaPlayerWidget *player;
    RangeSelector *m_rangeSel;
    TimeRangeEdit *m_rangeEdit;
};

#endif // INTERACTIVECUTTINGDIALOG_H
