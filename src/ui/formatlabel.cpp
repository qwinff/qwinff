#include "ui/formatlabel.h"
#include "services/constants.h"

#define MARGIN_X 6

FormatLabel::FormatLabel(const QString &fileExt, QWidget *parent)
    : QLabel(fileExt, parent) {
    QMap<QString, QString> color_map = getColorMap(fileExt);
    setStyleSheet(QString("border-radius: 2px;"
                          "background-color: %1;"
                          "color: %2;"
                          "border: 1px solid %3;"
                          "font: bold 'monospace';").arg(color_map["background"])
                                                    .arg(color_map["foreground"])
                                                    .arg(color_map["border"]));
    setAlignment(Qt::AlignCenter);

    int w = fontMetrics().boundingRect(text()).width();
    w += MARGIN_X * 2;
    setFixedWidth(w);
}

QMap<QString, QString> FormatLabel::getColorMap(const QString &fileExt){
    QString EXT = fileExt.toUpper();
    std::string key_string = "FormatLabel/Format_" + EXT.toStdString();
    const char *key = key_string.c_str();
    QMap<QString, QString> map = Constants::getEntryAttributesMap(key);
    if (!map.contains("background"))
    {
        QMap<QString, QString> default_color_map;
        default_color_map["foreground"] = "#fff";
        default_color_map["background"] = "#888";
        default_color_map["border"] = "#444";
        return default_color_map;
    }
    return map;
}
