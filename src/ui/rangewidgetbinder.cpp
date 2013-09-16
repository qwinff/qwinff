#include "rangewidgetbinder.h"

RangeWidgetBinder::RangeWidgetBinder(RangeSelector *sel,
                                     TimeRangeEdit *edit,
                                     QObject *parent) :
    QObject(parent),
    m_selector(sel),
    m_rangeEdit(edit)
{
    connect(m_selector, SIGNAL(valueChanged()), SLOT(sync_sel_to_edit()));
    connect(m_rangeEdit, SIGNAL(valueChanged()), SLOT(sync_edit_to_sel()));
}

// sync m_selector (visual) value to m_rangeEdit (text)
void RangeWidgetBinder::sync_sel_to_edit()
{
    int begin_time = m_selector->beginValue();
    int end_time = m_selector->endValue();
    bool from_begin = (begin_time == 0);
    bool to_end = (end_time == m_selector->maxValue());

    m_rangeEdit->setBeginTime(begin_time);
    m_rangeEdit->setEndTime(end_time);
    m_rangeEdit->setFromBegin(from_begin);
    m_rangeEdit->setToEnd(to_end);
}

// sync m_rangeEdit (text) value to m_selector (visual)
void RangeWidgetBinder::sync_edit_to_sel()
{
    int begin_time = m_rangeEdit->beginTime();
    int end_time = m_rangeEdit->endTime();

    if (m_rangeEdit->fromBegin())
        begin_time = 0;
    if (m_rangeEdit->toEnd())
        end_time = m_rangeEdit->maxTime();

    m_selector->setBeginValue(begin_time);
    m_selector->setEndValue(end_time);
}
