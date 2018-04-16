import { Injectable } from "@angular/core";
import { Http, Response } from "@angular/http";
import { Observable } from "rxjs/Observable";

@Injectable()
export class StatService {
  constructor(private http: Http) {}

  private statUrl = "api/stat"; // url to web API

  private extractData(res: Response) {
    let body = res.json();
    return body || {};
  }

  private handleError(error: any) {
    // in a real world app, we might use a remote logging infrastructure
    // we'd also dig deeper into the error to get a better message
    let errMsg = error.message
      ? error.message
      : error.status ? `${error.status} - ${error.statusText}` : "Server error";
    console.error(errMsg); // log to console instead
    return Observable.throw(errMsg);
  }
}
