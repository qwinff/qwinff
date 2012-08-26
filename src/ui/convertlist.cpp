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

#include "convertlist.h"
#include "progressbar.h"
#include "converter/mediaconverter.h"
#include "converter/mediaprobe.h"
#include "services/filepathoperations.h"
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
#include <QFileDialog>
#include <QInputDialog>
#include <cassert>

#define TIMEOUT 3000
#define MIN_DURATION 100 // Minimum duration(milliseconds) to show progress dialog.

/* This enum defines the columns of the list.
   The last item is always COL_COUNT, which is used to identify
   how many columns there are. To add a new item, just follow the
   following steps:

     (1) Add a new enumeration constant to ConvertListColumns before
         COL_COUNT.

     (2) Search for the function "init_treewidget_fill_column_titles"
         and fill in the title of the new field there. Read the
         instruction before the function body for details.

     (3) Search for the function "init_treewidget_columns_visibility"
         and set the default visibility of the field.

     (4) Search for the function "fill_list_fields". This function is
         called when a new task is being added to the list. Fill in
         the field according to the parameter and the probing result.

 */
enum ConvertListColumns
{
    COL_SOURCE,
    COL_DESTINATION,
    COL_DURATION,
    COL_FILE_SIZE,
    COL_AUDIO_SAMPLE_RATE,
    COL_AUDIO_BITRATE,
    COL_AUDIO_CHANNELS,
    COL_AUDIO_CODEC,
    COL_VIDEO_DIMENSIONS,
    COL_VIDEO_BITRATE,
    COL_VIDEO_FRAMERATE,
    COL_VIDEO_CODEC,
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
            return true;
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
    run_next(false),
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
    connect(m_list, SIGNAL(itemSelectionChanged()),
            this, SIGNAL(itemSelectionChanged()));

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

    m_startTime.start();
}

ConvertList::~ConvertList()
{
    QSettings settings;
    settings.setValue("convertlist/header_state", m_list->header()->saveState());
}

