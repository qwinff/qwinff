/*  QWinFF - a qt4 gui frontend for ffmpeg
 *  Copyright (C) 2011-2013 Timothy Lin <lzh9102@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file extra-translations.h
 * @brief File containing extra translatable strings.
 *
 * This file contains translatable strings not used in the
 * program, but used in other places (website, desktop files...).
 * These strings will be extracted by lupdate so that they can be
 * translated just like other strings do. Not including this file
 * in any source files prevents generating extra object code.
 */

#ifndef EXTRATRANSLATIONS_H
#define EXTRATRANSLATIONS_H

#include <QObject>

class ExtraTranslations : QObject
{
    Q_OBJECT
private:
    ExtraTranslations()
    {
        tr("Convert between media file formats");
        tr("Media Converter");
    }
};

#endif // EXTRATRANSLATIONS_H
