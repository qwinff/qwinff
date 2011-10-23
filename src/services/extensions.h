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

/* This file is taken from smplayer. */

#ifndef _EXTENSIONS_H_
#define _EXTENSIONS_H_

#include <QStringList>

class ExtensionList : public QStringList
{
public:
	ExtensionList();

	QString forFilter();
	QString forRegExp();
};

class Extensions 
{
public:
	Extensions();
	~Extensions();

	ExtensionList video() { return _video; };
	ExtensionList audio() { return _audio; };
	ExtensionList multimedia() { return _multimedia; };

protected:
        ExtensionList _video, _audio;
	ExtensionList _multimedia; //!< video and audio
};

#endif
