using LogViewerWeb.Data;
using LogViewerWeb.Data.DataPersisters;
using LogViewerWeb.Models.ViewModels;
using Microsoft.AspNetCore.Mvc;
using StationLogModels;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace LogViewerWeb.Controllers
{
    [Produces("application/json")]
    [Route("api/stations")]    
    public class StationsController : Controller
    {
        private IStationDataProvider _stationProvider;
        private IStationPersister _stationPersister;
        public StationsController(IStationDataProvider stationProvider, IStationPersister stationPersister)
        {
            _stationProvider = stationProvider;
            _stationPersister = stationPersister;
        }
        // GET: api/values
        [HttpGet]
        [Route("")]
        public async Task<IActionResult> GetStations()
        {
            var stations = await _stationProvider.GetStations();
            return Ok(stations);
        }

        // GET api/values/5/districts
        [HttpGet("{groupName}")]
        [Route("{groupName}")]
        public async Task<IActionResult> GetStations(string groupName)
        {
            var stations = await _stationProvider.GetStationsInGroup(groupName);
            return Ok(stations);
        }

        [HttpGet()]
        [Route("count")]
        public IActionResult GetRunningAndPowerOffStationsCount()
        {
            var result = _stationProvider.GetRunningAndTotalCount();
            return Ok(new RunningAndTotalCount { Running = result.running, Total = result.total });
        }

        [HttpGet("info/{powerOnOnly}")]
        [Route("info/{powerOnOnly:bool=true}")]
        public async Task<IActionResult> GetStationsInfo(bool powerOnOnly = true)
        {
            var result = await _stationProvider.GetStationsInfo(powerOnOnly);
            return Ok(result);
        }

        [HttpGet]
        [Route("stat/{begin:datetime}/{end:datetime}/{type:TimeType?}/{distinct:bool?}")]
        public async Task<IActionResult> GetStationRunningStatistic(DateTime begin, DateTime end, TimeType type = TimeType.Daily, bool distinct = true)
        {
            List<StationRunningStatistic> statistics = await _stationProvider.GetStationRunningStatistics(begin, end, type, distinct);
            return Ok(statistics);
        }

        [HttpGet]
        [Route("alerts/{index:int?}/{count:int?}")]
        public async Task<IActionResult> GetStationAlerts(int index = 0, int count = 10)
        {
            List<RunningState> alerts = await _stationProvider.GetStationAlerts(index, count);
            return Ok(alerts);
        }
    }
}
