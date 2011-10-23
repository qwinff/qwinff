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

/* This file is taken from smplayer */

#include "extensions.h"

ExtensionList::ExtensionList() : QStringList()
{
}

QString ExtensionList::forFilter() {
	QString s;
	for (int n=0; n < count(); n++) {
		s = s + "*." + at(n) + " ";
	}
	if (!s.isEmpty()) s = " (" + s + ")";
	return s;
}

QString ExtensionList::forRegExp() {
	QString s;
	for (int n=0; n < count(); n++) {
		if (!s.isEmpty()) s = s + "|";
		s = s + "^" + at(n) + "$";
	}
	return s;
}

Extensions::Extensions()
{
	_video << "avi" << "vfw" << "divx" 
           << "mpg" << "mpeg" << "m1v" << "m2v" << "mpv" << "dv" << "3gp"
           << "mov" << "mp4" << "m4v" << "mqv"
           << "dat" << "vcd"
           << "ogg" << "ogm" << "ogv"
           << "asf" << "wmv"
           << "bin" << "iso" << "vob"
           << "mkv" << "nsv" << "ram" << "flv"
           << "rm" << "swf"
           << "ts" << "rmvb" << "dvr-ms" << "m2t" << "m2ts" << "rec"
           << "mts";

	_audio << "mp3" << "ogg" << "wav" << "wma" <<  "ac3" << "ra" << "ape" << "flac";

        // multimedia = union of video and audio
	_multimedia = _video;
	for (int n = 0; n < _audio.count(); n++) {
		if (!_multimedia.contains(_audio[n])) _multimedia << _audio[n];
	}

}

Extensions::~Extensions() {
}

