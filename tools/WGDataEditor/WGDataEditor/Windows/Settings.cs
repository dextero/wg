using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.ComponentModel;

namespace WGDataEditor
{
    public class Settings
    {
        public Settings()
        {
            RecentFiles = new List<string>();
            DataDirectory = "";
            WorkingDirectory = "";
        }
        [Editor("System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(System.Drawing.Design.UITypeEditor))]
        public List<string> RecentFiles { get; set; }

        [Category("Directories"), Description("Directory that contains all data that editor need to work"), EditorAttribute(typeof(System.Windows.Forms.Design.FolderNameEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string DataDirectory { get; set; }
        [Category("Directories"), Description("Warlock's Gauntlet data folder"), EditorAttribute(typeof(System.Windows.Forms.Design.FolderNameEditor), typeof(System.Drawing.Design.UITypeEditor))]
        public string WorkingDirectory { get; set; }

        public void Load()
        {
            string ConfigFilePath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Config.xml");
            if (File.Exists(ConfigFilePath))
            {
                using (StreamReader Reader = new StreamReader(ConfigFilePath))
                {
                    System.Xml.Serialization.XmlSerializer Serializer = new System.Xml.Serialization.XmlSerializer(typeof(Settings));
                    MainForm.Instance.Settings = (Settings)Serializer.Deserialize(Reader);
                }
            }
        }

        public void Save()
        {
            string ConfigFilePath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Config.xml");
            using (StreamWriter Writer = new StreamWriter(ConfigFilePath))
            {

                System.Xml.Serialization.XmlSerializerNamespaces ns = new System.Xml.Serialization.XmlSerializerNamespaces();
                ns.Add("", "");

                System.Xml.Serialization.XmlSerializer Serializer = new System.Xml.Serialization.XmlSerializer(typeof(Settings));
                Serializer.Serialize(Writer, MainForm.Instance.Settings, ns);
            }
        }
    }
}
