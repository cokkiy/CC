using System;
using System.IO;
using System.Reflection;

namespace CC_StationService.Properties {
    
    
    // 通过此类可以处理设置类的特定事件: 
    //  在更改某个设置的值之前将引发 SettingChanging 事件。
    //  在更改某个设置的值之后将引发 PropertyChanged 事件。
    //  在加载设置值之后将引发 SettingsLoaded 事件。
    //  在保存设置值之前将引发 SettingsSaving 事件。
    internal sealed partial class Settings {
        
        public Settings() {
            // // 若要为保存和更改设置添加事件处理程序，请取消注释下列行: 
            //
            // this.SettingChanging += this.SettingChangingEventHandler;
            //
            // this.SettingsSaving += this.SettingsSavingEventHandler;
            //
        }
        
        private void SettingChangingEventHandler(object sender, System.Configuration.SettingChangingEventArgs e) {
            // 在此处添加用于处理 SettingChangingEvent 事件的代码。
        }
        
        private void SettingsSavingEventHandler(object sender, System.ComponentModel.CancelEventArgs e) {
            // 在此处添加用于处理 SettingsSaving 事件的代码。
        }


        /// <summary>
        /// 加载保存的配置信息
        /// </summary>
        public void Load()
        {
            if (Environment.OSVersion.Platform == PlatformID.Unix
                    || Environment.OSVersion.Platform == PlatformID.MacOSX)
            {
                try
                {
                    // linux,unix,macos，保存到文件中
                    using (TextReader reader = new StreamReader("/etc/ccservice.conf"))
                    {
                        string line = reader.ReadLine();
                        while (line != null)
                        {
                            int pos = line.IndexOf('=');
                            if (pos != -1)
                            {
                                string name = line.Substring(0, pos);
                                string value = line.Substring(pos + 1);
                                Type t = this.GetType();
                                PropertyInfo property = t.GetProperty(name);
                                if (property != null)
                                {
                                    object v = Convert.ChangeType(value, property.PropertyType);
                                    property.SetValue(this, v, null);
                                }
                            }
                            line = reader.ReadLine();
                        }


                        reader.Close();
                    }
                }
                catch
                {

                }
            }
        }

        public override void Save()
        {
            if (Environment.OSVersion.Platform == PlatformID.Unix
                    || Environment.OSVersion.Platform == PlatformID.MacOSX)
            {
                base.Save();
                try
                {
                    // linux,unix,macos，保存到文件中
                    using (TextWriter writer = new StreamWriter("/etc/ccservice.conf"))
                    {
                        Type t = this.GetType();
                        PropertyInfo[] properties = t.GetProperties(BindingFlags.Instance | BindingFlags.Public | BindingFlags.DeclaredOnly);
                        foreach (var property in properties)
                        {
                            writer.WriteLine("{0}={1}", property.Name, property.GetValue(this, null));
                        }
                        writer.Close();
                    }
                }
                catch
                {

                }
            }
            else
            {
                // windows 
                base.Save();
            }
        }


    }
}
