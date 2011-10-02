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
#include <QMenu>
#include <cassert>

#define TIMEOUT 3000
#define MIN_DURATION 100 // Minimum duration(milliseconds) to show progress dialog.

enum ConvertListColumns
{
    COL_SOURCE,
    COL_DESTINATION,
    COL_DURATION,
    COL_PROGRESS,
    COL_COUNT
};

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

ConvertList::ConvertList(Presets *presets, QWidget *parent) :
    QWidget(parent),
    m_list(new QTreeWidget(this)),
    m_listEventFilter(new ListEventFilter(this)),
    prev_index(0),
    m_converter(new MediaConverter(this)),
    m_probe(new MediaProbe(this)),
    m_current_task(0),
    is_busy(false),
    m_presets(presets)
{
    QLayout *layout = new QHBoxLayout(this);
    this->setLayout(layout);

    init_treewidget(m_list);
    layout->addWidget(m_list);

    connect(m_converter, SIGNAL(finished(int))
            , this, SLOT(task_finished_slot(int)));
    connect(m_converter, SIGNAL(progressRefreshed(int)),
            this, SLOT(progress_refreshed(int)));

    // Propagate context menu event.
    m_list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_list, SIGNAL(customContextMenuRequested(QPoint))
            , this, SIGNAL(customContextMenuRequested(QPoint)));

    // enable drag/drop functions
    m_list->setAcceptDrops(true);

    // allow selecting multiple items
    m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Propagate events from the QTreeWidget to ConvertList.
    m_list->installEventFilter(m_listEventFilter);

    QSettings settings;
    QHeaderView *header = m_list->header();
    header->restoreState(settings.value("convertlist/header_state").toByteArray());
    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotHeaderContextMenu(QPoint)));
}

ConvertList::~ConvertList()
{
    QSettings settings;
    settings.setValue("convertlist/header_state", m_list->header()->saveState());
}

bool ConvertList::addTask(const ConversionParameters& param)
{
    // get source file information
    qDebug() << "Probe media file: " << param.source;
    m_probe->start(param.source);

    if (!m_probe->wait(TIMEOUT) || m_probe->error()) {
        if (m_probe->error())
            qDebug() << "Failed to get media information";
        else
            qDebug() << "FFprobe timeout";
        // failed to get media information immediately
        return false;
    }

    TaskPtr task(new Task());
    task->param = param;
    task->status = Task::QUEUED;
    task->id = ++prev_index;

    m_tasks.push_back(task);

    QStringList columns;
    for (int i=0; i<COL_COUNT; i++)
        columns.append(QString());

    columns[COL_SOURCE] = QFileInfo(param.source).fileName(); // source file
    columns[COL_DESTINATION] = QFileInfo(param.destination).fileName(); // destination file
    columns[COL_DURATION] = QString().sprintf("%02d:%02d:%02.0f"   // duration
                  , m_probe->hours()              //    hours
                  , m_probe->minutes()            //    minutes
                  , m_probe->seconds());          //    seconds


    QTreeWidgetItem *item = new QTreeWidgetItem(m_list, columns);
    task->listitem = item;

    // Add a progress bar widget to the list item
    m_list->addTopLevelItem(item);
    m_list->setItemWidget(item, COL_PROGRESS, new ProgressBar());
    m_list->itemWidget(item, COL_PROGRESS)->adjustSize();

    item->setToolTip(COL_SOURCE, param.source);
    item->setToolTip(COL_DESTINATION, param.destination);

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

            progressBar(task)->setActive(true);

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
        progressBar(m_current_task)->setActive(false);
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
            m_tasks[index]->param.copyConfigurationFrom(param);
        }
    }
}

void ConvertList::retrySelectedItems()
{
    QList<QTreeWidgetItem*> itemList = m_list->selectedItems();

    if (itemList.isEmpty())
        return;

    foreach (QTreeWidgetItem* item, itemList) {
        const int index = m_list->indexOfTopLevelItem(item);
        reset_item(index);
    }

    start();
}

void ConvertList::retryAll()
{
    const int list_size = m_list->topLevelItemCount();
    for (int i=0; i<list_size; i++) {
        reset_item(i);
    }

    start();
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

        ProgressBar *prog = progressBar(m_current_task);

        if (exitcode) { // conversion failed
            prog->setValue(0);
            /*: The text to be displayed on the progress bar when a conversion fails */
            m_current_task->listitem->setText(COL_PROGRESS, tr("Failed"));
        }

        prog->setActive(false);
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
        progressBar(m_current_task)->setValue(percentage);
    }
}

