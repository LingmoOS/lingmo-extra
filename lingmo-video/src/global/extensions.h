/*  GUI front-end for mpv.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _EXTENSIONS_H_
#define _EXTENSIONS_H_

#include <QStringList>

class ExtensionList : public QStringList
{
public:
	ExtensionList();

	QString forFilter();
	QStringList forDirFilter();
	QString forRegExp();
};

class Extensions 
{
public:
	Extensions();
	~Extensions();

    ExtensionList video() { return _video; }
    ExtensionList audio() { return _audio; }
    ExtensionList playlist() { return _playlist; }
    ExtensionList subtitles() { return _subtitles; }
    ExtensionList multimedia() { return _multimedia; }
    ExtensionList allPlayable() { return _all_playable; }

    static QString extensionFromUrl(const QString & url);
protected:
	ExtensionList _video, _audio, _playlist, _subtitles;
	ExtensionList _multimedia; //!< video and audio
	ExtensionList _all_playable; //!< video, audio and playlist
};

#endif
