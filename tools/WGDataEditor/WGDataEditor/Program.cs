using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Threading;

namespace WGDataEditor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // Creating a Global culture specific to our application.
            System.Globalization.CultureInfo cultureInfo =
                new System.Globalization.CultureInfo("en-US");
            // Creating the DateTime Information specific to our application.
            System.Globalization.DateTimeFormatInfo dateTimeInfo =
                new System.Globalization.DateTimeFormatInfo();
            // Defining various date and time formats.
            dateTimeInfo.DateSeparator = "/";
            dateTimeInfo.LongDatePattern = "dd-MMM-yyyy";
            dateTimeInfo.ShortDatePattern = "dd-MMM-yy";
            dateTimeInfo.LongTimePattern = "hh:mm:ss tt";
            dateTimeInfo.ShortTimePattern = "hh:mm tt";
            // Setting application wide date time format.
            cultureInfo.DateTimeFormat = dateTimeInfo;
            // Assigning our custom Culture to the application.
            Application.CurrentCulture = cultureInfo;
            Thread.CurrentThread.CurrentCulture = cultureInfo;
            Thread.CurrentThread.CurrentUICulture = cultureInfo;



            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
    }
}
