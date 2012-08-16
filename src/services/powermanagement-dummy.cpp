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

/* Dummy Implementation of PowerManagement class that does nothing */

#include "powermanagement.h"

static const bool verbose = true;

bool PowerManagement::suspend()
{
    return false;
}

bool PowerManagement::shutdown()
{
    return false;
}

bool PowerManagement::implemented()
{
    /* Returns false because this implementation doesn't provide any
       power management functionality.
     */
    return false;
}
