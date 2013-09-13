#include "interactivecuttingdialog.h"
#include "ui_interactivecuttingdialog.h"
#include "mediaplayerwidget.h"
#include "rangeselector.h"
#include "timerangeedit.h"
#include "compositerangewidget.h"

InteractiveCuttingDialog::InteractiveCuttingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InteractiveCuttingDialog),
    player(new MediaPlayerWidget(this)),
    rangeWidget(new CompositeRangeWidget(this))
{
    ui->setupUi(this);
    ui->layoutPlayer->addWidget(player);
    ui->layoutRange->addWidget(rangeWidget);

    connect(player, SIGNAL(stateChanged()), SLOT(playerStateChanged()));
    connect(ui->btnAsBegin, SIGNAL(clicked()), SLOT(set_as_begin()));
    connect(ui->btnAsEnd, SIGNAL(clicked()), SLOT(set_as_end()));
    connect(ui->btnPlaySelection, SIGNAL(clicked()), SLOT(play_selection()));

    setFromBegin(true);
    setToEnd(true);
}

InteractiveCuttingDialog::~InteractiveCuttingDialog()
{
    delete ui;
}

bool InteractiveCuttingDialog::fromBegin() const
{
    return rangeWidget->rangeEditWidget()->fromBegin();
}

bool InteractiveCuttingDialog::toEnd() const
{
    return rangeWidget->rangeEditWidget()->toEnd();
}

int InteractiveCuttingDialog::beginTime() const
{
    return rangeWidget->rangeEditWidget()->beginTime();
}

int InteractiveCuttingDialog::endTime() const
{
    return rangeWidget->rangeEditWidget()->endTime();
}

void InteractiveCuttingDialog::setFromBegin(bool from_begin)
{
    rangeWidget->rangeEditWidget()->setFromBegin(from_begin);
}

void InteractiveCuttingDialog::setToEnd(bool to_end)
{
    rangeWidget->rangeEditWidget()->setToEnd(to_end);
}

void InteractiveCuttingDialog::setBeginTime(int sec)
{
    rangeWidget->rangeEditWidget()->setBeginTime(sec);
}

void InteractiveCuttingDialog::setEndTime(int sec)
{
    rangeWidget->rangeEditWidget()->setEndTime(sec);
}

int InteractiveCuttingDialog::exec(const QString &filename)
{
    player->load(filename);
    return exec();
}

int InteractiveCuttingDialog::exec(const QString &filename, TimeRangeEdit *range)
{
    setBeginTime(range->beginTime());
    setEndTime(range->endTime());
    setFromBegin(range->fromBegin());
    setToEnd(range->toEnd());
    int status = exec(filename);
    if (status == QDialog::Accepted) {
        range->setBeginTime(beginTime());
        range->setEndTime(endTime());
        range->setFromBegin(fromBegin());
        range->setToEnd(toEnd());
    }
    return status;
}

int InteractiveCuttingDialog::exec()
{
    return QDialog::exec();
}

void InteractiveCuttingDialog::playerStateChanged()
{
    TimeRangeEdit *rangeEdit = rangeWidget->rangeEditWidget();
    int duration = player->duration();
    if (duration > 0 && duration != rangeEdit->maxTime()) {
        // get media duration and set limits
        rangeWidget->selectorWidget()->setMaxValue(duration);
        rangeWidget->rangeEditWidget()->setMaxTime(duration);
    }
}

void InteractiveCuttingDialog::set_as_begin()
{
    rangeWidget->rangeEditWidget()->setBeginTime(player->position());
}

void InteractiveCuttingDialog::set_as_end()
{
    rangeWidget->rangeEditWidget()->setEndTime(player->position());
}

void InteractiveCuttingDialog::play_selection()
{
    TimeRangeEdit *rangeEdit = rangeWidget->rangeEditWidget();
    player->playRange(rangeEdit->beginTime(), rangeEdit->endTime());
}
