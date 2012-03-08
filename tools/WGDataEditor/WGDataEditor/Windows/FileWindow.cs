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

        public bool CheckIfNodeCanContainNodeOfType(TreeNode Parent, NodeDefinition Type)
        {
            if (XDNodeP.Get(Parent).Definition.AllowedNodeType.Find(n => n.Definition == Type.Name) == null) return false;

            int Count = 0;
            foreach (TreeNode Node in Parent.Nodes)
            {
                if (Node.Tag != null && XDNodeP.Get(Node).Definition != null 
                    && XDNodeP.Get(Node).Definition.Name == Type.Name)
                {
                    Count++;
                }
            }

            if (XDNodeP.Get(Parent).Definition.AllowedNodeType.Find(n => n.Definition == Type.Name).Count < Count)
                return true;
            else return false;
        }

        #region Building Tree
        public void PrepareTreeNode(XmlNode Node, TreeNode TreeNode)
        {
            TreeNode.Text = BuildXmlNodeName(Node, XDNodeP.Get(TreeNode));

            switch (Node.NodeType)
            {
                case XmlNodeType.Comment:
                    TreeNode.ForeColor = System.Drawing.Color.Green;
                    break;
            }
        }

        public string BuildXmlNodeName(XmlNode Node, NodeDefinition Definition)
        {
            if (Node == null) return "";            

            // 3 stepy tworzenia opisu noda
            string Result = "";

            // Nazwa
            switch(Node.NodeType)
            {
                case XmlNodeType.Comment:
                    break;

                default:
                    {
                        if (Definition != null)
                        {
                            Result = Definition.Name;
                        }
                        else
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
                                AttributeDefinition ADef = null;
                                if (Definition != null)
                                {
                                    foreach (var AttributeLink in Definition.AttributeDefinitions)
                                    {
                                        if((ADef = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == AttributeLink.Name)) != null) // Sprawdź, czy nie jest nullem
                                        if (ADef.NameXml == Attribute.Name)
                                        {
                                            AttributesLength = Result.Length;
                                            Result += ADef.Name + " = " + Attribute.Value + ",";
                                            AttributesLengths.Add(Result.Length - AttributesLength);
                                            break;
                                        }
                                        else ADef = null;
                                    }
                                }
                                if (ADef == null)
                                {
                                    AttributesLength = Result.Length;
                                    Result += Attribute.Name + " = " + Attribute.Value + ",";
                                    AttributesLengths.Add(Result.Length - AttributesLength);
                                }
                            }
                            Result = Result.Remove(Result.Length - 1, 1);

                            Result += ")";
                        }
                    }
                    break;
            }
            AttributesLength = Result.Length - NameLength - 3;

            // Wartość
            switch (Node.NodeType)
            {
                case XmlNodeType.Element:
                    if (Node.ChildNodes.Count == 1)
                    {
                        Result += ": " + Node.InnerText;
                    }
                        break;

                default:
                    {
                        Result += ": " + Node.InnerText;
                    }
                    break;
            }

            if (String.IsNullOrEmpty(Node.Value) && Definition != null && Definition.Type != "none")
            {
                if(string.IsNullOrWhiteSpace(Node.InnerText))
                Result += "          ";
            }

            return Result;
        }

        public void AddNodes(XmlNode Node, TreeNode Parent)
        {
            TreeNode NewNode = new TreeNode();
            NewNode.Tag = new XDNodeP(Node);
            PrepareTreeNode(Node, NewNode);
            Parent.Nodes.Add(NewNode);

            foreach (XmlNode ChildNode in Node.ChildNodes)
            {
                if (ChildNode.NodeType == XmlNodeType.Text) continue;
                AddNodes(ChildNode, NewNode);
            }
        }

        public void Reload()
        {
            Doc.Load(FilePath);

            treeView.Nodes.Clear();
            treeView.TopNode = null;

            XmlElement RootNode = Doc["root"];
            TreeNode Root = new TreeNode();
            Root.Tag = new XDNodeP(RootNode);
            PrepareTreeNode(RootNode, Root);
            treeView.Nodes.Add(Root);
            treeView.TopNode = Root;
            

            foreach (XmlNode ChildNodes in RootNode.ChildNodes)
            {
                AddNodes(ChildNodes, Root);
            }
            Root.ExpandAll();
        }
        #endregion

        #region Context Menu

        private void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode.Tag != null && ((XDNodeP)treeView.SelectedNode.Tag).Definition != null)
            {
                XDNodeP Definition = (XDNodeP.Get(treeView.SelectedNode.Tag));
                if(!Definition.Definition.Removable) // Node unremovable
                {
                    MainForm.SetStatusBarText(string.Format("Can't remove unremovable node {0} (chceck node definition).", Definition.Definition.Name));
                    return;
                }
                else
                {
                    Definition.XmlNode.ParentNode.RemoveChild(Definition);// Remove xml node
                    MainForm.SetStatusBarText(string.Format("Removed node {0}", Definition.Definition.Name));
                }
            }
            else
            {
                MainForm.SetStatusBarText(string.Format("Removed unknown node {0}", treeView.SelectedNode.Name));
            }
            treeView.SelectedNode.Remove();
        }

        private void commentToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode.Tag != null)
            {
                TreeNode NewNode = new TreeNode();
                treeView.SelectedNode.Nodes.Add(NewNode);
                if (treeView.SelectedNode.Nodes.Count == 1)
                    treeView.SelectedNode.Expand();

                XmlComment NewComment = Doc.CreateComment("Empty comment");
                NewNode.Tag = new XDNodeP(NewComment);
                PrepareTreeNode(NewComment, NewNode);

                ((XDNodeP)treeView.SelectedNode.Tag).XmlNode.ParentNode.AppendChild(NewComment);

                StartEditing(NewNode, "");
            }
        }

        private void nodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode.Tag != null)
            {
                TreeNode NewNode = new TreeNode("Empty node");
                treeView.SelectedNode.Nodes.Add(NewNode);
                treeView.SelectedNode.Expand();
                StartEditing(NewNode, "", true);
            }
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
                int TotalLength = BuildXmlNodeName(XDNodeP.Get(es.Node), XDNodeP.Get(es.Node)).Length;

                if (es.SelectedIndex >= NameLength)
                {
                    if (es.SelectedIndex < NameLength + AttributesLength + 2) // Edytujemy atrybut {+2 bo ' ('
                    {
                        int SectionPosition = es.SelectedIndex - (NameLength + 2);// Sprawdź który z atrybutów został kliknięty
                        int OldSection = 0, NewSection = 0, i = 0;
                        for (i = 0; i < AttributesLengths.Count; i++)
                        {
                            NewSection += AttributesLengths[i];
                            if (SectionPosition > OldSection && SectionPosition <= NewSection + 1)
                            {
                                // Sprawdź czy kliknięto przez =
                                string Attribute = es.Node.Text.Substring(NameLength + 2 + OldSection, NewSection - OldSection - 1);
                                string AttributeType = Attribute.Substring(0, Attribute.IndexOf('=') - 1);
                                
                                if (SectionPosition < OldSection + AttributeType.Length + 2) // Jeśli tak, edytujemy typ (+ 2 , bo " =")
                                {
                                    StartEditing(es.Node, AttributeType, true);
                                }
                                else
                                {
                                    StartEditing(es.Node, AttributeType);
                                }

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
                else
                {
                    StartEditing(es.Node, "", true);
                }
            }

        }

        #endregion

        #region ValueEditor
        bool EditingType = false;
        TreeNode CurrentlyEditedNode = null;
        string CurrentlyEditedAttribute = "";
        Control CurrentValueEditor = null;

        public void StartEditing(TreeNode Node, string Attribute, bool EditingType = false)
        {
            if (CurrentlyEditedNode == null && CurrentValueEditor == null)
            {
                Point NodePosition = new Point(Node.Bounds.X + Node.Bounds.Width, Node.Bounds.Y);

                XDNodeP XDN = XDNodeP.Get(Node);
                if (Node == null) return;

                CurrentlyEditedNode = Node;
                CurrentlyEditedAttribute = Attribute;

                if (XDN == null) // Just created node
                {
                    EditingType = true;
                    StartEditingType("", NodePosition);
                }
                else
                if (string.IsNullOrEmpty(Attribute)) // węzeł
                {
                    if (EditingType) // Typ
                    {
                        if (!XDN.Definition.Removable) // Skip if node is inremovable
                        {
                            MainForm.SetStatusBarText(string.Format("Can't change type of not removable node ({0})!", XDN.Definition.Name));
                            CancelEditing();                        
                        }
                        else
                        {
                            MainForm.SetStatusBarText(string.Format("Editing type of node {0}", XDN.Definition.Name));
                            StartEditingType(XDN.Definition.Name, NodePosition);
                        }
                    }
                    else
                    {
                        if (XDN.Definition.Type != "none") // Wartość
                        {
                            MainForm.SetStatusBarText(string.Format("Editing value of node {0}", XDN.Definition.Name));

                            StartEditingValue(XDN.XmlNode.InnerText, XDN.Definition.Type, NodePosition);
                        }
                        else
                            CancelEditing();
                    }
                }
                else // atrybut
                {
                    AttributeDefinition ADef = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == Attribute);
                    if (EditingType) // Typ
                    {
                        
                        if (!ADef.Removable)
                        {
                            MainForm.SetStatusBarText(string.Format("Can't change type of not removable attribute - {0} (node: {1})", ADef.Name, XDN.Definition.Name));
                            CancelEditing();
                        }
                        else
                        {
                            MainForm.SetStatusBarText(string.Format("Editing type of attribute {1}, node {2}", XDN.Definition.Name, ADef.Name));

                            StartEditingType(ADef.Name, NodePosition);
                        }
                    }
                    else // Wartość
                    {
                        MainForm.SetStatusBarText(string.Format("Editing value of attribute {1}, node {0}", XDN.Definition.Name, ADef.Name));

                        StartEditingValue(XDN.XmlNode.Attributes[XDN.FindAttributeDefinition(Node, Attribute).NameXml].Value, ADef.Type, NodePosition);
                    }
                }
            }
        }

        public void StartEditingValue(string OldValue, string Type, Point Position)
        {
            EditingType = false;

            string[] Data = Type.Split('~');

            switch (Data[0])
            {
                case "string":
                    {
                        CurrentValueEditor = new TextBox();
                        ((TextBox)CurrentValueEditor).ShortcutsEnabled = true;
                        CurrentValueEditor.Text = OldValue;
                        if (Data.Length >= 3)
                        {
                            CurrentValueEditor.Width = int.Parse(Data[2]);
                        }
                    }
                    break;
                case "number":
                    {
                        NumericUpDown Nud = new NumericUpDown();
                        CurrentValueEditor = Nud;
                        if (Data.Length >= 3)
                        {
                            Nud.DecimalPlaces = int.Parse(Data[2]);
                        }
                        else Nud.DecimalPlaces = 7;

                        if (Data.Length >= 5)
                        {
                            Nud.Minimum = int.Parse(Data[3]);
                            Nud.Maximum = int.Parse(Data[4]);
                        }
                        else
                        {
                            Nud.Minimum = -99999;
                            Nud.Maximum = 99999;
                        }
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
                            Extensions.ComboboxItem<string> NewItem = new Extensions.ComboboxItem<string>(Path.GetFileNameWithoutExtension(fi.FullName), Path.Combine("data", fi.FullName.Replace(MainForm.Instance.Settings.WorkingDirectory + "\\", "")));
                            Combo.Items.Add(NewItem);
                        }

                        foreach (Extensions.ComboboxItem<string> item in Combo.Items)
                        {
                            if (item.Data == OldValue)
                            {
                                Combo.SelectedItem = item;
                            }
                        }
                    }
                    break;
                case "enum":
                    {
                        ComboBox Combo = new ComboBox();
                        Combo.DropDownStyle = ComboBoxStyle.DropDownList;
                        CurrentValueEditor = Combo;

                        if (Data.Length >= 2)
                        {
                            string[] Value = Data[2].Split(';');
                            for (int i = 0; i < Value.Length; i++)
                            {
                                Combo.Items.Add(Value[i]);
                                if (Value[i] == OldValue)
                                    Combo.SelectedItem = Value[i];
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

            CurrentValueEditor.SetBounds(Position.X, Position.Y, CurrentValueEditor.Width, CurrentValueEditor.Height);
            treeView.Controls.Add(CurrentValueEditor);
            CurrentValueEditor.Focus();
        }

        public void StartEditingType(string OldValue, Point Position)
        {
            EditingType = true;

            ComboBox Combo = new ComboBox();

            Combo.DropDownStyle = ComboBoxStyle.DropDownList;
            CurrentValueEditor = Combo;

            XDNodeP XND = XDNodeP.Get(CurrentlyEditedNode);

            if (string.IsNullOrEmpty(CurrentlyEditedAttribute)) // Node type
            {
                if (treeView.TopNode == CurrentlyEditedNode) // Editing top node
                {
                    foreach (NodeDefinition Definition in MainForm.Instance.NodeDefinitions.NodeDefinitions)
                    {
                        if (Definition.IsRoot)
                        {
                            Combo.Items.Add(new Extensions.ComboboxItem<NodeDefinition>(Definition.Name, Definition));
                        }
                    }
                }
                else // Casual node type
                {
                    //try
                    //{
                        TreeNode ParentNode = CurrentlyEditedNode.Parent;
                        if (ParentNode == null) throw new Exception("Parent node is null!");

                        XDNodeP ParentXND = XDNodeP.Get(ParentNode);
                        if (ParentXND == null || ParentXND.Definition == null || ParentXND.XmlNode == null) throw new Exception("Parent node data is not valid!");
                                                
                        // If data is valid
                        foreach (var ExternalDefinition in ParentXND.Definition.AllowedNodeType)
                        {
                            NodeDefinition CandidateDefinition = MainForm.Instance.NodeDefinitions.NodeDefinitions.Find(n => n.Name == ExternalDefinition.Definition);
                            if(CheckIfNodeCanContainNodeOfType(ParentNode, CandidateDefinition))
                            {
                                Combo.Items.Add(new Extensions.ComboboxItem<NodeDefinition>(CandidateDefinition.Name, CandidateDefinition));
                            }

                        }
                    //}
                    //catch(Exception Ex)
                    //{
                    //    MainForm.SetStatusBarText("Can't determine parent node type: " + Ex.Message);
                    //    CancelEditing();
                    //    return;
                    //}

                }
            }
            else // Attribute type
            {
                Extensions.ComboboxItem<AttributeDefinition> CurrentAdded = null;
                foreach (var Definition in XND.Definition.AttributeDefinitions)
                {
                    AttributeDefinition ADef = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == Definition.Name);

                    if (XND.XmlNode.Attributes[ADef.NameXml] == null) // Attribute not present
                    {
                        var NewItem = new Extensions.ComboboxItem<AttributeDefinition>(ADef.Name, ADef);
                        Combo.Items.Add(NewItem);
                        if(ADef.Name == OldValue) CurrentAdded = NewItem;
                    }
                    if(CurrentAdded == null)// Add currently edited attribute
                    {
                        var NewItem = new Extensions.ComboboxItem<AttributeDefinition>(OldValue, MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == OldValue));
                        Combo.Items.Add(NewItem);
                    }

                    Combo.SelectedItem = CurrentAdded;
                }
            }


            CurrentValueEditor.SetBounds(Position.X, Position.Y, CurrentValueEditor.Width, CurrentValueEditor.Height);
            treeView.Controls.Add(CurrentValueEditor);
            CurrentValueEditor.Focus();
        }
        

        public void CancelEditing()
        {
            CurrentlyEditedNode = null;
            CurrentlyEditedAttribute = null;
    

            if (CurrentValueEditor != null)
            {
                if (treeView.Controls.Contains(CurrentValueEditor))
                {
                    treeView.Controls.Remove(CurrentValueEditor);
                }
                CurrentValueEditor = null;
            }
        }

        public string EndGetValue()
        {
            XDNodeP XDN = XDNodeP.Get(CurrentlyEditedNode.Tag);
            if (XDN == null) return CurrentValueEditor.Text.ToString();
            string[] Data;
            if (string.IsNullOrEmpty(CurrentlyEditedAttribute))
            {
                Data = XDN.Definition.Type.Split('~');
            }
            else
            {
                AttributeDefinition ADef = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == CurrentlyEditedAttribute);
                Data = ADef.Type.Split('~'); 
            }

            switch (Data[0])
            {
                case "number":
                    {
                        string Result = CurrentValueEditor.Text.ToString().TrimEnd('0');
                        if (Result.EndsWith(".") || Result.EndsWith(",")) Result += '0';
                        return Result;
                    }
                case "path":
                        return ((Extensions.ComboboxItem<string>)((ComboBox)CurrentValueEditor).SelectedItem).Data;
                default:
                        return CurrentValueEditor.Text.ToString();
            }
        }

        public void AddDefaultAttributes(TreeNode Node, NodeDefinition Definition)
        {
            // Add attributes
            foreach (NodeDefinition.AttributeValuePair AVP in Definition.DefaultAttributeDefinitions)
            {
                AttributeDefinition ADef = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == AVP.Definition.Name);

                XmlAttribute NewAttribute = Doc.CreateAttribute(ADef.NameXml);
                NewAttribute.Value = AVP.Value;
                XDNodeP.Get(Node).XmlNode.Attributes.Append(NewAttribute);
            }

            // Check again if attributes defining type are inserted correctly
            foreach (NodeDefinition.AttributeValuePair AVP in Definition.AttributesDefiningType)
            {
                // Node is already added with default attributes
                if (Definition.DefaultAttributeDefinitions.Find(n => n.Definition.Name == AVP.Definition.Name) != null) continue;


                XmlAttribute NewAttribute = Doc.CreateAttribute(AVP.Definition.Name);
                NewAttribute.Value = AVP.Value;
                XDNodeP.Get(Node).XmlNode.Attributes.Append(NewAttribute);
            }
        }

        public void AddDefaultNodes(TreeNode Parent, NodeDefinition Definition)
        {            
            // Add nodes
            foreach (NodeDefinition.ExternalNode ExternalNode in Definition.DefaultNodes)
            {
                NodeDefinition SubNodeDefinition = MainForm.Instance.NodeDefinitions.NodeDefinitions.Find(n => n.Name == ExternalNode.Definition);

                TreeNode ChildNode = new TreeNode();
                Parent.Nodes.Add(ChildNode);

                XmlNode NewNode = Doc.CreateElement(SubNodeDefinition.NameDefiningType);
                XDNodeP.Get(Parent).XmlNode.AppendChild(NewNode);
                ChildNode.Tag = new XDNodeP(NewNode);

                AddDefaultAttributes(ChildNode, SubNodeDefinition);

                AddDefaultNodes(ChildNode, SubNodeDefinition);

                ChildNode.Text = BuildXmlNodeName(NewNode, SubNodeDefinition);
            }
        }

        public void EndEditing()
        {
            XDNodeP XDN = XDNodeP.Get(CurrentlyEditedNode.Tag);
            string Value = EndGetValue();

            if (string.IsNullOrEmpty(CurrentlyEditedAttribute)) // Edytujemy węzełek
            {
                if (EditingType)
                {
                    if (string.IsNullOrEmpty(Value))
                    {
                        CancelEditing();
                        return;
                    }

                    XmlNode ParentNode = XDNodeP.Get(CurrentlyEditedNode.Parent).XmlNode;
                    if (XDN != null)
                    {
                        // Usuń stary node z XML'a
                        ParentNode = XDN.XmlNode.ParentNode;
                        ParentNode.RemoveChild(XDN.XmlNode);
                    }
                    CurrentlyEditedNode.Nodes.Clear();


                    // Ustwórz xml'a
                    
                    XDN = new XDNodeP();
                    CurrentlyEditedNode.Tag = XDN;

                    NodeDefinition Def = MainForm.Instance.NodeDefinitions.NodeDefinitions.Find(n => n.Name == Value);
                    XDN.Definition = Def;
                    XmlNode NewNode = Doc.CreateElement(Def.NameDefiningType);

                    // Dodaj node do dokumentu xml
                    XDN.XmlNode = NewNode;
                    ParentNode.AppendChild(NewNode);

                    AddDefaultAttributes(CurrentlyEditedNode, Def);

                    AddDefaultNodes(CurrentlyEditedNode, Def);

                    CurrentlyEditedNode.ExpandAll();

                }
                else // Wartość węzełka
                {
                    XDN.XmlNode.InnerText = Value;
                }
            }
            else // Edytujemy atrybut
            {
                if (EditingType)
                {
                    // Znajdź definicję (aby mieć nazwę xml)
                    AttributeDefinition OldAttribute = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == CurrentlyEditedAttribute);
                    AttributeDefinition NewAttribute = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == Value);

                    // Remove old attribute
                    XDN.XmlNode.Attributes.Remove(XDN.XmlNode.Attributes[OldAttribute.NameXml]);

                    // Add new
                    XmlAttribute NewXmlAttribute = Doc.CreateAttribute(NewAttribute.NameXml);
                    XDN.XmlNode.Attributes.Append(NewXmlAttribute);

                    NewXmlAttribute.Value = NewAttribute.DefaultValue;
                }
                else // Edytujemy wartość atrybutu
                {
                    // Znajdź definicję (aby mieć nazwę xml)
                    AttributeDefinition EditedAttribute = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == CurrentlyEditedAttribute);

                    // Ustaw wartość
                    XDN.XmlNode.Attributes[EditedAttribute.NameXml].Value = Value;
                }
            }


            CurrentlyEditedNode.Text = BuildXmlNodeName(XDN.XmlNode, XDN);

            CurrentValueEditor.Hide();
            treeView.Controls.Remove(CurrentValueEditor);
            CurrentValueEditor = null;
            CurrentlyEditedAttribute = null;
            CurrentlyEditedNode = null;
        }

        #endregion

    }
}
