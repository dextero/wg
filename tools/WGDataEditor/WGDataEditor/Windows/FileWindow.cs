using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;

namespace WGDataEditor
{
    public partial class FileWindow : WeifenLuo.WinFormsUI.Docking.DockContent
    {
        public string FilePath { get; set; }
        public XmlDocument Doc = new XmlDocument();

        int NameLength;
        int AttributesLength = 0;
        List<int> AttributesLengths = new List<int>();

        public FileWindow(string _FilePath)
        {
            InitializeComponent();

            FilePath = _FilePath;
            Reload();
        }

        #region Building Tree
        public void PrepareTreeNode(XmlNode Node, TreeNode TreeNode)
        {
            TreeNode.Text = BuildXmlNodeName(Node);

            switch (Node.NodeType)
            {
                case XmlNodeType.Comment:
                    TreeNode.ForeColor = System.Drawing.Color.Green;
                    break;
            }
        }

        public string BuildXmlNodeName(XmlNode Node)
        {
            // 3 stepy tworzenia opisu noda
            string Result = "";

            // Nazwa
            switch(Node.NodeType)
            {
                case XmlNodeType.Comment:
                    break;

                default:
                    {
                        Result = Node.Name;
                    }
                    break;
            }

            NameLength = Result.Length;
            AttributesLengths.Clear();
            // Atrybuty
            switch (Node.NodeType)
            {
                default:
                    {
                        if (Node.Attributes != null && Node.Attributes.Count > 0)
                        {
                            Result += " (";
                            foreach (XmlAttribute Attribute in Node.Attributes)
                            {
                                AttributesLength = Result.Length;
                                Result += Attribute.Name + " = " + Attribute.Value + ",";
                                AttributesLengths.Add(Result.Length - AttributesLength);
                            }
                            Result = Result.Remove(Result.Length - 1, 1);
                            Result += ")";
                        }
                    }
                    break;
            }
            AttributesLength = Result.Length - NameLength;

            // Wartość
            switch (Node.NodeType)
            {
                case XmlNodeType.Element:
                    if (Node.ChildNodes.Count == 1)
                    {
                        Result += " " + Node.InnerText;
                    }
                        break;

                default:
                    {
                        Result += " " + Node.InnerText;
                    }
                    break;
            }

            return Result;
        }

        public void AddNodes(XmlNode Node, TreeNode Parent)
        {
            TreeNode NewNode = new TreeNode();
            PrepareTreeNode(Node, NewNode);
            Parent.Nodes.Add(NewNode);
            NewNode.Tag = new XDNodeP(null, Node);
            ((XDNodeP)NewNode.Tag).Definition = ((XDNodeP)NewNode.Tag).FindNodeDefinition(NewNode);// Aaaarghhh

            foreach (XmlNode ChildNode in Node.ChildNodes)
            {
                if (ChildNode.NodeType == XmlNodeType.Text) continue;
                AddNodes(ChildNode, NewNode);
            }
        }

        #endregion

        public void Reload()
        {
            Doc.Load(FilePath);

            treeView.Nodes.Clear();

            XmlElement RootNode = Doc["root"];
            TreeNode Root = new TreeNode();
            PrepareTreeNode(RootNode, Root);
            treeView.Nodes.Add(Root);
            treeView.TopNode = Root;
            

            foreach (XmlNode ChildNodes in RootNode.ChildNodes)
            {
                AddNodes(ChildNodes, Root);
            }
            Root.ExpandAll();
        }

        #region Context Menu

        private void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            XmlNode ElementToRemove = ((XDNodeP)treeView.SelectedNode.Tag).XmlNode;
            ElementToRemove.ParentNode.RemoveChild(ElementToRemove);
            treeView.SelectedNode.Remove();
        }

        private void commentToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            TreeNode NewNode = new TreeNode();
            treeView.SelectedNode.Nodes.Add(NewNode);
            if (treeView.SelectedNode.Nodes.Count == 1)
                treeView.SelectedNode.Expand();

            XmlComment NewComment = Doc.CreateComment("Empty comment");
            PrepareTreeNode(NewComment, NewNode);
            NewNode.Tag = new XDNodeP(null, NewComment);
            ((XDNodeP)NewNode.Tag).Definition = ((XDNodeP)NewNode.Tag).FindNodeDefinition(NewNode);// Aaaarghhh

