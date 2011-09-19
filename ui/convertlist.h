#ifndef CONVERTLIST_H
#define CONVERTLIST_H

#include <QTreeWidget>
#include <QVector>
#include <QSharedPointer>
#include "converter/conversionparameters.h"

class MediaConverter;
class MediaProbe;

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

    /*! Append a task to the list
     * @param param the conversion parameter including the source and destination filename.
     * @return If the function succeeds, it returns true.
     *  Otherwise, it returns false.
     */
    bool addTask(const ConversionParameters &param);

    /*! Append a list of tasks to the list
     * @param paramList the list of conversion parameters
     * @return the number of successfully added tasks.
     */
    int addTasks(const QList<ConversionParameters>& paramList);

    void removeTask(int index);

    bool isBusy() const;
    bool isEmpty() const;
    int count() const;

    /*! Returns the pointer to the conversion parameter of the currently selected item.
     *  @return If the function fails, it returns NULL.
     */
    const ConversionParameters* getCurrentIndexParameter() const;

signals:
    void start_conversion(int index, ConversionParameters param);
    void task_finished(int);
    void all_tasks_finished();

public slots:
    void start();
    void stop();
    void removeSelectedItems();

private slots:
    void task_finished_slot(int);
    void progress_refreshed(int);

protected:
    void keyPressEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Q_DISABLE_COPY(ConvertList)

    typedef QSharedPointer<Task> TaskPtr;
    QVector<TaskPtr> m_tasks;
    int prev_index;
    void init_treewidget(QTreeWidget*);
    MediaConverter *m_converter;
    MediaProbe *m_probe;
    Task *m_current_task;
    bool is_busy;
    unsigned int m_progress_column_index;
};

#endif // CONVERTLIST_H
