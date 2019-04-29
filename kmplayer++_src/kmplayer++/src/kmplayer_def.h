/*
 * Copyright (C) 2011 Koos Vriezen <koos.vriezen@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef _KMPLAYER_DEF_H_
#define _KMPLAYER_DEF_H_

#define VERSION "0.11.0"
#define ASSERT Q_ASSERT
#if __GNUC__ - 0 > 3 || (__GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 > 3)
  #define KMPLAYER_NO_CDTOR_EXPORT __attribute__ ((visibility("hidden")))
  #define KMPLAYER_NO_EXPORT __attribute__ ((visibility("hidden")))
  #define KMPLAYER_EXPORT __attribute__ ((visibility("default")))
#elif __GNUC__ - 0 > 3 || (__GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 > 2)
  #define KMPLAYER_NO_CDTOR_EXPORT
  #define KMPLAYER_NO_EXPORT __attribute__ ((visibility("hidden")))
  #define KMPLAYER_EXPORT
#else
  #define KMPLAYER_NO_CDTOR_EXPORT
  #define KMPLAYER_NO_EXPORT
  #define KMPLAYER_EXPORT
#endif
#ifndef KDE_EXPORT
  #define KDE_EXPORT KMPLAYER_EXPORT
  #define KDE_NO_EXPORT KMPLAYER_NO_EXPORT
  #define KDE_NO_CDTOR_EXPORT KMPLAYER_NO_CDTOR_EXPORT
#endif

#define GCONF_KEY_PREFIX "/apps/meego/kmplayer"
#define GCONF_KEY_DATA_DIR GCONF_KEY_PREFIX"/data_dir"
#define GCONF_KEY_RECENT_FILE GCONF_KEY_PREFIX"/recent_file"
#define GCONF_KEY_PLAYLIST_FILE GCONF_KEY_PREFIX"/playlist_file"
#define GCONF_KEY_LOCATION_URL GCONF_KEY_PREFIX"/location"
#define GCONF_KEY_PLAYER GCONF_KEY_PREFIX"/player"
#define GCONF_KEY_VO GCONF_KEY_PREFIX"/video_output"
#define GCONF_KEY_SAVE_FOLDER GCONF_KEY_PREFIX"/save_folder"
#define GCONF_KEY_MIME GCONF_KEY_PREFIX"/mimetypes/"
#define GCONF_KEY_LISTS GCONF_KEY_PREFIX"/lists"
#define GCONF_KEY_LISTS_VIEW GCONF_KEY_PREFIX"/lists_view"
#define GCONF_KEY_KEY_DIR GCONF_KEY_PREFIX"/keys"
#define GCONF_KEY_KEYS GCONF_KEY_KEY_DIR"/"

#define USER_ROOT_DIRECTORY "/home/user/MyDocs"

#define HSPLIT 380
#endif //_KMPLAYER_DEF_H_
