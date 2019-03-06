import { Component, OnInit, NgZone, OnDestroy } from "@angular/core";
import { Router, ActivatedRoute, Params } from "@angular/router";
// import { CookieService } from "ngx-cookie";
import { ECharts, EChartOption } from "echarts";
import { StationCommonService } from "../common/station.common.service";
import { Observable } from "rxjs/Observable";
import { Subscription } from "rxjs";
import { DatePipe } from "@angular/common";
import * as moment from "moment";

@Component({
  selector: "my-home",
  templateUrl: "app.home.component.html",
  styleUrls: ["app.home.component.scss"]
})
export class AppHomeComponent implements OnInit, OnDestroy {
  alerts: Models.RunningState;
  stationChart: ECharts;
  appRunningTimeBarChart: ECharts;
  messageRefresher: Subscription;
  startAndExitMessages: Models.ProcessInfo[];
  stationsInfo: Models.StationViewModel[];
  updateTime: Date;
  stationInfoUpdateTime: Date;
  appRunningTimeBarUpdateTime: string;
  timerRefresher: Subscription;
  errorMessage: string;
  loading: boolean;
  runningAndTotalCount: Models.RunningAndTotalCount;
  totalRunningPieChart: ECharts;
  powerOnOnly: boolean;
  runningProcessesCount: number = 0;
  timespanForMostusedApp: string = "preWeek";
  amountForMostusedApp: number = 10;
  statisticAppRunningType: string = "ByTime";
  private StationRunningStatisticBegin: string;
  private StationRunningStatisticEnd: string;
  private StationRunningStatisticTimeType: string;

  defaultOption = {
    title: {
      text: "加载数据..."
    },
    tooltip: {},
    animationDurationUpdate: 1500,
    animationEasingUpdate: "quinticInOut"
  };

  runningAndTotalCountPieOption = {
    tooltip: {
      trigger: "item",
      formatter: "{a}<br/>{b}: {c}台 ({d}%)"
    },
    legend: {
      orient: "vertical",
      x: "right",
      data: ["开机", "未开机"]
    },
    series: [
      {
        name: "运行对比",
        type: "pie",
        radius: ["30%", "70%"],
        avoidLabelOverlap: false,
        label: {
          normal: {
            show: true,
            position: "left",
            formatter: "{b}: {c}台 ({d}%)"
          },
          emphasis: {
            show: true,
            position: "center",
            textStyle: {
              fontSize: "14",
              fontWeigth: "bold"
            }
          }
        },
        labelLine: {
          normal: {
            show: false
          }
        },
        data: [{ value: 0, name: "开机" }, { value: 30, name: "未开机" }]
      }
    ]
  };

  constructor(
    private zone: NgZone,
    private route: ActivatedRoute,
    private router: Router,
    // private cookieService: CookieService,
    private stationCommonService: StationCommonService
  ) {
    this.StationRunningStatisticBegin = moment()
      .subtract(10, "days")
      .toJSON();
    this.StationRunningStatisticEnd = moment().toJSON();
    this.StationRunningStatisticTimeType = "Daily";
  }

  private redirect: string;

  ngOnInit(): void {
    this.powerOnOnly = true;
    this.route.queryParams.subscribe(params => (this.redirect = params["r"]));
    if (this.redirect) {
      // console.log(this.redirect);
      this.router.navigateByUrl(this.redirect);
    } else {
      this.refreshData();
      this.getMostUsedApps();
    }
  }

  ngOnDestroy(): void {
    if (this.timerRefresher) {
      this.timerRefresher.unsubscribe();
    }

    if (this.messageRefresher) {
      this.messageRefresher.unsubscribe();
    }
  }

  powerOnOnly_changed(): void {
    this.loadStationsInfo(this.powerOnOnly);
  }

  private loadData(): void {
    this.getRunningAndTotalCount();
    this.loadStationsInfo(this.powerOnOnly);
    this.getRunningProcessesCount();
    this.loadStationAlerts();
  }

  private getRunningAndTotalCount(): void {
    this.stationCommonService.getRunningAndTotalStationCount().subscribe(
      count => {
        this.runningAndTotalCount = count;
        this.loading = false;
        this.totalRunningPieChart.hideLoading();
        this.totalRunningPieChart.setOption(
          this.runningAndTotalCountPieOption,
          true
        );
        let option: any = {
          series: [
            {
              data: [
                { value: this.runningAndTotalCount.running, name: "开机" },
                {
                  value:
                    this.runningAndTotalCount.total -
                    this.runningAndTotalCount.running,
                  name: "未开机"
                }
              ]
            }
          ]
        };
        this.totalRunningPieChart.setOption(option, false);
      },
      err => {
        this.errorMessage = err;
        this.loading = false;
      }
    );
  }

