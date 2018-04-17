using Serilog;

namespace LogClient
{
    public class Logger : Ice.Logger
    {
        private string prefix;
        private readonly Serilog.Core.Logger logger;

        public Logger(string prefix = "")
        {
            this.prefix = prefix;
            logger = new Serilog.LoggerConfiguration()
                .WriteTo.Console(restrictedToMinimumLevel:Serilog.Events.LogEventLevel.Verbose)
                .WriteTo.RollingFile("log/log.log")
                .CreateLogger();
        }

        public void error(string message)
        {
            if(string.IsNullOrWhiteSpace(prefix))
            {
                logger.Error("{0}",message);
            }
            else
            {
                logger.Error("{0}-{1}", prefix, message);
            }
            
        }

        public string getPrefix()
        {
            return prefix;
        }

        public void print(string message)
        {
            if (string.IsNullOrWhiteSpace(prefix))
            {
                logger.Information("{0}", message);
            }
            else
            {
                logger.Information("{0}-{1}", prefix, message);
            }
        }

        public void trace(string category, string message)
        {
            if (string.IsNullOrWhiteSpace(prefix))
            {
                logger.Debug("{0}:{1}", category, message);
            }
            else
            {
                logger.Debug("{0}-{1}:{2}", prefix, category, message);
            }

        }

        public void warning(string message)
        {
            if (string.IsNullOrWhiteSpace(prefix))
            {
                logger.Warning("{0}",message);
            }
            else
            {
                logger.Warning("{0}-{1}", prefix, message);
            }
            
        }


        Ice.Logger Ice.Logger.cloneWithPrefix(string prefix)
        {
            return new Logger(prefix);
        }

    }
}
