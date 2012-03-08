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
    public partial class DefinitionsWindow : WeifenLuo.WinFormsUI.Docking.DockContent
    {
        List<NodeDefinition> NDefinitions { get; set; }
        List<AttributeDefinition> ADefinitions { get; set; }
        public DefinitionsWindow()
        {
            InitializeComponent();
            NDefinitions = MainForm.Instance.NodeDefinitions.NodeDefinitions;
            ADefinitions = MainForm.Instance.NodeDefinitions.AttributeDefinitions;
        }

        public bool EditingAttributes = false;

        public void Reload()
        {
            listBox.Items.Clear();
            if (!EditingAttributes)
            {
                foreach (var Definition in NDefinitions)
                {
                    listBox.Items.Add(Definition.Name);
                }
            }
            else
            {
                foreach (var Definition in ADefinitions)
                {
                    listBox.Items.Add(Definition.Name);
                }
            }
        }

        private void reloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Reload();
        }

        private void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!EditingAttributes)
            {
                if (listBox.SelectedIndex != -1)
                {
                    NDefinitions.Remove(NDefinitions.Find(n => n.Name == listBox.Text));
                }
            }
            else
            {
                if (listBox.SelectedIndex != -1)
                {
                    ADefinitions.Remove(ADefinitions.Find(n => n.Name == listBox.Text));
                }
            }
        }

        private void editToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!EditingAttributes)
            {
                if (listBox.SelectedIndex != -1)
                {
                    propertyGrid.SelectedObject = NDefinitions.Find(n => n.Name == listBox.Text);
                }
            }
            else
            {
                if (listBox.SelectedIndex != -1)
                {
                    propertyGrid.SelectedObject = ADefinitions.Find(n => n.Name == listBox.Text);
                }
            }
        }

        private void addToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!EditingAttributes)
            {
                NodeDefinition NewDefinition = new NodeDefinition();
                NewDefinition.Name = "New_Definition";
                NDefinitions.Add(NewDefinition);
                Reload();
            }
            else
            {
                AttributeDefinition NewDefinition = new AttributeDefinition();
                NewDefinition.Name = "New_Definition";
                ADefinitions.Add(NewDefinition);
                Reload();
            }
        }

        private void listBox_DoubleClick(object sender, EventArgs e)
        {
            editToolStripMenuItem_Click(null, null);
        }

        private void DefinitionsWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            MainForm.Instance.NodeDefinitions.Save();

            if (!MainForm.Instance.EditorClosing)
            {
                e.Cancel = true;
                MainForm.Instance.ChangeVisiblity(this);
            }
        }

        private void editAttributesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            EditingAttributes = !EditingAttributes;

            if (EditingAttributes)
            {
                editAttributesToolStripMenuItem.Text = "Edit nodes";
            }
            else
            {
                editAttributesToolStripMenuItem.Text = "Edit attributes";
            }

            propertyGrid.SelectedObject = null;
            Reload();
        }

        private void DefinitionsWindow_Leave(object sender, EventArgs e)
        {
            MainForm.Instance.NodeDefinitions.Save();
        }

        private void DefinitionsWindow_Activated(object sender, EventArgs e)
        {
            Reload();
        }

        private void editTypesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            new OptionsWindow(MainForm.Instance.NodeDefinitions, "Node types").Show();
        }
    }
}
