#include "convertlist.h"
#include "progressbar.h"
#include "converter/mediaconverter.h"
#include "converter/mediaprobe.h"
#include "ui/conversionparameterdialog.h"
#include "addtaskwizard.h"
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMessageBox>
#include <QUrl>
#include <QDebug>
#include <QFileInfo>
#include <QProgressDialog>
#include <QSettings>
#include <cassert>

#define MIN_DURATION 100 // Minimum duration(milliseconds) to show progress dialog.


class ConvertList::ListEventFilter : public QObject
{
public:
    ListEventFilter(ConvertList *parent) : QObject(parent), m_parent(parent) { }

    // Propagate events from the list to its parent.
    bool eventFilter(QObject */*object*/, QEvent *event)
    {
        switch (event->type()) {
        case QEvent::KeyPress:
            return m_parent->list_keyPressEvent(static_cast<QKeyEvent*>(event));
        case QEvent::DragEnter:
            m_parent->list_dragEnterEvent(static_cast<QDragEnterEvent*>(event));
            return true;
        case QEvent::DragMove:
            m_parent->list_dragMoveEvent(static_cast<QDragMoveEvent*>(event));
            return true;
        case QEvent::DragLeave:
            m_parent->list_dragLeaveEvent(static_cast<QDragLeaveEvent*>(event));
        case QEvent::Drop:
            m_parent->list_dropEvent(static_cast<QDropEvent*>(event));
            return true;
        default:
            break;
        }
        return false;
    }

private:
    ConvertList *m_parent;
};

ConvertList::ConvertList(QWidget *parent) :
    QWidget(parent),
    m_list(new QTreeWidget(this)),
    m_listEventFilter(new ListEventFilter(this)),
    prev_index(0),
    m_converter(new MediaConverter(this)),
    m_probe(new MediaProbe(this)),
    m_current_task(0),
    is_busy(false)
{
    QLayout *layout = new QHBoxLayout(this);
    this->setLayout(layout);

    init_treewidget(m_list);
    layout->addWidget(m_list);

    connect(m_converter, SIGNAL(finished(int))
            , this, SLOT(task_finished_slot(int)));
    connect(m_converter, SIGNAL(progressRefreshed(int)),
            this, SLOT(progress_refreshed(int)));

    // enable drag/drop functions
    m_list->setAcceptDrops(true);

    // allow selecting multiple items
    m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Propagate events from the QTreeWidget to ConvertList.
    m_list->installEventFilter(m_listEventFilter);

    QSettings settings;
    m_list->header()->restoreState(settings.value("convertlist/header_state").toByteArray());
}

ConvertList::~ConvertList()
{
    QSettings settings;
    settings.setValue("convertlist/header_state", m_list->header()->saveState());
}

bool ConvertList::addTask(const ConversionParameters& param)
{
    // get source file information
    m_probe->start(param.source);
    if (!m_probe->wait() || m_probe->error()) {
        // failed to get media information immediately
        return false;
    }

    TaskPtr task(new Task());
    task->param = param;
    task->status = Task::QUEUED;
    task->id = ++prev_index;

    m_tasks.push_back(task);

    QStringList columns;
    columns << QFileInfo(param.source).fileName()       // source file
            << QFileInfo(param.destination).fileName()  // destination file
            << QString().sprintf("%02d:%02d:%02.0f"     // duration
                        , m_probe->hours()              //    hours
                        , m_probe->minutes()            //    minutes
                        , m_probe->seconds())           //    seconds
            << "";                                      // progress

    QTreeWidgetItem *item = new QTreeWidgetItem(m_list, columns);
    task->listitem = item;

    // Add a progress bar widget to the list item
    m_list->addTopLevelItem(item);
    m_list->setItemWidget(item, m_progress_column_index, new ProgressBar());
    m_list->itemWidget(item, m_progress_column_index)->adjustSize();

    item->setToolTip(/*source index*/ 0, param.source);
    item->setToolTip(/*destination index*/ 1, param.destination);

    qDebug() << QString("Added: \"%1\" -> \"%2\"").arg(param.source).arg(param.destination);

    return true;
}

