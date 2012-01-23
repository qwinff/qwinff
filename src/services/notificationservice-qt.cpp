#include "notificationservice-qt.h"
#include <QProcess>
#include <QMessageBox>

NotificationService_Qt::NotificationService_Qt()
{

}

NotificationService_Qt::~NotificationService_Qt()
{

}

void NotificationService_Qt::send(QString title, QString message)
{
    send(title, message, NotifyLevel::INFO);
}

void NotificationService_Qt::send(QString title, QString message, int level)
{
    QMessageBox *msgbox = new QMessageBox();
    msgbox->setAttribute(Qt::WA_DeleteOnClose); // delete itself on close
    msgbox->setWindowFlags(msgbox->windowFlags()
                           | Qt::WindowStaysOnTopHint); // always on top
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->setWindowTitle(title);
    msgbox->setText(message);
    msgbox->setModal(false); // non-modal message box

    switch (level) {
    case NotifyLevel::INFO:
        msgbox->setIcon(QMessageBox::Information);
        break;
    case NotifyLevel::WARNING:
        msgbox->setIcon(QMessageBox::Warning);
        break;
    case NotifyLevel::CRITICAL:
        msgbox->setIcon(QMessageBox::Critical);
        break;
    default:
        msgbox->setIcon(QMessageBox::NoIcon);
    }

    msgbox->show();
}

bool NotificationService_Qt::serviceAvailable() const
{
    return true;
}
