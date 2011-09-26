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

