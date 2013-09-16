#ifndef RANGEWIDGETBINDER_H
#define RANGEWIDGETBINDER_H

#include <QObject>
#include "rangeselector.h"
#include "timerangeedit.h"

/**
 * @brief Automatically sync between a RangeSelector and a TimeRangeEdit.
 */
class RangeWidgetBinder : public QObject
{
    Q_OBJECT
public:
    explicit RangeWidgetBinder(RangeSelector *sel,
                               TimeRangeEdit *edit,
                               QObject *parent = 0);

private slots:
    void sync_sel_to_edit();
    void sync_edit_to_sel();

private:
    RangeSelector *m_selector;
    TimeRangeEdit *m_rangeEdit;

};

#endif // RANGEWIDGETBINDER_H
