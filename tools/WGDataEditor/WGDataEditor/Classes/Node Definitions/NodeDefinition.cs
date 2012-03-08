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


            public override string ToString()
            {
                return Definition + ": " + Count.ToString();
            }
        }
        public class ExternalNode
        {
            [TypeConverter(typeof(NodeDefinitonConverter))]
            public string Definition { get; set; }

            public override string ToString()
            {
                return Definition;
            }
        }
        public class AttributeValuePair
        {
            public AttributeValuePair()
            {
                Definition = null;
                Value = "";
            }

            public AttributeValuePair(ExternalAttribute Definition, string Value)
            {
                this.Definition = Definition;
                this.Value = Value;
            }

            [TypeConverter(typeof(AttributeDefinitonConverter))]
            public ExternalAttribute Definition { get; set; }

            public string Value { get; set; }

            public override string ToString()
            {
                return Definition + " = " + Value;
            }
        }
        public class ExternalAttribute
        {
            [TypeConverter(typeof(AttributeDefinitonConverterCollection))]
            public string Name { get; set; }

            public override string ToString()
            {
                return Name;
            }
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
            DefaultAttributeDefinitions = new List<AttributeValuePair>();
            Type = "none";
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

        [Category("Type definition"), Description("Root nodes can be used only as root of the document")]
        public bool IsRoot { get; set; }

        [Category("Nodes"), Description("Type of value"), TypeConverter(typeof(AttributeDefinition.NodeTypeConventer))]
        public string Type { get; set;}

        [Category("Attributes"), Description("Attributes list available for this node")]
        public List<ExternalAttribute> AttributeDefinitions { get; set; }

        [Category("Attributes"), Description("Automagically inserted attributes")]
        public List<AttributeValuePair> DefaultAttributeDefinitions { get; set; }

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

            public override bool CanConvertFrom(ITypeDescriptorContext context, Type destinationType)
            {
                if (destinationType == typeof(ExternalAttribute)) return true;

                else return base.CanConvertTo(context, destinationType);
            }

            public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
            {
                if (value is string)
                {
                    ExternalAttribute ExternalAttribute = new ExternalAttribute();
                    ExternalAttribute.Name = (string)value;
                    return ExternalAttribute;
                }
                return null;
            }

            public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
            {
                if(destinationType == typeof(string)) return true;
                return base.CanConvertTo(context, destinationType);
            }

            public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
            {
                if (value is ExternalAttribute)
                {
                    return ((ExternalAttribute)value).Name;
                }

                return base.ConvertTo(context, culture, value, destinationType);
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
        public class AttributeDefinitonConverterCollection : StringConverter
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
