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
