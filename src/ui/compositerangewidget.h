#ifndef COMPOSITERANGEWIDGET_H
#define COMPOSITERANGEWIDGET_H

#include <QWidget>

class RangeSelector;
class TimeRangeEdit;

/**
 * @brief Combine RangeSelector (visual) and TimeRangeEdit (text).
 *
 * RangeSelector is only shown if the maximum time is explicitly set.
 */
class CompositeRangeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CompositeRangeWidget(QWidget *parent = 0);

    RangeSelector *selectorWidget();
    TimeRangeEdit *rangeEditWidget();

private slots:
    void sync_sel_to_edit();
    void sync_edit_to_sel();

private:
    RangeSelector *m_selector;
    TimeRangeEdit *m_rangeEdit;
};

#endif // COMPOSITERANGEWIDGET_H