            ((XDNodeP)treeView.SelectedNode.Tag).XmlNode.ParentNode.AppendChild(NewComment);
        }

        private void treeView_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                treeView.SelectedNode = treeView.GetNodeAt(e.X, e.Y);
                if (treeView.SelectedNode != null)
                {
                    itemContextMenuStrip.Show(treeView, e.Location);
                }
            }

            if (CurrentValueEditor != null)
            {
                EndEditing();
            }
        }

        private void treeView_EditingClicked(object sender, EditableTreeView.EditEventArgs es)
        {
            if (es.SelectedIndex >= 0 && es.Node != null)
            {
                int TotalLength = BuildXmlNodeName(((XDNodeP)es.Node.Tag).XmlNode).Length;

                if (es.SelectedIndex >= NameLength)
                {
                    if (es.SelectedIndex < NameLength + AttributesLength) // Edytujemy atrybut
                    {
                        int SectionPosition = es.SelectedIndex - NameLength;// Sprawdź który z atrybutów został kliknięty
                        int OldSection = 0, NewSection = 0, i = 0;
                        for (i = 0; i < AttributesLengths.Count; i++)
                        {
                            NewSection = AttributesLengths[i];
                            if (SectionPosition > OldSection && SectionPosition < NewSection)
                            {
                                break;
                            }
                            OldSection = NewSection;
                        }


                    }
                    else // Edytujemy wartość
                    {
                        StartEditing(es.Node, "");
                    }
                }
            }

        }

        #endregion

        #region ValueEditor
        TreeNode CurentlyEditedNode = null;
        string CurrentlyEditedAttribute = "";
        Control CurrentValueEditor = null;

        public void StartEditing(TreeNode Node, string Attribute)
        {
            if (CurentlyEditedNode == null && CurrentValueEditor == null)
            {
                CurentlyEditedNode = Node;
                CurrentlyEditedAttribute = Attribute;
                XDNodeP XDN = (XDNodeP)Node.Tag;


                if (string.IsNullOrEmpty(Attribute)) // Edytujemy wartość węzła
                {
                    if (XDN.Definition.Type != "none")
                    {
                        CreateEditorWindow(XDN.XmlNode.InnerText, XDN.Definition.Type, new Point(Node.Bounds.X + Node.Bounds.Width, Node.Bounds.Y));
                    }
                }
                else // Edytujemy atrybut
                {

                }
            }
        }

        public void CreateEditorWindow(string OldValue, string Type, Point Position)
        {
            string[] Data = Type.Split('~');

            switch (Data[0])
            {
                case "string":
                    {
                        CurrentValueEditor = new TextBox();
                        CurrentValueEditor.Text = OldValue;
                    }
                    break;
                case "int":
                    {
                        NumericUpDown Nud = new NumericUpDown();
                        CurrentValueEditor = Nud;
                        Nud.Minimum = -99999;
                        Nud.Maximum = 99999;
                        Nud.DecimalPlaces = 0;
                        Nud.Value = (Decimal)System.Convert.ChangeType(OldValue, typeof(Decimal));
                    }
                    break;

                case "float":
                    {
                        NumericUpDown Nud = new NumericUpDown();
                        CurrentValueEditor = Nud;
                        Nud.Minimum = -99999;
                        Nud.Maximum = 99999;
                        Nud.DecimalPlaces = 7;
                        Nud.Value = (Decimal)System.Convert.ChangeType(OldValue, typeof(Decimal));

                    }
                    break;
                case "path":
                    {
                        ComboBox Combo = new ComboBox();
                        Combo.DropDownStyle = ComboBoxStyle.DropDownList;
                        CurrentValueEditor = Combo;

                        string dirPath = Path.Combine(MainForm.Instance.Settings.WorkingDirectory, Path.GetDirectoryName(Data[2]));
                        string filter =  Path.GetFileName(Data[2]);

                        foreach (FileInfo fi in new DirectoryInfo(dirPath).GetFiles(filter))
                        {
                            Extensions.ComboboxStringItem NewItem = new Extensions.ComboboxStringItem(Path.GetFileNameWithoutExtension(fi.FullName), Path.Combine("data", fi.FullName.Replace(MainForm.Instance.Settings.WorkingDirectory + "\\", "")));
                            Combo.Items.Add(NewItem);
                        }

                        foreach (Extensions.ComboboxStringItem item in Combo.Items)
                        {
                            if (item.Data == OldValue)
                            {
                                Combo.SelectedItem = item;
                            }
                        }
                    }
                    break;

            
            }

            if (CurrentValueEditor is ComboBox)
            {
                if ((CurrentValueEditor as ComboBox).SelectedIndex == -1 && (CurrentValueEditor as ComboBox).Items.Count >= 1)
                {
                    (CurrentValueEditor as ComboBox).SelectedIndex = 0;
                }
            }

            CurrentValueEditor.SetBounds(Position.X, Position.Y, 150, Bounds.Height);
            treeView.Controls.Add(CurrentValueEditor);
        }

        public void EndEditing()
        {

            XDNodeP XDN = (XDNodeP)CurentlyEditedNode.Tag;

            string[] Data = XDN.Definition.Type.Split('~');
            switch (Data[0])
            {
                case "path":
                    {
                        XDN.XmlNode.InnerText = ((Extensions.ComboboxStringItem)((ComboBox)CurrentValueEditor).SelectedItem).Data;
                    }
                    break;
                default:
                    {
                        XDN.XmlNode.InnerText = CurrentValueEditor.Text.ToString();
                    }
                    break;

            }
            CurentlyEditedNode.Text = BuildXmlNodeName(XDN.XmlNode);

            CurrentValueEditor.Hide();
            treeView.Controls.Remove(CurrentValueEditor);
            CurrentValueEditor = null;
            CurrentlyEditedAttribute = null;
            CurentlyEditedNode = null;
        }

        #endregion
    }
}
