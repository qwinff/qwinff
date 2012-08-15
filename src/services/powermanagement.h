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

#ifndef POWERMANAGEMENT_H
#define POWERMANAGEMENT_H

class PowerManagement
{
public:

    /*
       Power Management Functions:
       return true on success, false otherwise
    */
    static bool suspend();
    static bool shutdown();

    /*
       This function is used to check whether there's an implementation
       for the current build. You should always return true in this function
       when implementing PowerManagement for another system/environment.
     */
    static bool implemented();

private:
    PowerManagement();
};

#endif // POWERMANAGEMENT_H
