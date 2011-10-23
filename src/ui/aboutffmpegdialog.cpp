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

#include "aboutffmpegdialog.h"
#include "ui_aboutffmpegdialog.h"
#include "converter/ffmpeginterface.h"

AboutFFmpegDialog::AboutFFmpegDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutFFmpegDialog)
{
    ui->setupUi(this);

    QTextBrowser *info = ui->txtInfo;
    QTextBrowser *codecinfo = ui->txtCodecInfo;

    info->setText(
         /*: ffmpeg description */
         tr("FFmpeg is a complete, cross-platform solution to record, "
            "convert and stream audio and video. It includes "
            "libavcodec - the leading audio/video codec library.")
         + "<br><br>"
         + tr("FFmpeg is free software licensed under the LGPL or GPL.")
         + "<br><br>"
         /*: visit ffmpeg website */
         + tr("Please visit %1 for more information.")
                .arg("<a href=\"http://ffmpeg.org\">http://ffmpeg.org</a>")
         + "<br><br>"
         + FFmpegInterface::getFFmpegVersionInfo().replace("\n", "<br>")
         );

    info->setOpenExternalLinks(true);

    info->setFrameShape(QTextBrowser::NoFrame);
    codecinfo->setFrameShape(QTextBrowser::NoFrame);

    ui->tabFFmpeg->setAutoFillBackground(true);
    ui->tabCodecs->setAutoFillBackground(true);

    QPalette p = info->palette();
    p.setColor(QPalette::Base, ui->tabFFmpeg->palette().color(QPalette::Window));

    info->setPalette(p);
    codecinfo->setPalette(p);

    codecinfo->setText(FFmpegInterface::getFFmpegCodecInfo());

}

AboutFFmpegDialog::~AboutFFmpegDialog()
{
    delete ui;
}
