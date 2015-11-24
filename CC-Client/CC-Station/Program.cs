using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC_Station
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Starting...");
            Ice.Communicator ic = null;
            try
            {
                ic = Ice.Util.initialize(ref args);
                Ice.ObjectAdapter adapter = ic.createObjectAdapterWithEndpoints("CCStationAdapter", "default -h 192.168.0.203 -p 10000");
                Ice.Object obj = new ControllerImp();
                adapter.add(obj, ic.stringToIdentity("StationController"));
                adapter.activate();
                Console.WriteLine("Started");
                ic.waitForShutdown();
            }
            catch(Exception e)
            {
                Console.WriteLine(e);
            }

            if(ic!=null)
            {
                //clean up
                try
                {
                    ic.destroy();
                }
                catch(Exception e)
                {
                    Console.WriteLine(e);
                }
            }
        }
    }
}
