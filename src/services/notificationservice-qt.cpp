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
    send(title, message, "");
}

void NotificationService_Qt::send(QString title, QString message, QString image)
{
    QMessageBox *msgbox = new QMessageBox();
    msgbox->setAttribute(Qt::WA_DeleteOnClose); // delete itself on close
    msgbox->setWindowFlags(msgbox->windowFlags()
                           | Qt::WindowStaysOnTopHint); // always on top
    msgbox->setStandardButtons(QMessageBox::Ok);
    msgbox->setWindowTitle(title);
    msgbox->setText(message);
    msgbox->setIcon(QMessageBox::Information);
    msgbox->setModal(false); // non-modal message box
    msgbox->show();
}

bool NotificationService_Qt::serviceAvailable() const
{
    return true;
}