int ConvertList::addTasks(const QList<ConversionParameters> &paramList)
{
    const int file_count = paramList.size();
    int success_count = 0;

    // Record the files that are not recognized by the converter.
    QStringList failed_files;

    // Create progress dialog.
    /* Translators: *//*: Cancel the operation of adding new tasks */
    QProgressDialog dlgProgress(QString(""),
                                tr("Cancel"),
                                0, file_count,  /* min/max */
                                this);
    dlgProgress.setWindowModality(Qt::WindowModal);
    dlgProgress.setMinimumDuration(MIN_DURATION);

    int progress_count = 0;
    QList<ConversionParameters>::const_iterator it = paramList.begin();
    for (; it!=paramList.end(); ++it) {

        // Indicate the current progress.
        /*: This text is the progress indicator of adding multiple tasks.
            %1 is the number of files that are already added.
            %2 is the total number of files. */
        dlgProgress.setLabelText(tr("Adding files (%1/%2)")
                                 .arg(progress_count).arg(file_count));

        // Update progress dialog.
        dlgProgress.setValue(progress_count++);

        // Check if the user has canceled the operation.
        if (dlgProgress.wasCanceled())
            break;

        if (addTask(*it)) { // This step takes the most of the time.
            success_count++;
        }
        else {
            failed_files.push_back(it->source); // Record failed files.
            qDebug() << QString("Failed to add file: %1").arg(it->source);
        }

    }

    dlgProgress.setValue(file_count); // Terminate the progress indicator.

    if (!failed_files.isEmpty()) { // Some files are incorrect.
        QMessageBox msgBox;
        msgBox.setText(tr("Some files are not recognized by the converter."));
        msgBox.setDetailedText(failed_files.join("\n"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }

    return success_count;
}

void ConvertList::removeTask(int index)
{
    qDebug() << "ConvertList::removeTask(), index=" << index;
    if (m_tasks[index]->status != Task::RUNNING) { // not a running task
        m_tasks.remove(index);
        delete m_list->takeTopLevelItem(index);
    } else { // The task is being executed.

        if (false)  // Silently ignore the event.
            QMessageBox::warning(this, tr("Remove Task")
                              , tr("Cannot remove a task while it is in progress.")
                              , QMessageBox::Ok);
    }
}

bool ConvertList::isBusy() const
{
    return is_busy;
}

bool ConvertList::isEmpty() const
{
    return m_list->topLevelItemCount() == 0;
}

int ConvertList::count() const
{
    return m_list->topLevelItemCount();
}

int ConvertList::selectedCount() const
{
    return m_list->selectedItems().size();
}

const ConversionParameters* ConvertList::getCurrentIndexParameter() const
{
    const int index = m_list->currentIndex().row();
    if (index >= 0 && index < m_tasks.size()) {
        return &m_tasks[index]->param;
    } else {
        return 0;
    }
}

// Public Slots

void ConvertList::start()
{
    if (is_busy)
        return;

    const int task_count = m_tasks.size();

    if (task_count == 0) { // the task list is empty
        this->stop();
        return;
    }

    for (int i=0; i<task_count; i++) {
        // execute the first queued task in the list and return
        Task& task = *m_tasks[i];
        if (task.status == Task::QUEUED) {
            // start the task
            is_busy = true;
            task.status = Task::RUNNING;
            m_current_task = &task;

            m_converter->start(task.param);
            emit start_conversion(i, task.param);

            return;
        }
    }

    // no task is executed
    this->stop();
    emit all_tasks_finished();
}

void ConvertList::stop()
{
    if (m_current_task) {
        progress_refreshed(0);
        m_current_task->status = Task::QUEUED;
        m_current_task = 0;
    }
    is_busy = false;
    m_converter->stop();
}

void ConvertList::removeSelectedItems()
{
    remove_items(m_list->selectedItems());
}

void ConvertList::removeCompletedItems()
{
    QList<QTreeWidgetItem*> itemList;
    foreach (TaskPtr task, m_tasks) {
        if (task->status == Task::FINISHED) {
            itemList.push_back(task->listitem);
        }
    }
    remove_items(itemList);
}

void ConvertList::editSelectedParameters()
{
    QList<QTreeWidgetItem*> itemList = m_list->selectedItems();

    if (itemList.isEmpty())
        return;

    // the index of the first selected item
    const int sel_begin = m_list->indexOfTopLevelItem(itemList[0]);
    ConversionParameters param = m_tasks[sel_begin]->param;

    ConversionParameterDialog dialog(this->parentWidget());

    if (dialog.exec(param)) {
        foreach (QTreeWidgetItem* item, itemList) {
            const int index = m_list->indexOfTopLevelItem(item);
            m_tasks[index]->param = param;
        }
    }
}

void ConvertList::clear()
{
    QList<QTreeWidgetItem*> itemList;
    foreach (TaskPtr task, m_tasks) {
        itemList.push_back(task->listitem);
    }
    remove_items(itemList);
}

// Private Slots
void ConvertList::task_finished_slot(int exitcode)
{
    if (m_current_task) {

        m_current_task->status = (exitcode == 0)
                ? Task::FINISHED
                : Task::FAILED;

        if (exitcode) { // conversion failed
            ProgressBar *widget
                    = (ProgressBar*)m_list->itemWidget(m_current_task->listitem, m_progress_column_index);
            widget->setValue(0);
            /*: The text to be displayed on the progress bar when a conversion fails */
            m_current_task->listitem->setText(m_progress_column_index, tr("Failed"));
        }

        m_current_task = 0;
        emit task_finished(exitcode);

        is_busy = false;
        this->start(); // start next task
    }
}

void ConvertList::progress_refreshed(int percentage)
{
    if (m_current_task) {
        qDebug() << "Progress Refreshed: " << percentage << "%";
        ProgressBar *widget
                = (ProgressBar*)m_list->itemWidget(m_current_task->listitem, m_progress_column_index);
        widget->setValue(percentage);
    }
}

// Events

bool ConvertList::list_keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) { // Remove all selected items.
        removeSelectedItems();
        return true; // processed
    } else {
        return false; // not processed
    }
}

