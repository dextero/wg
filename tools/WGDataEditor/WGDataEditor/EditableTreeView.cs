using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WGDataEditor
{
    public partial class EditableTreeView : TreeView
    {

        public EditableTreeView()
        {
            InitializeComponent();

            MainTextBox = new TextBox();
            MainTextBox.ReadOnly = true;
            MainTextBox.Parent = this;
            MainTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            MainTextBox.ForeColor = Color.White;
            MainTextBox.BackColor = System.Drawing.SystemColors.Highlight;
            MainTextBox.Click += new EventHandler(MainTextBox_Click);
            MainTextBox.Hide();
        }

        void MainTextBox_Click(object sender, EventArgs e)
        {
            MouseEventArgs em = e as MouseEventArgs;

            if (_EditingClicked != null)
            _EditingClicked(this, new EditEventArgs(CurrentlyEditingNode, MainTextBox.GetCharIndexFromPosition(new Point(em.X, em.Y))));
        }

        #region Evnets
        public class EditEventArgs : EventArgs
        {
            public EditEventArgs(TreeNode _Node, int _SelectedIndex)
            {
                Node = _Node;
                SelectedIndex = _SelectedIndex;
            }

            public TreeNode Node;
            public int SelectedIndex;
        }

        public delegate void EditEventHandler(object sender, EditEventArgs es);

        event EditEventHandler _EditingStarted;
        event EditEventHandler _EditingFinished;
        event EditEventHandler _EditingClicked;

        public event EditEventHandler EditingStarted
        {
            add
            {
                _EditingStarted += value;
            }
            remove
            {
                _EditingStarted -= value;
            }
        }
        public event EditEventHandler EditingFinished
        {
            add
            {
                _EditingFinished += value;
            }
            remove
            {
                _EditingFinished -= value;
            }
        }
        public event EditEventHandler EditingClicked
        {
            add
            {
                _EditingClicked += value;
            }
            remove
            {
                _EditingClicked -= value;
            }
        }

        #endregion
        private TextBox MainTextBox;
        private bool IsEditing = false;
        private TreeNode CurrentlyEditingNode = null;

        public void EditStart(TreeNode Node, MouseEventArgs e)
        {
            IsEditing = true;
            CurrentlyEditingNode = Node;

            MainTextBox.SetBounds(Node.Bounds.X + 2, Node.Bounds.Y + 1, Node.Bounds.Width, Node.Bounds.Height);
            MainTextBox.Font = Font;
            MainTextBox.Show();
            MainTextBox.SelectAll();
            MainTextBox.Text = Node.Text;
            MainTextBox.Tag = Node;

            if(_EditingStarted != null)
            _EditingStarted(this, new EditEventArgs(CurrentlyEditingNode, -1));

            if(MainTextBox.Visible)
                Extensions.DoMouseClick();
        }

        public void EditEnd()
        {
            IsEditing = false;
            MainTextBox.Hide();
            if (_EditingFinished != null)
            _EditingFinished(this, new EditEventArgs(CurrentlyEditingNode, -1));
            CurrentlyEditingNode = null;
        }

        private void EditableTreeView_MouseUp(object sender, MouseEventArgs e)
        {
            if (IsEditing)
            {
                EditEnd();
            }

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                // Select the clicked node
                if(SelectedNode == GetNodeAt(e.X, e.Y))
                if (SelectedNode != null)
                {
                    if (!IsEditing)
                    {
                        EditStart(SelectedNode, e);
                        SelectedNode = null;
                    }
                    else
                    {
                        EditEnd();
                    }
                }
            }
        }
    }
}
