import { Injectable } from '@angular/core';
import { Http, ConnectionBackend, Headers, RequestOptions, Request, Response, RequestOptionsArgs } from '@angular/http';
import { Observable } from "rxjs/Observable";
import { Router } from "@angular/router";
import { AuthService } from "../security/auth.service";

@Injectable()
export class AuthAwareHttpService extends Http {
    constructor(backend: ConnectionBackend,
        defaultOptions: RequestOptions,
        private router: Router,
        private authService: AuthService) {
        super(backend, defaultOptions);
    }

    request(url: string | Request, options?: RequestOptionsArgs): Observable<Response> {
        return Observable.fromPromise(this.setHeadersThenRequest(url, options));
    }

    post(url: string, body: any, options?: RequestOptionsArgs): Observable<Response> {
        if (!options) {
            options = { headers: new Headers() };
        }
        options.headers.set('Content-Type', `application/json`);;
        return super.post(url, body, options);
    }
    /**
     * Performs a request with `put` http method.
     */
    put(url: string, body: any, options?: RequestOptionsArgs): Observable<Response> {
        if (!options) {
            options = { headers: new Headers() };
        }
        options.headers.set('Content-Type', `application/json`);;
        return super.put(url, body, options);
    }



    private catchErrors() {
        return (res: Response) => {
            if (res.status === 401 || res.status === 403) {
                if (this.authService.loggedIn) {                    
                    this.router.navigate(["/logout"], { queryParams: { relogin: true, callback: false } });
                }
                else {
                    this.router.navigate(['/login'], { queryParams: { callback: false } });
                }
            }
            return Observable.throw(res);
        };
    }

    private async setHeadersThenRequest(url: string | Request, options?: RequestOptionsArgs): Promise<Response> {

        let loggedIn = await this.authService.checkLogin();
        //console.log(loggedIn);
        if (loggedIn) {
            if (typeof url === 'string') {
                if (!options) {
                    options = { headers: new Headers() };
                }
                options.headers.set('Authorization', `Bearer ${this.authService.currentUser.access_token}`);;
            } else {
                url.headers.set('Authorization', `Bearer ${this.authService.currentUser.access_token}`);;
            }
        }

        return super.request(url, options).catch(this.catchErrors()).toPromise();
    }
}