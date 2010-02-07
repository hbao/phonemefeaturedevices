import os
import re
import sys
import time
#import SendKeys
import win32api, win32process, win32security
import win32event, win32con, msvcrt, win32gui
import win32com.client
import win32ui
import win32file

class Process:
    """
    A Windows process.
    """

    def __init__(self, cmd, login=None,
                 hStdin=None, hStdout=None, hStderr=None,
                 show=1, xy=None, xySize=None,
                 desktop=None):
        """
        Create a Windows process.
        cmd:     command to run
        login:   run as user 'Domain\nUser\nPassword'
        hStdin, hStdout, hStderr:
                 handles for process I/O; default is caller's stdin,
                 stdout & stderr
        show:    wShowWindow (0=SW_HIDE, 1=SW_NORMAL, ...)
        xy:      window offset (x, y) of upper left corner in pixels
        xySize:  window size (width, height) in pixels
        desktop: lpDesktop - name of desktop e.g. 'winsta0\\default'
                 None = inherit current desktop
                 '' = create new desktop if necessary

        User calling login requires additional privileges:
          Act as part of the operating system [not needed on Windows XP]
          Increase quotas
          Replace a process level token
        Login string must EITHER be an administrator's account
        (ordinary user can't access current desktop - see Microsoft
        Q165194) OR use desktop='' to run another desktop invisibly
        (may be very slow to startup & finalize).
        """
        si = win32process.STARTUPINFO()
        si.dwFlags = (win32con.STARTF_USESTDHANDLES ^
                      win32con.STARTF_USESHOWWINDOW)
        if hStdin is None:
            si.hStdInput = win32api.GetStdHandle(win32api.STD_INPUT_HANDLE)
        else:
            si.hStdInput = hStdin
        if hStdout is None:
            si.hStdOutput = win32api.GetStdHandle(win32api.STD_OUTPUT_HANDLE)
        else:
            si.hStdOutput = hStdout
        if hStderr is None:
            si.hStdError = win32api.GetStdHandle(win32api.STD_ERROR_HANDLE)
        else:
            si.hStdError = hStderr
        si.wShowWindow = show
        if xy is not None:
            si.dwX, si.dwY = xy
            si.dwFlags ^= win32con.STARTF_USEPOSITION
        if xySize is not None:
            si.dwXSize, si.dwYSize = xySize
            si.dwFlags ^= win32con.STARTF_USESIZE
        if desktop is not None:
            si.lpDesktop = desktop
        procArgs = (None,  # appName
                    cmd,  # commandLine
                    None,  # processAttributes
                    None,  # threadAttributes
                    1,  # bInheritHandles
                    win32process.CREATE_NEW_CONSOLE,  # dwCreationFlags
                    None,  # newEnvironment
                    None,  # currentDirectory
                    si)  # startupinfo
        if login is not None:
            hUser = logonUser(login)
            win32security.ImpersonateLoggedOnUser(hUser)
            procHandles = win32process.CreateProcessAsUser(hUser, *procArgs)
            win32security.RevertToSelf()
        else:
            procHandles = win32process.CreateProcess(*procArgs)
        self.hProcess, self.hThread, self.PId, self.TId = procHandles

    def postMessage(self,msg,wparam,lparam):
        hwnd = win32ui.FindWindow(None,"S60 3rd Edition SDK, for C++  [352x416] - winscw udeb")
        if hwnd != 0:
            hwnd.PostMessage(msg,wparam,lparam)
        else:
            print "window not found"

    def sendMessage(self,msg,wparam,lparam):
        hwnd = win32gui.FindWindow(0,"S60 3rd Edition SDK, for C++  [352x416] - winscw udeb")
        win32api.SendMessage(hwnd,msg,wparam,lparam)

    def processID(self):
        return self.PId
    def wait(self, mSec=None):
        """
        Wait for process to finish or for specified number of
        milliseconds to elapse.
        """
        if mSec is None:
            mSec = win32event.INFINITE
        return win32event.WaitForSingleObject(self.hProcess, mSec)

    def kill(self, gracePeriod=5000):
        """
        Kill process. Try for an orderly shutdown via WM_CLOSE.  If
        still running after gracePeriod (5 sec. default), terminate.
        """
        win32gui.EnumWindows(self.__close__, 0)
        if self.wait(gracePeriod) != win32event.WAIT_OBJECT_0:
            win32process.TerminateProcess(self.hProcess, 0)
            win32api.Sleep(100) # wait for resources to be released

    def __close__(self, hwnd, dummy):
        """
        EnumWindows callback - sends WM_CLOSE to any window
        owned by this process.
        """
        TId, PId = win32process.GetWindowThreadProcessId(hwnd)
        if PId == self.PId:
            win32gui.PostMessage(hwnd, win32con.WM_CLOSE, 0, 0)

    def exitCode(self):
        """
        Return process exit code.
        """
        return win32process.GetExitCodeProcess(self.hProcess)

def run(cmd, mSec=None, stdin=None, stdout=None, stderr=None, **kw):
    """
    Run cmd as a child process and return exit code.
    mSec:  terminate cmd after specified number of milliseconds
    stdin, stdout, stderr:
           file objects for child I/O (use hStdin etc. to attach
           handles instead of files); default is caller's stdin,
           stdout & stderr;
    kw:    see Process.__init__ for more keyword options
    """
    if stdin is not None:
        kw['hStdin'] = msvcrt.get_osfhandle(stdin.fileno())
    if stdout is not None:
        kw['hStdout'] = msvcrt.get_osfhandle(stdout.fileno())
    if stderr is not None:
        kw['hStderr'] = msvcrt.get_osfhandle(stderr.fileno())
    child = Process(cmd, **kw)
    return child

def waitForExit(child, timeout = 1000):
	if child.wait(timeout) != win32event.WAIT_OBJECT_0:
		child.kill()
	return child.exitCode()