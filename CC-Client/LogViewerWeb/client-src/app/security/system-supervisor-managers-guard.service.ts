import { Injectable } from '@angular/core';
import {
    CanActivate, Router,
    ActivatedRouteSnapshot,
    RouterStateSnapshot,
    CanActivateChild,
    NavigationExtras,
    CanLoad, Route
} from '@angular/router';
import { AuthService } from './auth.service';
import * as jwtDecode from 'jwt-decode';


@Injectable()
export class SystemSupervisorManagersGuard implements CanActivate, CanActivateChild, CanLoad {
    constructor(private authService: AuthService, private router: Router) { }

    canActivate(route: ActivatedRouteSnapshot, state: RouterStateSnapshot): boolean {
        let url: string = state.url;
        return this.checkLogin(url);
    }

    canActivateChild(route: ActivatedRouteSnapshot, state: RouterStateSnapshot): boolean {
        return this.canActivate(route, state);
    }

    canLoad(route: Route): boolean {
        let url = `/${route.path}`;
        return this.checkLogin(url);
    }

    checkLogin(url: string): boolean {
        //console.log("logined in:" + this.authService.loggedIn);
        if (this.authService.loggedIn) {
            var token = jwtDecode(this.authService.currentUser.access_token);
            ///console.log(token);
            //var ac = JSON.parse(token);
            if (token["role"]) {
                var regex = new RegExp("^SupervisorManagers.System?$", "i");
                var index = token["role"].findIndex((m: string) => { return regex.test(m); });

                if (index >= 0) {
                    return true;
                }
                else {
                    // Store the attempted URL for redirecting
                    this.authService.redirectUrl = url;
                    this.router.navigate(["/logout"], { queryParams: { relogin: true, callback: false } });
                    return false;
                }
            }
            else {
                //console.log("logout");
                this.authService.redirectUrl = url;
                this.router.navigate(["/logout"], { queryParams: { relogin: true, callback: false } });
                return false;
            }
        }

        // Store the attempted URL for redirecting
        this.authService.redirectUrl = url;

        // Create a dummy session id
        //let sessionId = 123456789;

        // Set our navigation extras object
        // that contains our global query params and fragment
        let navigationExtras: NavigationExtras = {
            queryParams: { /*'session_id': sessionId,*/'callback': 'false' },
            //fragment: 'anchor'
        };

        // Navigate to the login page with extras
        this.router.navigate(['/login'], navigationExtras);
        return false;
    }
}
