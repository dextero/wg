using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace WGDataEditor
{
    class Extensions
    {

        [Serializable]
        public struct ShellExecuteInfo
        {
            public int Size;
            public uint Mask;
            public IntPtr hwnd;
            public string Verb;
            public string File;
            public string Parameters;
            public string Directory;
            public uint Show;
            public IntPtr InstApp;
            public IntPtr IDList;
            public string Class;
            public IntPtr hkeyClass;
            public uint HotKey;
            public IntPtr Icon;
            public IntPtr Monitor;
        }

        // Code For OpenWithDialog Box
        [DllImport("shell32.dll", SetLastError = true)]
        extern private static bool
               ShellExecuteEx(ref ShellExecuteInfo lpExecInfo);

        public const uint SW_NORMAL = 1;

        public static void OpenAs(string file)
        {
            ShellExecuteInfo sei = new ShellExecuteInfo();
            sei.Size = Marshal.SizeOf(sei);
            sei.Verb = "openas";
            sei.File = file;
            sei.Show = SW_NORMAL;
            if (!ShellExecuteEx(ref sei))
                throw new System.ComponentModel.Win32Exception();
        }

        public static void ShowInFoler(string file)
        {
            System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("Explorer.exe", string.Format("/Select, {0}", file)));
        }

        [DllImport("user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        public static extern void mouse_event(uint dwFlags, uint dx, uint dy, uint cButtons, uint dwExtraInfo);

        private const int MOUSEEVENTF_LEFTDOWN = 0x02;
        private const int MOUSEEVENTF_LEFTUP = 0x04;
        private const int MOUSEEVENTF_RIGHTDOWN = 0x08;
        private const int MOUSEEVENTF_RIGHTUP = 0x10;
        public static void DoMouseClick()
        {
            //Call the imported function with the cursor's current position
            int X = System.Windows.Forms.Cursor.Position.X;
            int Y = System.Windows.Forms.Cursor.Position.Y;
            mouse_event((uint)MOUSEEVENTF_LEFTDOWN | (uint)MOUSEEVENTF_LEFTUP, (uint)X, (uint)Y, 0, 0);
        }

        public class ComboboxStringItem
        {
            public ComboboxStringItem()
            {

            }
            public ComboboxStringItem(string Name, string Data)
            {
                this.Name = Name;
                this.Data = Data;
            }

            public override string ToString()
            {
                return Name;
            }

            public string Name;
            public string Data;
        }

    }
}
