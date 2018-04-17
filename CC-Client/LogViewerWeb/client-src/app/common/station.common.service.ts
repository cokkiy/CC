import { Injectable } from "@angular/core";
import { Http, Response, Headers } from "@angular/http";
import { Observable } from "rxjs/Observable";
import { ErrorObservable } from "rxjs/observable/ErrorObservable";

@Injectable()
export class StationCommonService {
  private stationUrl: string = "api/stations";
  private processUrl = "api/processes";

  constructor(private http: Http) {}

  getRunningAndTotalStationCount(): Observable<Models.RunningAndTotalCount> {
    let url = `${this.stationUrl}/count`;
    return this.http
      .get(url)
      .map(this.extractData)
      .catch(this.handleError);
  }

  getStationsInfo(powerOnOnly: boolean): Observable<Models.StationViewModel[]> {
    let url = `${this.stationUrl}/info/${powerOnOnly}`;
    return this.http
      .get(url)
      .map(this.extractData)
      .catch(this.handleError);
  }

  getRunningProcessCounts(): Observable<number> {
    let url = `${this.processUrl}/running/count`;
    return this.http
      .get(url)
      .map(this.extractData)
      .catch(this.handleError);
  }

  getProcessStartAndExitMessages(
    count: number
  ): Observable<Models.ProcessInfo[]> {
    let url = `${this.processUrl}/message/${count}`;
    return this.http
      .get(url)
      .map(this.extractData)
      .catch(this.handleError);
  }

  private getMostUsedApps(
    begin: string,
    end: string,
    count: number
  ): Observable<Models.AppRunningStatistic[]> {
    let url: string = `${
      this.processUrl
    }/runningcount/${begin}/${end}/${count}`;
    return this.http
      .get(url)
      .map(this.extractData)
      .catch(this.handleError);
  }

  private getLongestRunningApps(
    begin: string,
    end: string,
    count: number
  ): Observable<Models.AppRunningStatistic[]> {
    let url: string = `${this.processUrl}/runningtime/${begin}/${end}/${count}`;
    return this.http
      .get(url)
      .map(this.extractData)
      .catch(this.handleError);
  }

  getAppRunningStatistic(
    begin: string,
    end: string,
    count: number,
    byTime: boolean
  ): Observable<Models.AppRunningStatistic[]> {
    if (byTime === true) {
      return this.getLongestRunningApps(begin, end, count);
    } else {
      return this.getMostUsedApps(begin, end, count);
    }
  }

  /**
   * 获取指定时间段内工作站运行统计信息
   *
   * @param {string} begin 开始时间
   * @param {string} end 结束时间
   * @param {Models.TimeType} type 统计分组类型,天/周/月/年
   * @returns {Observable<Models.StationRunningStatistic>}
   * @memberof StationCommonService
   */
  getStationRunningStatistic(
    begin: string,
    end: string,
    type: string,
    dinstict:boolean
  ): Observable<Models.StationRunningStatistic[]> {
    let url = `${this.stationUrl}/stat/${begin}/${end}/${type}/${dinstict}`;
    return this.http
      .get(url)
      .map(this.extractData)
      .catch(this.handleError);
  }

  /**
   * 获取工作站报警信息
   * @param {number} index 开始位置
   * @param {number} count 数量
   * @returns {Observable<Models.RunningState>} 报警信息列表
   * @memberof StationCommonService
   */
  getStationAlerts(
    index: number,
    count: number
  ): Observable<Models.RunningState> {
    let url: string = `${this.stationUrl}/alerts/${index}/${count}`;
    return this.http
      .get(url)
      .map(this.extractData)
      .catch(this.handleError);
  }

  private extractData(res: Response): any {
    let body: any = res.json();
    return body || {};
  }

  private handleError(error: any): ErrorObservable {
    // in a real world app, we might use a remote logging infrastructure
    // we'd also dig deeper into the error to get a better message
    let errMsg: any = error.message
      ? error.message
      : error.status ? `${error.status} - ${error.statusText}` : "Server error";
    console.error(errMsg); // log to console instead
    return Observable.throw(errMsg);
  }
}
