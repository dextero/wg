using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Xml;
// Ten plik to potwór, jeśli przeglądasz - ostrzegam, na własne ryzyko

namespace WGDataEditor
{
    public class NodeDefinitionContainer
    {
        public NodeDefinitionContainer()
        {
            NodeDefinitions = new List<NodeDefinition>();
            AttributeDefinitions = new List<AttributeDefinition>();
        }

        public List<NodeDefinition> NodeDefinitions { get; set; }
        public List<AttributeDefinition> AttributeDefinitions { get; set; }




        public static NodeDefinitionContainer Load()
        {
            string FilePath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), Path.Combine(MainForm.Instance.Settings.DataDirectory, "NodeDefinitions.xml"));
            if (File.Exists(FilePath))
            {
                using (StreamReader Reader = new StreamReader(FilePath))
                {
                    System.Xml.Serialization.XmlSerializer Serializer = new System.Xml.Serialization.XmlSerializer(typeof(NodeDefinitionContainer));
                    return (NodeDefinitionContainer)Serializer.Deserialize(Reader);
                }
            }
            else MessageBox.Show("Node definitions file not found!", "Warning!");
            return null;
        }

        public void Save()
        {
            string FilePath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), Path.Combine(MainForm.Instance.Settings.DataDirectory, "NodeDefinitions.xml"));
            using (StreamWriter Writer = new StreamWriter(FilePath))
            {

                System.Xml.Serialization.XmlSerializerNamespaces ns = new System.Xml.Serialization.XmlSerializerNamespaces();
                ns.Add("", "");

                System.Xml.Serialization.XmlSerializer Serializer = new System.Xml.Serialization.XmlSerializer(typeof(NodeDefinitionContainer));
                Serializer.Serialize(Writer, this, ns);
            }
        }


    }

    public class XDNodeP
    {
        public static XDNodeP Get(TreeNode obj)
        {
            if (obj != null)
            {
                return (XDNodeP)obj.Tag;
            }
            else return null;
        }
        public static XDNodeP Get(Object obj)
        {
            if (obj != null)
            {
                return (XDNodeP)obj;
            }
            else return null;
        }

        public XDNodeP()
        {

        }

        public static implicit operator XmlNode(XDNodeP Node)
        {
            if (Node != null)
            {
                return Node.XmlNode;
            }
            else return null;
        }

        public static implicit operator NodeDefinition(XDNodeP Node)
        {
            if (Node != null)
            {
                return Node.Definition;
            }
            else return null;
        }

        public XDNodeP(NodeDefinition NewDefinition, System.Xml.XmlNode NewXmlNode)
        {
            Definition = NewDefinition;
            XmlNode = NewXmlNode;
        }
        public NodeDefinition Definition;
        public System.Xml.XmlNode XmlNode;
        
        public NodeDefinition FindNodeDefinition(TreeNode Node)
        {
            XmlNode XmlNode = ((XDNodeP)Node.Tag).XmlNode;

            foreach (var Definition in MainForm.Instance.NodeDefinitions.NodeDefinitions)
            {
                if (Definition.NameDefiningType == XmlNode.Name)
                {
                    foreach (var Attribute in Definition.AttributesDefiningType)
                    {
                        foreach (XmlAttribute XmlAttribute in XmlNode.Attributes)
                        {
                            if (Attribute.Definition.NameXml == XmlAttribute.Name && Attribute.Value == XmlAttribute.Value)
                            {
                                return Definition;
                            }
                        }
                    }
                    if (Definition.AttributesDefiningType.Count == 0) return Definition;
                }
            }

            return null;
        }

        public AttributeDefinition FindAttributeDefinition(TreeNode Node, String Name)
        {
            NodeDefinition def = FindNodeDefinition(Node);
            if (def == null) return null;
            else
                return FindAttributeDefinition(Node, Name, def);
        }

        public AttributeDefinition FindAttributeDefinition(TreeNode Node, String Name, NodeDefinition NodeDefinition)
        {
            foreach (var Attribute in MainForm.Instance.NodeDefinitions.AttributeDefinitions)
            {
                if (Attribute.Name == Name)
                {
                    return Attribute;
                }
            }
            return null;
        }


    }

    public class NodeDefinition
    {       
        public class NodeDefinitionCountPair
        {
            public NodeDefinitionCountPair()
            {
                Count = 1;
            }

            public NodeDefinitionCountPair(string Definition, int Count)
            {
                this.Definition = Definition;
                this.Count = Count;
            }

            [TypeConverter(typeof(NodeDefinitonConverter))]
            public string Definition { get; set; }
            public int Count { get; set; }
        }
        public class ExternalNode
        {
            [TypeConverter(typeof(NodeDefinitonConverter))]
            public string Definition { get; set; }
        }
        public class AttributeValuePair
        {
            public AttributeValuePair()
            {
                Definition = null;
                Value = "";
            }

            public AttributeValuePair(AttributeDefinition Definition, string Value)
            {
                this.Definition = Definition;
                this.Value = Value;
            }

            [TypeConverter(typeof(AttributeDefinitonConverter))]
            public AttributeDefinition Definition { get; set; }
            public string Value { get; set; }
        }

        public class ExternalAttribute
        {
            [TypeConverter(typeof(AttributeDefinitonConverter))]
            public string Definition { get; set; }
        }

        public NodeDefinition()
        {
            AttributesDefiningType = new List<AttributeValuePair>();
            Name = "";
            NameDefiningType = "";
            MaximumSubnodes = 0;
            DefaultNodes = new List<ExternalNode>();
            AllowedNodeType = new List<NodeDefinitionCountPair>();
            Removable = true;
            AttributeDefinitions = new List<ExternalAttribute>();
            DefaultAttributeDefinitions = new List<ExternalAttribute>();

        }



        [Category("Metadata"), Description("Editor-only node type name")]
        public string Name { get; set; }

        [Category("Type definition"), Description("Xml name")]
        public string NameDefiningType { get; set; }

        [Category("Type definition"), Description("")]
        public List<AttributeValuePair> AttributesDefiningType { get; set; }

        
        [Category("Nodes"), Description("Maximum sub-node count (-1 = infinite)")]
        public int MaximumSubnodes { get; set; }

        [Category("Nodes"), Description("Nodes automagically inserted with this node")]
        public List<ExternalNode> DefaultNodes { get; set; }

        [Category("Nodes"), Description("If not empty only items from this list can be added (-1 = infinite)")]
        public List<NodeDefinitionCountPair> AllowedNodeType { get; set; }

        [Category("Nodes"), Description("If false, node will disappear only if user remove higer level node")]
        public bool Removable { get; set; }

        [Category("Nodes"), Description("Type of value"), TypeConverter(typeof(AttributeDefinition.TypeConventer))]
        public string Type { get; set;}

        [Category("Attributes"), Description("Attributes list available for this node")]
        public List<ExternalAttribute> AttributeDefinitions { get; set; }

        [Category("Attributes"), Description("Automagically inserted attributes")]
        public List<ExternalAttribute> DefaultAttributeDefinitions { get; set; }

        public class NodeDefinitonConverter : StringConverter
        {
            public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
            {
                return true;
            }

            public override StandardValuesCollection
                     GetStandardValues(ITypeDescriptorContext context)
            {

                string[] Result = new string[MainForm.Instance.NodeDefinitions.NodeDefinitions.Count];

                for(int i = 0; i < Result.Length; i++)
                {
                    Result[i] = MainForm.Instance.NodeDefinitions.NodeDefinitions[i].Name;
                }

                return new StandardValuesCollection(Result);
            } 
        }
        public class AttributeDefinitonConverter : StringConverter
        {
            public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
            {
                return true;
            }

            public override StandardValuesCollection
                     GetStandardValues(ITypeDescriptorContext context)
            {

                string[] Result = new string[MainForm.Instance.NodeDefinitions.AttributeDefinitions.Count];

                for (int i = 0; i < Result.Length; i++)
                {
                    Result[i] = MainForm.Instance.NodeDefinitions.AttributeDefinitions[i].Name;
                }

                return new StandardValuesCollection(Result);
            }
        } 
    }
}
