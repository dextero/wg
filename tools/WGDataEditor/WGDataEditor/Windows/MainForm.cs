using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Xml;
using System.Reflection;
using System.Text;
using System.Windows.Forms;


namespace WGDataEditor
{
    public partial class MainForm : Form
    {
        public static MainForm Instance;

        public MainForm()
        {
            Instance = this;
            InitializeComponent();

            Settings = new Settings();
            Settings.Load();
            RefreshRecentItems();

            NodeDefinitions = DefinitionContainer.Load();
            if (NodeDefinitions == null)
                NodeDefinitions = new DefinitionContainer();

            SetupWindows();
        }

        public void SetupWindows()
        {
            DirectoriesWindow = new DirectoriesWindow();
            WindowStripDictionary.Add(DirectoriesWindow, directoriesToolStripMenuItem);
            DefinitionsWindow = new DefinitionsWindow();
            WindowStripDictionary.Add(DefinitionsWindow, nodeDefinitionsToolStripMenuItem);

            DirectoriesWindow.Show(dockPanel);
        }

        public Settings Settings;
        public DefinitionContainer NodeDefinitions;

        public bool EditorClosing = false;
        public string OpenedDocument = "";

        private Dictionary<WeifenLuo.WinFormsUI.Docking.DockContent, ToolStripMenuItem> WindowStripDictionary = new Dictionary<WeifenLuo.WinFormsUI.Docking.DockContent, ToolStripMenuItem>();
        DirectoriesWindow DirectoriesWindow;
        DefinitionsWindow DefinitionsWindow;

        public void LoadScenario(string Path)
        {
            OpenedDocument = Path;

            // Load documents
            if (File.Exists(Path))
            {
                FileWindow NewWindow = new FileWindow(Path);
                NewWindow.Show(dockPanel);
            }
        }

        public void SaveScenario(string Path)
        {

        }

        public void RefreshRecentItems()
        {
            if (Settings.RecentFiles.Count == 5)
            {
                item1ToolStripMenuItem.Text = Settings.RecentFiles[4];
            }
            item1ToolStripMenuItem.Visible = (Settings.RecentFiles.Count == 5);

            if (Settings.RecentFiles.Count == 4)
            {
                item2ToolStripMenuItem.Text = Settings.RecentFiles[3];
            }
            item2ToolStripMenuItem.Visible = (Settings.RecentFiles.Count == 4);

            if (Settings.RecentFiles.Count == 3)
            {
                item3ToolStripMenuItem.Text = Settings.RecentFiles[2];
            }
            item3ToolStripMenuItem.Visible = (Settings.RecentFiles.Count == 3);

            if (Settings.RecentFiles.Count == 2)
            {
                item4ToolStripMenuItem.Text = Settings.RecentFiles[1];
            }
            item4ToolStripMenuItem.Visible = (Settings.RecentFiles.Count == 2);

            if (Settings.RecentFiles.Count == 1)
            {
                item5ToolStripMenuItem.Text = Settings.RecentFiles[0];
            }
            item5ToolStripMenuItem.Visible = (Settings.RecentFiles.Count == 1);

        }

        public static void SetStatusBarText(string Message)
        {
            MainForm.Instance.toolStripStatusLabelText.Text = Message;
        }


        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openDirecory.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                LoadScenario(openDirecory.SelectedPath);

                Settings.RecentFiles.Add(OpenedDocument);
                while (Settings.RecentFiles.Count > 5)
                {
                    Settings.RecentFiles.RemoveAt(0);
                }
                RefreshRecentItems();

            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!Directory.Exists(OpenedDocument))
            {
                saveAsToolStripMenuItem_Click(null, null);
            }

            SaveScenario(OpenedDocument);
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openDirecory.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                OpenedDocument = openDirecory.SelectedPath;
                saveToolStripMenuItem_Click(null, null);
            }
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetupWindows();
        }
        
        public void ChangeVisiblity(WeifenLuo.WinFormsUI.Docking.DockContent Window)
        {
            if (!WindowStripDictionary[Window].Checked)
            {
                if (dockPanel.Contains(Window))
                    Window.Show();
                else 
                    Window.Show(dockPanel);

                WindowStripDictionary[Window].Checked = true;
            }
            else
            {
                Window.Hide();
                WindowStripDictionary[Window].Checked = false;
            }
        }

        public static XmlElement AddElem(string Name, string Value, XmlElement Parent)
        {
            XmlElement NewElem = Parent.OwnerDocument.CreateElement(Name);
            NewElem.InnerText = Value;
            Parent.AppendChild(NewElem);
            return NewElem;
        }

        private void showInFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(OpenedDocument))
                Extensions.ShowInFoler(OpenedDocument);
            
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            EditorClosing = true;
            Settings.Save();
        }

        private void recentItemToolStripMenuItem_Clicked(object sender, EventArgs e)
        {
            LoadScenario(((ToolStripMenuItem)sender).Text);
            Settings.RecentFiles.Remove(((ToolStripMenuItem)sender).Text);
            Settings.RecentFiles.Add(((ToolStripMenuItem)sender).Text);
        }

        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OptionsWindow OptionsWindow = new OptionsWindow(Settings, "Settings");
            OptionsWindow.ShowDialog();
        }

        private void directoriesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChangeVisiblity(DirectoriesWindow);
        }

        private void nodeDefinitionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChangeVisiblity(DefinitionsWindow);
        }
    }
}