void ConvertList::list_dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void ConvertList::list_dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void ConvertList::list_dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

// The user drops files into the area.
void ConvertList::list_dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        AddTaskWizard wizard;

        // Fill in the filenames and execute the wizard.
        // The wizard will skip the file-selecting page.
        wizard.exec(urlList);

        const QList<ConversionParameters> &paramList = wizard.getConversionParameters();
        addTasks(paramList);
    }
}

// Initialize the QTreeWidget listing files.
void ConvertList::init_treewidget(QTreeWidget *w)
{
    assert(w != 0);

    w->setColumnCount(4);

    QStringList columnTitle;
    /*: label of the "input file" field */
    columnTitle.append(tr("Input"));
    /*: label of the "output file" field */
    columnTitle.append(tr("Output"));
    /*: label of the "media duration (length)" field */
    columnTitle.append(tr("Duration"));
    /*: label of the "progress" field. */
    columnTitle.append(tr("Progress"));

    m_progress_column_index = 3;

    w->setHeaderLabels(columnTitle);
    //w->header()->setMovable(false); // disable title drag-drop reordering

    w->setRootIsDecorated(false);
    w->setUniformRowHeights(true);

}

// Remove items in the list.
// A progress dialog is shown if the operation takes time longer than MIN_DURATION.
void ConvertList::remove_items(const QList<QTreeWidgetItem *>& itemList)
{
    /*: Remove files from the tasklist */
    QProgressDialog dlgProgress(tr("Removing files..."),
                                tr("Cancel"),
                                0, itemList.count(),
                                this);
    dlgProgress.setWindowModality(Qt::WindowModal);
    dlgProgress.setMinimumDuration(MIN_DURATION);

    int progress_count = 0;
    foreach (QTreeWidgetItem *item, itemList) {
        // Update the progress value.
        dlgProgress.setValue(++progress_count);

        // Check if the user has canceled the operation.
        if (dlgProgress.wasCanceled())
            break;

        removeTask(m_list->indexOfTopLevelItem(item));
    }

    dlgProgress.setValue(itemList.size());
}
