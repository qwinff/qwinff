#ifndef COMPOSITERANGEWIDGET_H
#define COMPOSITERANGEWIDGET_H

#include <QWidget>
#include "rangeselector.h"
#include "timerangeedit.h"

/**
 * @brief Combine RangeSelector (visual) and TimeRangeEdit (text).
 *
 * RangeSelector is hidden by default.
 */
class CompositeRangeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CompositeRangeWidget(QWidget *parent = 0);

    /**
     * @brief Set the maximum time (duration of media file) and show RangeSelector widget.
     * @param secs
     */
    void setMaxTime(int secs);

    /**
     * @brief Get the selector widget. The widget is hidden by default.
     * Set the maximum time with setMaxTime() to make it visible.
     */
    RangeSelector *selectorWidget();

    /**
     * @brief Get the TimeRangeEdit widget.
     */
    TimeRangeEdit *rangeEditWidget();

private slots:
    void sync_sel_to_edit();
    void sync_edit_to_sel();

private:
    RangeSelector *m_selector;
    TimeRangeEdit *m_rangeEdit;
};

#endif // COMPOSITERANGEWIDGET_H
