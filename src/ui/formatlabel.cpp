#include "ui/formatlabel.h"

#define MARGIN_X 6

FormatLabel::FormatLabel(const QString &fileExt, QWidget *parent)
    : QLabel(fileExt, parent)
{
    setStyleSheet("border-radius: 2px;"
                  "background-color: #f50;"
                  "foreground-color: #fff;"
                  "font: bold 'monospace';");
    setAlignment(Qt::AlignCenter);

    int w = fontMetrics().boundingRect(text()).width();
    w += MARGIN_X * 2;
    setFixedWidth(w);
}

QStringList FormatLabel::getColorSet(const QString &fileExt)
{
    return QStringList();
}
