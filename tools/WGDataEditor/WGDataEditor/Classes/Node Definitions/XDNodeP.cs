using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace WGDataEditor
{
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

        public XDNodeP(System.Xml.XmlNode NewXmlNode)
        {            
            XmlNode = NewXmlNode;
            Definition = FindNodeDefinition(NewXmlNode);
        }
        public NodeDefinition Definition;
        public System.Xml.XmlNode XmlNode;

        public static NodeDefinition FindNodeDefinition(XmlNode XmlNode)
        {
            foreach (var Definition in MainForm.Instance.NodeDefinitions.NodeDefinitions)
            {
                if (Definition.NameDefiningType == XmlNode.Name)
                {
                    foreach (var Attribute in Definition.AttributesDefiningType)
                    {
                        AttributeDefinition ADefinition = MainForm.Instance.NodeDefinitions.AttributeDefinitions.Find(n => n.Name == Attribute.Definition.Name);

                        foreach (XmlAttribute XmlAttribute in XmlNode.Attributes)
                        {
                            
                            if (ADefinition.NameXml == XmlAttribute.Name && Attribute.Value == XmlAttribute.Value)
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
            NodeDefinition def = FindNodeDefinition(XDNodeP.Get(Node.Tag));
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

}
