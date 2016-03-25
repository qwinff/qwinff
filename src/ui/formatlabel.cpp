#include <QString>
#include <QLabel>
#include "ui/formatlabel.h"


FormatLabel::FormatLabel(const QString &fileExt, QWidget *parent)
    : QLabel(fileExt, parent) {
    setStyleSheet("border-radius: 2px;"
                  "background-color: #f50;"
                  "foreground-color: #fff;"
                  "font: bold 'monospace';");
    setAlignment(Qt::AlignCenter);
    int w = fontMetrics().boundingRect(text()).width();
    w += 12;
    setFixedWidth(w);
}

FormatLabel::getColorSet(const QString &fileExt)
{
    return QString()
}
