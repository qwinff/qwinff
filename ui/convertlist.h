#ifndef CONVERTLIST_H
#define CONVERTLIST_H

#include <QTreeWidget>
#include <QVector>
#include <QSharedPointer>
#include "converter/conversionparameters.h"

class MediaConverter;

class ConvertList : public QTreeWidget
{
    Q_OBJECT
public:

    struct Task
    {
        enum TaskStatus { QUEUED, RUNNING, FINISHED, FAILED };
        int id;
        TaskStatus status;
        ConversionParameters param;
        QTreeWidgetItem *listitem;
    };

    explicit ConvertList(QWidget *parent = 0);
    void addTask(const ConversionParameters &param);
    void removeTask(int index);

signals:
    void start_conversion(int index, ConversionParameters param);
    void task_finished(int);
    void all_tasks_finished();

public slots:
    void start();
    void stop();

private slots:
    void task_finished_slot(int);
    void progress_refreshed(int);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Q_DISABLE_COPY(ConvertList)

    typedef QSharedPointer<Task> TaskPtr;
    QVector<TaskPtr> m_tasks;
    int prev_index;
    void init_treewidget(QTreeWidget*);
    MediaConverter *m_converter;
    Task *m_current_task;
    bool is_busy;
    unsigned int m_progress_column_index;
};

#endif // CONVERTLIST_H
