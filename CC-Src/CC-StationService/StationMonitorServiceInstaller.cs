using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// Installer for StationMonitorService
    /// </summary>
    [System.ComponentModel.RunInstaller(true)]
    public class StationMonitorServiceInstaller : System.Configuration.Install.Installer
    {
        public StationMonitorServiceInstaller()
        {
            var processInstaller = new ServiceProcessInstaller();
            var serviceInstaller = new ServiceInstaller();
            processInstaller.Account = ServiceAccount.LocalSystem;

            serviceInstaller.DisplayName = "工作站监视服务";
            serviceInstaller.StartType = ServiceStartMode.Automatic;
            serviceInstaller.ServiceName = "Station Monitor Service";
            serviceInstaller.Description = "监视工作站CPU,内存占用率和应用程序状态.";
            
            this.Installers.Add(processInstaller);
            this.Installers.Add(serviceInstaller);
        }
    }
}
