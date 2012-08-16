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

/* Windows32 Implementation of PowerManagement class */

#include "powermanagement.h"
#include <windows.h>
#include <powrprof.h>

namespace
{

bool adjustPrivilegeForShutdown()
{
    HANDLE hCurrentProc = GetCurrentProcess();
    HANDLE hToken;
    LUID tmpLuid;
    TOKEN_PRIVILEGES tkp;
    LUID_AND_ATTRIBUTES luidAttr;

    if (!OpenProcessToken(hCurrentProc
                     , TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
                     , &hToken))
        return false;

    if (!LookupPrivilegeValueA("", "SeShutdownPrivilege", &tmpLuid))
        return false;

    luidAttr.Luid = tmpLuid;
    luidAttr.Attributes = SE_PRIVILEGE_ENABLED;
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0] = luidAttr;

    if (!AdjustTokenPrivileges(hToken, false, &tkp
                               , sizeof(TOKEN_PRIVILEGES), 0, 0))
        return false;

    return true;
}

bool power_suspend()
{
    bool hal_works = false;

    /*
        BOOLEAN WINAPI SetSuspendState(
          __in  BOOLEAN Hibernate,
          __in  BOOLEAN ForceCritical,
          __in  BOOLEAN DisableWakeEvent
        );
    */
    hal_works = SetSuspendState(false, true, false);

    return hal_works;
}

bool power_shutdown()
{
    bool shutdown_works = false;

    if (!adjustPrivilegeForShutdown())
        return false;

    shutdown_works = ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE | EWX_POWEROFF, 0);

    return shutdown_works;
}

} // anonymous namespace

bool PowerManagement::sendRequest(int action)
{
    switch (action) {
    case SHUTDOWN:
        return power_shutdown();
    case SUSPEND:
        return power_suspend();
    }
    return false;
}

bool PowerManagement::implemented()
{
    return true;
}
