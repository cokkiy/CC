using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using LogViewerWeb.Data;
using LogViewerWeb.Models.ViewModels;
using Microsoft.AspNetCore.Mvc;

// For more information on enabling MVC for empty projects, visit https://go.microsoft.com/fwlink/?LinkID=397860

namespace LogViewerWeb.Controllers
{
    [Produces("application/json")]
    [Route("api/processes")]
    public class ProcessesController : Controller
    {
        private readonly IProcessDataProvider processDataProvider;
        public ProcessesController(IProcessDataProvider processDataProvider)
        {
            this.processDataProvider = processDataProvider;
        }

        [HttpGet()]
        [Route("running/count")]
        public async Task<IActionResult> GetRunningProcessesCount()
        {
            var count = await processDataProvider.GetRunningProcessesCount();
            return Json(count);
        }

        [HttpGet()]
        [Route("message/{count:int?}")]
        public async Task<IActionResult> GetProcessStartAndExitMessage(int count=10)
        {
            var message = await processDataProvider.GetProcessStartAndExitMessage(count);
            return Ok(message);
        }

        [HttpGet]
        [Route("runningtime/{begin:datetime}/{end:datetime?}/{count:int?}")]
        public async Task<IActionResult> GetAppRunningTimeStatistic(DateTime begin, DateTime? end, int count = 10)
        {
            if (end == null)
            {
                end = DateTime.Now;
            }

            List<AppRunningStatistic> statistics = await processDataProvider.GetAppRunningTimeStatistic(begin, end.Value, count);
            return Ok(statistics);
        }

        [HttpGet]
        [Route("runningcount/{begin:datetime}/{end:datetime?}/{count:int?}")]
        public async Task<IActionResult> GetAppRunningCountStatistic(DateTime begin, DateTime? end, int count = 10)
        {
            if (end == null)
            {
                end = DateTime.Now;
            }

            List<AppRunningStatistic> statistics = await processDataProvider.GetAppRunningCountStatistics(begin, end.Value, count);
            return Ok(statistics);
        }
    }
}
