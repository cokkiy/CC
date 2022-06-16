using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace AppLuncher
{
    public static class HideMe
    {
        [DllImport("kernel32.dll")]
        public static extern IntPtr GetConsoleWindow();
        [DllImport("user32.dll")]
        public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
        public const int SW_SHOW = 5;
        public const int SW_HIDE = 0;
        public const int SW_MINIMIZE = 2;
        public const int SW_SHOWMAXIMIZED= 3;
    }
}
