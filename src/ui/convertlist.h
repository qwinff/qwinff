#ifndef CONVERTLIST_H
#define CONVERTLIST_H

#include <QWidget>
#include <QVector>
#include <QTime>
#include <QSharedPointer>
#include <QSet>
#include "converter/conversionparameters.h"

class MediaConverter;
class MediaProbe;
class QTreeWidget;
class QTreeWidgetItem;
class ProgressBar;
class Presets;

class ConvertList : public QWidget
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

    explicit ConvertList(Presets *presets, QWidget *parent = 0);
    ~ConvertList();

    /*! Append a task to the list
     * @param param the conversion parameter including the source and destination filename.
     * @return If the function succeeds, it returns true.
     *  Otherwise, it returns false.
     */
    bool addTask(ConversionParameters param);

    /*! Append a list of tasks to the list
     * @param paramList the list of conversion parameters
     * @return the number of successfully added tasks.
     */
    int addTasks(const QList<ConversionParameters>& paramList);

    void removeTask(int index);

    bool isBusy() const;
    bool isEmpty() const;
    int count() const;
    int selectedCount() const;

    /*! Get the elapsed time of the session (in milliseconds).
     *  If the converter is idle, the function returns 0.
     */
    int elapsedTime() const;

    /*! Returns the pointer to the conversion parameter of the currently selected item.
     *  @return If the function fails, it returns NULL.
     */
    const ConversionParameters* getCurrentIndexParameter() const;

signals:
    void start_conversion(int index, ConversionParameters param);
    void task_finished(int);
    void all_tasks_finished();
    void customContextMenuRequested(const QPoint &pos);

public slots:

    /*! Start the conversion progress.
     *  If another task is being processed, the function does nothing.
     */
    void start();

    /*! Stop the conversion progress
     */
    void stop();

    /*! Remove all selected tasks but quietly ignore tasks in progress.
     */
    void removeSelectedItems();

    /*! Remove all tasks marked as completed.
     */
    void removeCompletedItems();

    /*! Popup edit-parameter dialog.
     *  The parameter of the first selected task will be used as the default
     *  configuration. If the user presses OK in the dialog, all selected
     *  tasks will be set to the same parameter.
     */
    void editSelectedParameters();

    /*! Mark selected items as queued so that they will be converted again.
     *  If the converter is idle, start converting those items.
     *  Otherwise, the items are simply marked as queued.
     */
    void retrySelectedItems();

    void retryAll();

    /*! Remove all tasks but quietly ignore tasks that are in progress.
     */
    void clear();

private slots:
    void task_finished_slot(int);
    void progress_refreshed(int);
    void slotHeaderContextMenu(QPoint);
    void slotHeaderContextMenuTriggered(QAction*);
    void slotRestoreListHeaders();

protected:
    bool list_keyPressEvent(QKeyEvent *event);
    void list_dragEnterEvent(QDragEnterEvent *event);
    void list_dragMoveEvent(QDragMoveEvent *event);
    void list_dragLeaveEvent(QDragLeaveEvent *event);
    void list_dropEvent(QDropEvent *event);

private:
    Q_DISABLE_COPY(ConvertList)

    class ListEventFilter;
    friend class ListEventFilter;
    typedef QSharedPointer<Task> TaskPtr;

    QTreeWidget *m_list;
    ListEventFilter *m_listEventFilter;
    QVector<TaskPtr> m_tasks;
    int prev_index;
    MediaConverter *m_converter;
    MediaProbe *m_probe;
    Task *m_current_task;
    bool is_busy;
    bool run_next; ///< run next task regardless of the value of is_busy
    Presets *m_presets;
    QSet<QString> m_outputFileNames;
    QTime m_startTime;
    void init_treewidget(QTreeWidget*);
    void init_treewidget_fill_column_titles(QStringList &columnTitle);
    void fill_list_fields(ConversionParameters &param, MediaProbe &probe,
                           QStringList &columns);
    void reset_item(int index);
    void remove_items(const QList<QTreeWidgetItem*>& itemList);
    ProgressBar* progressBar(Task*);
    ProgressBar* progressBar(const Task&);
};

#endif // CONVERTLIST_H