bool ConvertList::addTask(ConversionParameters param)
{
    // get source file information
    qDebug() << "Probe media file: " << param.source;

    if (!m_probe->run(param.source, TIMEOUT)) {
        if (m_probe->error())
            qDebug() << "Failed to get media information";
        else
            qDebug() << "FFprobe timeout";
        // failed to get media information immediately
        return false;
    }

    /* Ensure unique output filename.
       If the destination filename already exists either on disk
       or in the ConvertList, rename it to prevent overwritting
       completed tasks.  */
    param.destination =
            FilePathOperations::GenerateUniqueFileName(param.destination, output_filenames());
    output_filenames_push(param.destination); // Record the filename for future reference.

    TaskPtr task(new Task());
    task->param = param;
    task->status = Task::QUEUED;
    task->id = ++prev_index;

    m_tasks.push_back(task);

    QStringList columns;
    for (int i=0; i<COL_COUNT; i++)
        columns.append(QString());

    fill_list_fields(param, *m_probe, columns);

    QTreeWidgetItem *item = new QTreeWidgetItem(m_list, columns);
    task->listitem = item;

    // Add a progress bar widget to the list item
    m_list->addTopLevelItem(item);
    m_list->setItemWidget(item, COL_PROGRESS, new ProgressBar());
    m_list->itemWidget(item, COL_PROGRESS)->adjustSize();

    item->setToolTip(COL_SOURCE, param.source);
    item->setToolTip(COL_DESTINATION, param.destination);

    for (int i=0; i<COL_COUNT; i++) {
        if (i!=COL_SOURCE && i!=COL_DESTINATION && i!=COL_PROGRESS
                && !columns[i].isEmpty()) {
			   /*: FieldName: Value */
            item->setToolTip(i, tr("%1: %2")
                             .arg(m_list->headerItem()->text(i))
                             .arg(columns[i]));
        }
    }

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
        output_filenames_pop(m_tasks[index]->param.destination);
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

int ConvertList::elapsedTime() const
{
    return is_busy ? m_startTime.elapsed() : 0;
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

bool ConvertList::selectedTaskFailed() const
{
    if (selectedCount() != 1)
        return false;
    const int index = m_list->currentIndex().row();
    if (index >= 0 && index < m_tasks.size())
        return m_tasks[index]->status == Task::FAILED;
    else
        return false;
}

// Public Slots

void ConvertList::start()
{
    if (is_busy && !run_next)
        return;

    run_next = false;

    if (!is_busy) { // new session: start timing
        m_startTime.restart();
        emit started();
    }

    const int task_count = m_tasks.size();

    if (task_count == 0) { // the task list is empty
        this->stop();
        return;
    }

    for (int i=0; i<task_count; i++) {
        // execute the first queued task in the list and return
        Task& task = *m_tasks[i];
        if (task.status == Task::QUEUED) {
            QSettings settings;

            // start the task
            is_busy = true;
            task.status = Task::RUNNING;
            m_current_task = &task;

            progressBar(task)->setActive(true);

            task.param.threads = settings.value("options/threads", DEFAULT_THREAD_COUNT).toInt();
            qDebug() << "Threads: " + QString::number(task.param.threads);

            m_converter->start(task.param);
            emit start_conversion(i, task.param);

            return;
        }
    }

    // no task is executed
    this->stop();
    emit all_tasks_finished();
    emit stopped();
}

void ConvertList::stop()
{
    if (m_current_task) {
        progress_refreshed(0);
        m_current_task->status = Task::QUEUED;
        progressBar(m_current_task)->setActive(false);
        m_current_task = 0;
        emit stopped();
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
            // copy conversion parameters
            // Be sure not to use assignment because it will overwrite the filename.
            m_tasks[index]->param.copyConfigurationFrom(param);
            // Reset m_tasks[index] to "QUEUED" state if it is not running.
            reset_item(index);
        }
    }
}

void ConvertList::changeSelectedOutputFile()
{
    QList<QTreeWidgetItem*> itemList = m_list->selectedItems();

    if (itemList.size() != 1)
        return;

    QTreeWidgetItem *item = itemList[0];

    // Get the index of the first selected item.
    const int index = m_list->indexOfTopLevelItem(item);

    ConversionParameters &param = m_tasks[index]->param;

    QString orig_name = QFileInfo(param.destination).completeBaseName();
    QString dir = QFileInfo(param.destination).path();
    QString ext = QFileInfo(param.destination).suffix();
    QString new_name = orig_name;

    bool try_again = false;
    do {
        new_name = QInputDialog::getText(this, tr("New File Name")
                    , tr("Please input the new name for the output file.")
                    , QLineEdit::Normal, new_name);

        try_again = false;
        if (!new_name.isEmpty() && new_name != orig_name) {
            QString orig_file = param.destination;
            QString file = QDir(dir).absoluteFilePath(new_name + "." + ext);
            QMessageBox::StandardButtons overwrite = QMessageBox::No;
            if (!change_output_file(index, file, overwrite, false))
                try_again = true;
        }
    } while (try_again);
}

void ConvertList::changeSelectedOutputDirectory()
{
    QList<QTreeWidgetItem*> itemList = m_list->selectedItems();

    if (itemList.isEmpty()) // No item is selected.
        return;

    QTreeWidgetItem *item = itemList[0];

    // Get the index of the first selected item.
    int index = m_list->indexOfTopLevelItem(item);

    ConversionParameters &param = m_tasks[index]->param;

    QString orig_path = QFileInfo(param.destination).path();

    QString path = QFileDialog::getExistingDirectory(this, tr("Output Directory")
                                     , orig_path);

    if (!path.isEmpty()) {
        // Apply the output path to all selected items
        QMessageBox::StandardButtons overwrite = QMessageBox::No;
        foreach (item, itemList) {
            index = m_list->indexOfTopLevelItem(item);
            QString orig_file = m_tasks[index]->param.destination;
            QString name = QFileInfo(orig_file).fileName();
            QString file = QDir(path).absoluteFilePath(name);

            change_output_file(index, file, overwrite, true);
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

void ConvertList::showErrorMessage()
{
    const int index = m_list->currentIndex().row();
    if (index >= 0 && index < m_tasks.size()) {
        QMessageBox msgBox;
        msgBox.setText(tr("Error Message from FFmpeg:\n\n") + m_tasks[index]->errmsg);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
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

        ProgressBar *prog = progressBar(m_current_task);

        if (exitcode) { // conversion failed
            prog->setValue(0);
            /*: The text to be displayed on the progress bar when a conversion fails */
            prog->showText(tr("Failed"));
            m_current_task->errmsg = m_converter->errorMessage();
            //: %1 is the error message
            prog->setToolTip(tr("Error: %1").arg(m_current_task->errmsg));
        } else {
            /*: The text to be displayed on the progress bar when a conversion finishes */
            prog->showText(tr("Finished"));
            prog->setToolTip(tr("Finished"));
        }

        prog->setActive(false);
        m_current_task = 0;
        emit task_finished(exitcode);

        run_next = true;
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

    // Restore default value.
    init_treewidget_columns_visibility(m_list);
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
        AddTaskWizard wizard(m_presets, parentWidget());

        // Fill in the filenames and execute the wizard.
        // The wizard will skip the file-selecting page.
        wizard.exec(urlList);

        const QList<ConversionParameters> &paramList = wizard.getConversionParameters();
        addTasks(paramList);
    }
}

// Functions to access m_outputFileNames

void ConvertList::output_filenames_push(const QString& filename)
{
    if (m_outputFileNames.contains(filename)) {
        ++m_outputFileNames[filename];
    } else {
        m_outputFileNames.insert(filename, 1);
    }
}

void ConvertList::output_filenames_pop(const QString &filename)
{
    int count = m_outputFileNames.value(filename, 0);
    if (count > 1) {
        --m_outputFileNames[filename];
    } else if (count == 1) {
        m_outputFileNames.remove(filename);
    }
}

QHash<QString, int>& ConvertList::output_filenames()
{
    return m_outputFileNames;
}


// Initialize the QTreeWidget listing files.
void ConvertList::init_treewidget(QTreeWidget *w)
{
    Q_ASSERT_X(w, "ConvertList::init_treewidget", "w: null pointer");

    w->setColumnCount(COL_COUNT);

    QStringList columnTitle;
    for (int i=0; i<COL_COUNT; i++) {
        columnTitle.append(QString());
    }

    // Set column titles.
    init_treewidget_fill_column_titles(columnTitle);

    w->setHeaderLabels(columnTitle);
    //w->header()->setMovable(false); // disable title drag-drop reordering

    w->setRootIsDecorated(false);
    w->setUniformRowHeights(true);

    init_treewidget_columns_visibility(w);
}

/* Fill in the column titles of each field.
   Use the COL_* macros as the index.
   Example: columnTitle[COL_SOURCE] = tr("Source");
*/
void ConvertList::init_treewidget_fill_column_titles(QStringList &columnTitle)
{
    columnTitle[COL_SOURCE] = tr("Source");
    columnTitle[COL_DESTINATION] = tr("Destination");
    columnTitle[COL_DURATION] = tr("Duration");
    columnTitle[COL_FILE_SIZE] = tr("File Size");

    // Audio Information
    columnTitle[COL_AUDIO_SAMPLE_RATE] = /*: Audio */ tr("Sample Rate");
    columnTitle[COL_AUDIO_BITRATE] = tr("Audio Bitrate");
    columnTitle[COL_AUDIO_CHANNELS] = /*: Audio */ tr("Channels");
    columnTitle[COL_AUDIO_CODEC] = tr("Audio Codec");

    // Video Information
    columnTitle[COL_VIDEO_DIMENSIONS] = tr("Dimensions");
    columnTitle[COL_VIDEO_BITRATE] = tr("Video Bitrate");
    columnTitle[COL_VIDEO_FRAMERATE] = /*: Video */ tr("Framerate");
    columnTitle[COL_VIDEO_CODEC] = tr("Video Codec");

    columnTitle[COL_PROGRESS] = tr("Progress");
}

/* Set the default visibility of each field.
   This configuration will be overriden by user settings.
   For example, to make the duration field invisible by default, write
   w->hideColumn(COL_DURATION, true);
*/
void ConvertList::init_treewidget_columns_visibility(QTreeWidget *w)
{
    w->hideColumn(COL_FILE_SIZE);
    // Audio Information
    w->hideColumn(COL_AUDIO_SAMPLE_RATE);
    w->hideColumn(COL_AUDIO_BITRATE);
    w->hideColumn(COL_AUDIO_CHANNELS);
    w->hideColumn(COL_AUDIO_CODEC);
    // Video Information
    w->hideColumn(COL_VIDEO_DIMENSIONS);
    w->hideColumn(COL_VIDEO_BITRATE);
    w->hideColumn(COL_VIDEO_FRAMERATE);
    w->hideColumn(COL_VIDEO_CODEC);
}

/* Fill in the columns of the list according to the conversion parameter
   and the probing results. QStringList columns will be filled with empty
   items in advanced and the MediaProbe probe will be ready for reading.
   Just write available information to the corresponding fields.
   Use the COL_* macros as the array index.
   Example: columns[COL_SOURCE] = param.source;
*/
void ConvertList::fill_list_fields(ConversionParameters &param, MediaProbe &probe,
                                    QStringList &columns)

{
    columns[COL_SOURCE] = QFileInfo(param.source).fileName(); // source file
    columns[COL_DESTINATION] = QFileInfo(param.destination).fileName(); // destination file
    columns[COL_DURATION] = QString().sprintf("%02d:%02d:%02.0f"   // duration
                  , probe.hours()              //    hours
                  , probe.minutes()            //    minutes
                  , probe.seconds());          //    seconds
    // File Size
    columns[COL_FILE_SIZE] = to_human_readable_size_1024(QFileInfo(param.source).size());

    // Audio Information
    if (probe.hasAudio()) {
        columns[COL_AUDIO_SAMPLE_RATE] = tr("%1 Hz").arg(probe.audioSampleRate());
        columns[COL_AUDIO_BITRATE] = tr("%1 kb/s").arg(probe.audioBitRate());
        columns[COL_AUDIO_CHANNELS] = QString::number(probe.audioChannels());
        columns[COL_AUDIO_CODEC] = probe.audioCodec();
    }

    // Video Information
    if (probe.hasVideo()) {
        columns[COL_VIDEO_DIMENSIONS] = QString("%1x%2")
                .arg(probe.videoWidth()).arg(probe.videoHeight());
        columns[COL_VIDEO_BITRATE] = tr("%1 kb/s").arg(probe.videoBitRate());
        columns[COL_VIDEO_FRAMERATE] = tr("%1 fps").arg(probe.videoFrameRate());
        columns[COL_VIDEO_CODEC] = probe.videoCodec();
    }
}

// Reset the item to the queued state.
void ConvertList::reset_item(int index)
{
    if (index >= 0 && index < m_tasks.size()) {
        TaskPtr task = m_tasks[index];
        if (task->status != Task::RUNNING) {
            task->status = Task::QUEUED;
            ProgressBar *prog = progressBar(*task);
            prog->setValue(0);
            prog->setActive(false);
            prog->setToolTip("");
        }
    }
}

// Remove items in the list.
// A progress dialog is shown if the operation takes time longer than MIN_DURATION.
void ConvertList::remove_items(const QList<QTreeWidgetItem *>& itemList)
{
    /*: Remove files from the tasklist */
    QProgressDialog dlgProgress(tr("Removing tasks..."),
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

// Convert bytes to human readable form such as
// "10 KiB" instead of "102400 Bytes".
QString ConvertList::to_human_readable_size_1024(qint64 nBytes)
{
    float num = nBytes;
    QStringList list;
    list << tr("KiB") << tr("MiB") << tr("GiB") << tr("TiB");

    QStringListIterator i(list);
    QString /*: Bytes */ unit(tr("B"));

    while(num >= 1024.0 && i.hasNext()) {
        unit = i.next();
        num /= 1024.0;
    }

    return QString().setNum(num,'f',2)+" "+unit;
}

/* Change the output file of the task to new_file.
 * @return true if success, false if failed
 */
bool ConvertList::change_output_file(int index, const QString &new_file
        , QMessageBox::StandardButtons &overwrite, bool show_all_buttons)
{
    if (overwrite == QMessageBox::NoToAll) return false;

    ConversionParameters &param = m_tasks[index]->param;
    QTreeWidgetItem *item = m_tasks[index]->listitem;

    QString orig_file = param.destination;

    if (new_file == orig_file) return true; // success: no need to rename

    if ((QFileInfo(new_file).exists() || output_filenames().contains(new_file))
            && overwrite != QMessageBox::YesToAll) {
        QMessageBox::StandardButtons flags = QMessageBox::Yes | QMessageBox::No;
        if (show_all_buttons) {
            flags |= QMessageBox::YesToAll | QMessageBox::NoToAll;
        }
        // The file name already exists.
        // Ask the user whether to force using the file name.
        overwrite = QMessageBox::warning(this, tr("File Exists"),
                          tr("%1 already exists on disk or in the task list. "
                             "Still use this name as the output filename?").arg(new_file)
                          , flags);
        if (overwrite != QMessageBox::Yes && overwrite != QMessageBox::YesToAll)
            return false;
    }

    param.destination = new_file;

    // Rebuild the set of all output filenames in the list.
    output_filenames_pop(orig_file);
    output_filenames_push(new_file);

    // Update item text
    item->setText(COL_DESTINATION, QFileInfo(new_file).fileName());
    item->setToolTip(COL_DESTINATION, new_file);

    qDebug() << "Output filename changed: " + orig_file + " => " + new_file;
    return true;
}

