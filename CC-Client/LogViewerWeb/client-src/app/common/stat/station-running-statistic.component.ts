import { Component, OnInit, NgZone, Input } from "@angular/core";
import { StationCommonService } from "../station.common.service";
import { ECharts, EChartOption } from "echarts";
import * as moment from "moment";
import { DatepickerOptions } from "ng2-datepicker";
import * as zhLocale from "date-fns/locale/zh_cn";

@Component({
  selector: "StationRunningStatistic",
  templateUrl: "station-running-statistic.component.html",
  styleUrls: ["station-running-statistic.component.scss"]
})
export class StationRunningStatisticComponent implements OnInit {
  @Input() showCtrl: boolean = false;
  @Input() dinstict: boolean = true;
  errorMessage: string;
  stationChart: ECharts;
  private begin: string;
  private end: string;
  private type: string;
  private datapicker_options: DatepickerOptions;
  typeOptions: Models.TextValuePair[] = [
    {
      text: "小时",
      value: "Hourly"
    },
    {
      text: "天",
      value: "Daily"
    },
    {
      text: "周",
      value: "Weekly"
    },
    {
      text: "月",
      value: "Monthly"
    },
    {
      text: "季度",
      value: "Quarterly"
    },
    {
      text: "年",
      value: "Yearly"
    }
  ];

  defaultOption: EChartOption = {
    title: {
      text: "加载数据..."
    },
    tooltip: {},
    animationDurationUpdate: 1500,
    animationEasingUpdate: "quinticInOut"
  };

  constructor(
    private zone: NgZone,
    private stationCommonService: StationCommonService
  ) {
    this.begin = moment()
      .subtract(10, "days")
      .toJSON();
    this.end = moment().toJSON();
    this.type = "Daily";
    this.datapicker_options = {
      minYear: 1970,
      maxYear: 2030,
      displayFormat: "MMM D[,] YYYY",
      barTitleFormat: "MMMM YYYY",
      dayNamesFormat: "dd",
      firstCalendarDay: 0, // 0 - Sunday, 1 - Monday
      locale: zhLocale,
      // minDate: new Date(Date.now()), // minimal selectable date
      maxDate: new Date(Date.now()), // maximal selectable date
      barTitleIfEmpty: "点击选择日期"
    };
  }

  ngOnInit(): void {
    this.getStationRunningStatistic();
  }

  getStationRunningStatistic(): void {
    this.stationCommonService
      .getStationRunningStatistic(
        this.begin,
        this.end,
        this.type,
        this.dinstict
      )
      .subscribe(stats => {
        let title: string = this.dinstict
          ? "运行的工作站（台）"
          : "运行的工作站（台次）";
        console.log(title);
        let option: EChartOption = {
          tooltip: {
            trigger: "axis"
          },
          legend: {
            data: [title]
          },
          dataZoom: [
            {
              id: "dataZoomX",
              type: "inside",
              xAxisIndex: 0,
              filterMode: "filter",
              zoomOnMouseWheel: true,
              moveOnMouseMove: false
            }
          ],
          xAxis: {
            type: "category",
            data: stats.map(d => d.time)
          },
          yAxis: { type: "value" },
          series: [
            {
              name: title,
              type: "line",
              data: stats.map(d => d.count)
            }
          ]
        };

        this.stationChart.hideLoading();
        this.stationChart.setOption(option, true);
      }, err => (this.errorMessage = err));
  }

  onStationChartInit(ec: ECharts): void {
    this.stationChart = ec;
    this.stationChart.showLoading();
  }

  update():void {
    this.begin= moment(this.begin).toJSON();
    this.end=moment(this.end).toJSON();
    this.getStationRunningStatistic();
  }
}
