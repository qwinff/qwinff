/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONVERTLIST_H
#define CONVERTLIST_H

#include <QWidget>
#include <QVector>
#include <QTime>
#include <QSharedPointer>
#include <QHash>
#include <QMessageBox>
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
        QString errmsg;
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

    /*! Determine whether the selected task has failed.
     *  @note If multiple tasks are selected, this function always returns false.
     */
    bool selectedTaskFailed() const;

signals:
    void start_conversion(int index, ConversionParameters param);
    void task_finished(int);
    void all_tasks_finished();
    void customContextMenuRequested(const QPoint &pos);
    void itemSelectionChanged();
    void started();
    void stopped();

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

    /*! Popup an input box to change the output filename
     *  of the **first** selected file.
     *  @warning If multiple files are selected, only the first
     *   file will be changed.
     */
    void changeSelectedOutputFile();

    /*! Popup a directory selection dialog to change the output directory.
     */
    void changeSelectedOutputDirectory();

    /*! Mark selected items as queued so that they will be converted again.
     *  If the converter is idle, start converting those items.
     *  Otherwise, the items are simply marked as queued.
     */
    void retrySelectedItems();

    void retryAll();

    /*! Popup a message box to show the error message from ffmpeg.
     *  This function only shows the error message for the first selected task.
     */
    void showErrorMessage();

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

    /** this variable should only be accessed by the output_filename_set* functions */
    QHash<QString, int> m_outputFileNames;
    void output_filenames_push(const QString& filename);
    void output_filenames_pop(const QString& filename);
    QHash<QString, int>& output_filenames();

    QTime m_startTime;
    void init_treewidget(QTreeWidget*);
    void init_treewidget_fill_column_titles(QStringList&);
    void init_treewidget_columns_visibility(QTreeWidget*);
    void fill_list_fields(ConversionParameters&, MediaProbe&, QStringList&);
    void reset_item(int index);
    void remove_items(const QList<QTreeWidgetItem*>&);
    ProgressBar* progressBar(Task*);
    ProgressBar* progressBar(const Task&);
    QString to_human_readable_size_1024(qint64 nBytes);
    bool change_output_file(int index, const QString& new_file
            , QMessageBox::StandardButtons &overwrite, bool show_all_buttons);

};

#endif // CONVERTLIST_H