  private loadStationsInfo(powerOnOnly: boolean): void {
    this.stationCommonService.getStationsInfo(powerOnOnly).subscribe(
      infos => {
        this.stationsInfo = infos;
        this.stationInfoUpdateTime = new Date();
      },
      err => {
        this.errorMessage = err;
      }
    );
  }

  private loadStationAlerts(): void {
    this.stationCommonService
      .getStationAlerts(0, 10)
      .subscribe(
        alerts => (this.alerts = alerts),
        err => (this.errorMessage = err)
      );
  }

  private getRunningProcessesCount(): void {
    this.stationCommonService.getRunningProcessCounts().subscribe(count => {
      if (typeof count === "number") {
        this.runningProcessesCount = count;
      } else {
        this.runningProcessesCount = 0;
      }
    }, err => (this.errorMessage = err));
  }

  private getProcessStartAndExitMessages(): void {
    this.stationCommonService
      .getProcessStartAndExitMessages(10)
      .subscribe(messages => {
        this.startAndExitMessages = messages;
      }, err => (this.errorMessage = err));
  }

  private refreshData(): void {
    this.timerRefresher = Observable.timer(0, 60000).subscribe(() => {
      this.loadData();
      this.updateTime = new Date();
    });

    this.messageRefresher = Observable.timer(0, 20000).subscribe(() => {
      this.getProcessStartAndExitMessages();
      this.updateTime = new Date();
    });
  }

  private getMostUsedApps(): void {
    var start: string, end: string;
    switch (this.timespanForMostusedApp) {
      case "week":
        start = moment()
          .startOf("week")
          .toJSON();
        end = moment()
          .endOf("week")
          .toJSON();
        break;
      case "preWeek":
        start = moment()
          .startOf("week")
          .subtract(7, "days")
          .toJSON();
        end = moment()
          .endOf("week")
          .subtract(7, "days")
          .toJSON();
        break;
      case "month":
        start = moment()
          .startOf("month")
          .toJSON();
        end = moment().toJSON();
        break;
      case "year":
        start = moment()
          .startOf("year")
          .toJSON();
        end = moment().toJSON();
        break;
    }

    this.stationCommonService
      .getAppRunningStatistic(
        start,
        end,
        this.amountForMostusedApp,
        this.statisticAppRunningType === "ByTime"
      )
      .subscribe(apps => {
        let appRunningTimeBarOpion: any = {
          tooltip: {
            trigger: "axis"
          },
          legend: {
            data: ["运行时间（分钟）", "运行次数"]
          },
          toolbox: {
            show: true,
            feature: {
              mark: { show: true },
              dataView: { show: true, readOnly: false },
              magicType: { show: true, type: ["line", "bar"] },
              restore: { show: true }
            }
          },
          calculabe: true,
          xAxis: [
            {
              type: "category",
              data: apps.map(app => app.appName),
              axisLabel: {
                rotate: -30
              }
            }
          ],
          yAxis: [
            {
              type: "value"
            }
          ],
          series: [
            {
              name: "运行时间（分钟）",
              type: "bar",
              markPoint: {
                data: [
                  { type: "max", name: "最大值" },
                  { type: "min", name: "最小值" }
                ]
              },
              data: apps.map(app => app.runningTimes),
              markLine: {
                data: [{ type: "average", name: "平均值" }]
              }
            },
            {
              name: "运行次数",
              type: "bar",
              markPoint: {
                data: [
                  { type: "max", name: "最大值" },
                  { type: "min", name: "最小值" }
                ]
              },
              data: apps.map(app => app.runningCount),
              markLine: {
                data: [{ type: "average", name: "平均值" }]
              }
            }
          ]
        };
        this.appRunningTimeBarChart.hideLoading();
        this.appRunningTimeBarChart.setOption(appRunningTimeBarOpion, true);
        this.appRunningTimeBarUpdateTime = moment()
          .locale("zh-cn")
          .format("llll");
      }, err => (this.errorMessage = err));
  }

  refreshAppTimeBarChart(): void {
    this.getMostUsedApps();
  }
  mostUsedAppParamChanged(): void {
    this.getMostUsedApps();
  }

  onChartInit(ec: any): void {
    this.totalRunningPieChart = ec;
    this.totalRunningPieChart.showLoading();
  }

  onAppRunningTimeBarChartInit(ec: any): void {
    this.appRunningTimeBarChart = ec;
    this.appRunningTimeBarChart.showLoading();
  }

  onChartClick(params: any): void {
    if (params.data.category <= 1) {
      this.zone.run(() => {
        this.router.navigate(["/trace", params.data.name]);
      });
    }
  }
}
