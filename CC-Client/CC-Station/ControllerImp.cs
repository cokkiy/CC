using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CC;
using Ice;

namespace CC_Station
{
    class ControllerImp : ControllerDisp_
    {
        public override void reboot(bool force, Current current__)
        {
            Console.WriteLine("reboot");
        }

        public override void startApp(string appName, Current current__)
        {
            Console.WriteLine("Start App");
        }
    }
}
