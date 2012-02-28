using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace WGDataEditor
{
    public partial class DirectoriesWindow : WeifenLuo.WinFormsUI.Docking.DockContent
    {
        public DirectoriesWindow()
        {
            InitializeComponent();
        }

        private void DirectoriesWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!MainForm.Instance.EditorClosing)
            {
                e.Cancel = true;
                MainForm.Instance.ChangeVisiblity(this);
            }
        }

        #region TREE

        private void AddFolderNode(string DirectoryPath, TreeNode Parent)
        {
            TreeNode MyNode = new TreeNode(Path.GetFileName(DirectoryPath));
            MyNode.Tag = DirectoryPath;
            MyNode.ImageIndex = MyNode.SelectedImageIndex = 0;
            Parent.Nodes.Add(MyNode);

            DirectoryInfo Di = new DirectoryInfo(DirectoryPath);
            foreach (var XmlFile in Di.GetFiles("*.xml"))
            {
                TreeNode FileNode = new TreeNode(Path.GetFileNameWithoutExtension(XmlFile.Name));
                FileNode.Tag = XmlFile.FullName;
                FileNode.ImageIndex = FileNode.SelectedImageIndex = 1;
                MyNode.Nodes.Add(FileNode);
            }
                        
            foreach (var ChildDi in Di.GetDirectories())
            {
                AddFolderNode(ChildDi.FullName, MyNode);
            }
        }


        #endregion

        public void Reload()
        {
            if (Directory.Exists(MainForm.Instance.Settings.WorkingDirectory))
            {
                TreeNode RootNode = new TreeNode(Path.GetFileName(MainForm.Instance.Settings.WorkingDirectory));
                RootNode.ImageIndex = RootNode.SelectedImageIndex = 0;
                RootNode.Tag = MainForm.Instance.Settings.WorkingDirectory;
                treeView.Nodes.Add(RootNode);
                treeView.TopNode = RootNode;
                
                foreach (var ChildDi in new DirectoryInfo(MainForm.Instance.Settings.WorkingDirectory).GetDirectories())
                {
                    AddFolderNode(ChildDi.FullName, RootNode);
                }

                RootNode.Expand();
            }
            else throw new Exception("Set WorkingDirectory in options window first!");
 
        }

        private void DirectoriesWindow_Activated(object sender, EventArgs e)
        {
            Reload();
        }

        private void treeView_DoubleClick(object sender, EventArgs e)
        {
            MouseEventArgs me = e as MouseEventArgs;

            treeView.SelectedNode = treeView.GetNodeAt(me.X, me.Y);

            if (treeView.SelectedNode != null)
            {
                if (treeView.SelectedNode.Nodes.Count == 0)
                {
                    editToolStripMenuItem_Click(null, e);
                }
            }
        }

        private void treeView_MouseUp(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Right)
            {
                // Select the clicked node
                treeView.SelectedNode = treeView.GetNodeAt(e.X, e.Y);

                if (treeView.SelectedNode != null && treeView.SelectedNode.Nodes.Count == 0 && (((string)treeView.SelectedNode.Tag).ToLower().EndsWith(".xml")))
                {
                    itemContextMenuStrip.Show(treeView, e.Location);
                }
            }
        }

        private void openWithToolStripMenuItem_Click(object sender, EventArgs e)
        {            
              Extensions.OpenAs((string)treeView.SelectedNode.Tag);
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start((string)treeView.SelectedNode.Tag);
        }

        private void showInFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string Path = (string)treeView.SelectedNode.Tag;
            if (File.Exists(Path))
                Extensions.ShowInFoler(Path);

        }

        private void editToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MainForm.Instance.LoadScenario((string)treeView.SelectedNode.Tag);
        }
    }
}
