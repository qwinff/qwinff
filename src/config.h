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

/* Modify this file to set several compile-time parameters. */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Whether to check for updates on program startup by default.
 *
 * This value will be overridden by QSetting value
 * "options/check_update_on_startup".
 *
 * type: boolean
 */
#define DEFAULT_CHECK_UPDATE_ON_STARTUP true

#endif // CONFIG_H
