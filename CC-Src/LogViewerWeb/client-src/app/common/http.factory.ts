import { XHRBackend, Http, RequestOptions } from "@angular/http";
import { Router } from "@angular/router";
import { AuthService } from "../security/auth.service";
import { AuthAwareHttpService } from "./auth-aware-http.service";

export function httpFactory(xhrBackend: XHRBackend, requestOptions: RequestOptions,
    router: Router, authService: AuthService): Http {

    return new AuthAwareHttpService(xhrBackend, requestOptions, router, authService);
}