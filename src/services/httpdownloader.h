/* qtypist - a typing training program
 *
 * Copyright (C) 2012 Timothy Lin <lzh9102@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or 3 of the License.
 *
 * project page: http://code.google.com/p/qtypist
 */

#ifndef HTTPDOWNLOADER_H
#define HTTPDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QMap>

class HttpDownloader : public QObject
{
    Q_OBJECT
public:
    explicit HttpDownloader(QObject *parent = 0);

    virtual ~HttpDownloader();

public slots:
    void startDownload(QString url);
    void cancelAllDownloads();

signals:
    void downloadFinished(bool success, QString url, QString content);

private slots:
    void slotDownloadFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager m_webCtrl;
    QMap<QNetworkReply*, QString> m_downloads;
};

#endif // HTTPDOWNLOADER_H
