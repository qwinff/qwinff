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

#include "httpdownloader.h"
#include <QFile>
#include <vector>

#define BUFFER_SIZE 1024

HttpDownloader::HttpDownloader(QObject *parent)
    : QObject(parent), m_sizeLimit(0)
{
    connect(&m_webCtrl, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotDownloadFinished(QNetworkReply*)));
}

HttpDownloader::~HttpDownloader()
{
    cancelAllDownloads();
}

void HttpDownloader::setSizeLimit(unsigned int limit)
{
    m_sizeLimit = limit;
}

unsigned int HttpDownloader::sizeLimit() const
{
    return m_sizeLimit;
}

void HttpDownloader::startDownload(QString url)
{
    QNetworkReply *reply = m_webCtrl.get(QNetworkRequest(url));
    m_downloads[reply] = url;
}

void HttpDownloader::cancelAllDownloads()
{
    QList<QNetworkReply*> replies = m_downloads.keys();
    foreach (QNetworkReply *reply, replies) {
        reply->abort();
        reply->deleteLater();
    }
    m_downloads.clear();
}

void HttpDownloader::slotDownloadFinished(QNetworkReply *reply)
{
    QString url = m_downloads[reply];
    bool success = !reply->error();
    QString content;
    readData(content, reply);
    m_downloads.remove(reply);
    reply->deleteLater();
    emit downloadFinished(success, url, content);
}

// read at most m_sizeLimit bytes from reply to dest
// or read all data if m_sizeLimit is 0
void HttpDownloader::readData(QString &dest, QNetworkReply *reply)
{
    if (m_sizeLimit == 0) {
        dest = reply->readAll();
    } else {
        std::vector<char> buffer;
        buffer.reserve(m_sizeLimit + 1); // reserve data length and NULL byte
        reply->read(buffer.data(), m_sizeLimit); // buffer.data() is char*
        buffer[m_sizeLimit] = 0; // terminate the data with NULL
        dest = QString(buffer.data()); // convert the data to QString
    }
}
