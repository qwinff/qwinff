#include <QVBoxLayout>
#include "compositerangewidget.h"

CompositeRangeWidget::CompositeRangeWidget(QWidget *parent) :
    QWidget(parent),
    m_selector(new RangeSelector(this)),
    m_rangeEdit(new TimeRangeEdit(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_selector);
    layout->addWidget(m_rangeEdit);
    setLayout(layout);
    m_selector->setVisible(false);
    connect(m_selector, SIGNAL(valueChanged()), SLOT(sync_sel_to_edit()));
    connect(m_rangeEdit, SIGNAL(valueChanged()), SLOT(sync_edit_to_sel()));
}

void CompositeRangeWidget::setMaxTime(int secs)
{
    // m_selector should be modified before m_rangeEdit, because m_selector
    // is inaccurate (unable to determine whether the right edge is the end of video)
    // and should be updated by m_rangeEdit.
    m_selector->setMaxValue(secs); // modify m_selector -> sync m_selector to m_rangeEdit
    m_rangeEdit->setMaxTime(secs); // modify m_rangeEdit -> sync m_rangeEdit to m_selector
    m_selector->setVisible(true);
}

RangeSelector* CompositeRangeWidget::selectorWidget()
{
    return m_selector;
}

TimeRangeEdit* CompositeRangeWidget::rangeEditWidget()
{
    return m_rangeEdit;
}

// sync m_selector (visual) value to m_rangeEdit (text)
void CompositeRangeWidget::sync_sel_to_edit()
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
void CompositeRangeWidget::sync_edit_to_sel()
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