void ConvertList::slotHeaderContextMenu(QPoint point)
{
    const int header_count = m_list->header()->count();
    const int current_column = m_list->header()->logicalIndexAt(point);

    // Count visible columns.
    int visible_column_count = 0, visible_column_index = 0;
    for (int i=0; i<header_count; i++) {
        if (!m_list->isColumnHidden(i)) {
            ++visible_column_count;
            visible_column_index = i;
        }
    }

    QMenu menu;

    // Add the item under the mouse to the list
    if (current_column >= 0 && visible_column_count > 1) {
        QAction *action = new QAction(&menu);
        QString column_text = m_list->headerItem()->text(current_column);
        /*: Hide a column in the list. For example, the text maybe 'Hide "Duration"'.
            The two \" are quotation marks in English,
            you may replace it with local quotation marks. */
        QString action_text = tr("Hide \"%1\"").arg(column_text);
        action->setText(action_text);
        action->setData(current_column);
        action->setCheckable(false);
        action->setChecked(false);
        menu.addAction(action);
    }

    QAction *actionRestore = new QAction(&menu);
    actionRestore->setText(tr("Restore All Columns"));
    actionRestore->setData(-1);
    connect(actionRestore, SIGNAL(triggered()),
            this, SLOT(slotRestoreListHeaders()));
    menu.addAction(actionRestore);

    menu.addSeparator();

    // Construct the rest of the menu and uncheck hidden items.
    for (int i=0; i<header_count; i++) {
        QString title = m_list->headerItem()->text(i);
        QAction *action = new QAction(title, &menu);
        action->setCheckable(true);
        action->setChecked(!m_list->isColumnHidden(i));
        action->setData(i); // save the column index

        // not allow user to hide the last column
        if (visible_column_count > 1 || visible_column_index != i)
            menu.addAction(action);
    }

    connect(&menu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotHeaderContextMenuTriggered(QAction*)));

    menu.exec(QCursor::pos());
}

void ConvertList::slotHeaderContextMenuTriggered(QAction *action)
{
    const int column_index = action->data().toInt();
    if (column_index >= 0)
        m_list->setColumnHidden(column_index, !action->isChecked());
}

void ConvertList::slotRestoreListHeaders()
{
    const int column_count = m_list->columnCount();
    QHeaderView *header = m_list->header();
    for (int i=0; i<column_count; i++) { // Restore all sections.
        m_list->showColumn(i);
        header->resizeSection(i, header->defaultSectionSize());
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
    const QMimeData *mimeData = event->mimeData();
    if (mimeData && mimeData->hasUrls())
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
    if (mimeData && mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        AddTaskWizard wizard(m_presets);

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

    w->setColumnCount(COL_COUNT);

    QStringList columnTitle;
    for (int i=0; i<COL_COUNT; i++) {
        columnTitle.append(QString());
    }

    columnTitle[COL_SOURCE] = tr("Input");
    columnTitle[COL_DESTINATION] = tr("Output");
    columnTitle[COL_DURATION] = tr("Duration");
    columnTitle[COL_PROGRESS] = tr("Progress");

    w->setHeaderLabels(columnTitle);
    //w->header()->setMovable(false); // disable title drag-drop reordering

    w->setRootIsDecorated(false);
    w->setUniformRowHeights(true);

}

// Reset the item to the queued state.
void ConvertList::reset_item(int index)
{
    if (index >= 0 && index < m_tasks.size()) {
        TaskPtr task = m_tasks[index];
        if (task->status != Task::RUNNING) {
            task->status = Task::QUEUED;
            task->listitem->setText(COL_PROGRESS, QString());
            ProgressBar *prog = progressBar(*task);
            prog->setValue(0);
            prog->setActive(false);
        }
    }
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

ProgressBar* ConvertList::progressBar(Task *task)
{
    return (ProgressBar*)m_list->itemWidget(task->listitem, COL_PROGRESS);
}

ProgressBar* ConvertList::progressBar(const Task &task)
{
    return (ProgressBar*)m_list->itemWidget(task.listitem, COL_PROGRESS);
}
