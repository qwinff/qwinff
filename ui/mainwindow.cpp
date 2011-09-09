#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "convertlist.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_list = new ConvertList();
    this->centralWidget()->layout()->addWidget(m_list);
    m_list->adjustSize();

    connect(m_list, SIGNAL(task_finished(int)),
            this, SLOT(task_finished(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start_conversion()
{

}

void MainWindow::stop_conversion()
{

}

void MainWindow::task_finished(int /*exitcode*/)
{
//    if (exitcode == 0) { // succeed
//        QMessageBox::information(this, this->windowTitle()
//                                 , tr("Conversion finished successfully.")
//                                 , QMessageBox::Ok);
//    } else { // failed
//        QMessageBox::critical(this, this->windowTitle()
//                              , tr("Conversion failed.")
//                              , QMessageBox::Ok);
//    }
}

void MainWindow::all_tasks_finished()
{

}

void MainWindow::on_pushButton_clicked()
{
    ConversionParameters param;
    QString src = QFileDialog::getOpenFileName(this, tr("Select a file"),
                                               QDir::homePath());
    if (src.isEmpty()) return;

    QString dest = QFileDialog::getSaveFileName(this, tr("Save file"),
                                                QDir::homePath());

    if (dest.isEmpty()) return;

    param.source = src;
    param.destination = dest;
    m_list->addTask(param);
}

void MainWindow::on_pushButton_2_clicked()
{
    m_list->start();
}
