#include "poweroffdialog.h"
#include "ui_poweroffdialog.h"
#include "services/powermanagement.h"
#include <QMessageBox>
#include <QTimer>

#define SEC_TO_WAIT 30

PoweroffDialog::PoweroffDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PoweroffDialog),
    m_timer(new QTimer(this))
{
    ui->setupUi(this);
    connect(ui->btnExecute, SIGNAL(clicked())
            , this, SLOT(btnExecute_click()));
    connect(ui->btnCancel, SIGNAL(clicked())
            , this, SLOT(btnCancel_click()));
    connect(this, SIGNAL(accepted())
            , this, SLOT(dialog_accepted()));
    connect(this, SIGNAL(rejected())
            , this, SLOT(dialog_rejected()));
    connect(m_timer, SIGNAL(timeout())
            , this, SLOT(timer_event()));
    m_timer->setInterval(1000); // 1 second
    m_timer->setSingleShot(false); // always execute the timer
    m_timer->stop();
}

PoweroffDialog::~PoweroffDialog()
{
    delete ui;
}

int PoweroffDialog::exec(int action)
{
    switch (action) {
    case PowerManagement::SHUTDOWN:
        ui->btnExecute->setIcon(QIcon(":/actions/icons/system_shutdown"));
        //: Shutdown the computer
        ui->btnExecute->setText(tr("Shutdown immediately"));
        break;
    case PowerManagement::SUSPEND:
        ui->btnExecute->setIcon(QIcon(":/actions/icons/system_suspend"));
        //: Suspend the computer (sleep to ram, standby)
        ui->btnExecute->setText(tr("Suspend immediately"));
        break;
    }

    m_action = action;
    m_time = SEC_TO_WAIT;
    m_timer->start();
    refresh_message();

    return QDialog::exec();
}

int PoweroffDialog::exec()
{
    return QDialog::Rejected;
}

void PoweroffDialog::show()
{

}

void PoweroffDialog::btnExecute_click()
{
    accept();
}

void PoweroffDialog::btnCancel_click()
{
    reject();
}

void PoweroffDialog::dialog_accepted()
{
    m_timer->stop();
    m_success = PowerManagement::sendRequest(m_action);
}

void PoweroffDialog::dialog_rejected()
{
    m_timer->stop();
}

void PoweroffDialog::timer_event()
{
    if (--m_time <= 0) {
        m_timer->stop();
        accept();
    }
    refresh_message();
}

void PoweroffDialog::refresh_message()
{
    QString msg;

    switch (m_action) {
    case PowerManagement::SHUTDOWN:
        msg = tr("The computer will shutdown in <b>%1</b> seconds").arg(m_time);
        break;
    case PowerManagement::SUSPEND:
        msg = tr("The computer will suspend in <b>%1</b> seconds").arg(m_time);
        break;
    }

    ui->lblMessage->setText(msg);
}
