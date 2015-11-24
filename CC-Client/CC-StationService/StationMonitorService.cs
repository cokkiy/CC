using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// 工作站信息监视服务
    /// </summary>
    public partial class StationMonitorService : ServiceBase
    {
        //ice communicator
        private Ice.Communicator ic = null;

        public StationMonitorService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            Ice.Communicator ic = null;
            try
            {
                ic = Ice.Util.initialize(ref args);
                Ice.ObjectAdapter adapter = ic.createObjectAdapterWithEndpoints("CCStationAdapter", "default -h 192.168.0.203 -p 10000");
                Ice.Object obj = new ControllerImp();
                adapter.add(obj, ic.stringToIdentity("StationController"));
                adapter.activate();
                ic.waitForShutdown();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        protected override void OnStop()
        {
            if (ic != null)
            {
                //clean up
                try
                {
                    ic.destroy();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
        }
    }
}
