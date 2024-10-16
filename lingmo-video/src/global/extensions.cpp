/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>

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

#include "extensions.h"
#include <QDebug>

ExtensionList::ExtensionList() : QStringList()
{
}

QString ExtensionList::forFilter() {
	QString s;
	for (int n=0; n < count(); n++) {
		s = s + "*." + at(n) + " ";
	}
//	if (!s.isEmpty()) s = " (" + s + ")";
	return s;
}

QStringList ExtensionList::forDirFilter() {
	QStringList l;
	for (int n=0; n < count(); n++) {
		QString s = "*." + at(n);
		l << s;
	}
	return l;
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
    _video << "dvr-ms" << "mp4" << "avi" << "rmvb" << "flv" << "vfw" << "divx" << "264" << "265"
       << "mpg" << "mpeg" << "m1v" << "m2v" << "mpv" << "dv" << "3gp" << "3p2" << "3g2"
       << "mov" << "m4v" << "mqv" << "dat" << "vcd" << "ogg" << "ogm" << "ogv" << "ogx"
       << "asf" << "wmv" << "bin" << "iso" << "vob" << "mkv" << "nsv" << "ram"
       << "rm" << "swf" << "ts" << "m2t" << "m2ts" << "mts" << "rec" << "wtv"
       << "f4v" << "hdmov" << "webm" << "vp8"
       << "bik" << "smk" << "m4b" << "wtv"
       << "part" << "dpg" << "m2p" << "tp";

    _audio << "mp3" << "ogg" << "oga" << "ac3" << "aiff" << "voc" << "au" << "m4p"
        << "ra" << "ape" << "flac" << "thd" << "mka" << "opus" << "mmf" << "mp2" << "wv"
       << "m4r" << "wav" << "dts" << "wma" << "m4a" << "aac" << "amr" << "mpa";

    _subtitles << "aqt" << "srt" << "sub" << "ssa" << "ass" << "idx" << "smi"
           << "rt" << "utf" << "vtt" << "sup";

    _playlist << "m3u" << "m3u8" << "pls" << "xspf";

    _multimedia = _video;
    for (int n = 0; n < _audio.count(); n++) {
        if (!_multimedia.contains(_audio[n])) _multimedia << _audio[n];
    }

    _all_playable << _multimedia << _playlist;
}

Extensions::~Extensions() {
}

QString Extensions::extensionFromUrl(const QString & url) {
    //qDebug() << "Extensions::extensionFromUrl:" << url;

    QString extension;
    int pos = url.lastIndexOf(".");
    if (pos != -1) {
        extension = url.mid(pos+1).toLower();
        // Check if extension contains a '?' and remove everything after it
        pos = extension.lastIndexOf("?");
        if (pos != -1) {
            extension = extension.left(pos);
        }
    }

    //qDebug() << "Extensions::extensionFromUrl: extension:" << extension;
    return extension;
}
