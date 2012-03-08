using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using System.ComponentModel;

namespace WGDataEditor
{

    public class DefinitionContainer
    {
        public DefinitionContainer()
        {
            NodeDefinitions = new List<NodeDefinition>();
            AttributeDefinitions = new List<AttributeDefinition>();

            Types = new string[] { "none", "int", "string", "float" };
        }

        public List<NodeDefinition> NodeDefinitions;
        public List<AttributeDefinition> AttributeDefinitions;
        
        [Category("Node/Attribute Data"), Description("Attribute value types - for directory containg files types use syntax \"path~<editor name>~<path from working directory>\" example for monster abilities: path~monser_ability~abilities\\monsters\\*.xml")]
        public string[] Types { get; set; }

        public static DefinitionContainer Load()
        {
            string FilePath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), Path.Combine(MainForm.Instance.Settings.DataDirectory, "NodeDefinitions.xml"));
            if (File.Exists(FilePath))
            {
                using (StreamReader Reader = new StreamReader(FilePath))
                {
                    System.Xml.Serialization.XmlSerializer Serializer = new System.Xml.Serialization.XmlSerializer(typeof(DefinitionContainer));
                    return (DefinitionContainer)Serializer.Deserialize(Reader);
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

                System.Xml.Serialization.XmlSerializer Serializer = new System.Xml.Serialization.XmlSerializer(typeof(DefinitionContainer));
                Serializer.Serialize(Writer, this, ns);
            }
        }
    }

}
