using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WGDataEditor
{
    public partial class OptionsWindow : Form
    {
        public OptionsWindow(object obj, string Title)
        {
            InitializeComponent();
            propertyGrid1.SelectedObject = obj;
            Text = Title;
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
