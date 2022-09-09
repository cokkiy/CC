import { Component,OnInit }        from '@angular/core';
import { Router,
    NavigationExtras, ActivatedRoute, Params } from '@angular/router';
import { AuthService }      from './auth.service';

@Component({
    templateUrl: 'login.component.html',
    styles:['login.component.css'],
})
export class LoginComponent implements OnInit {
    message: string;
    callback: boolean;
    constructor(private authService: AuthService, private router: Router,
        private route: ActivatedRoute) {
        this.setMessage();
    }

    ngOnInit() {
        this.route.queryParams.forEach((params: Params) => {            
            this.callback = !(params['callback'] === 'false');
        });
        if (this.callback) {
            this.authService.endSigninMainWindow().then(() => {                
                if (this.authService.loggedIn) {
                    // Get the redirect URL from our auth service
                    // If no redirect has been set, use the default
                    let url = this.authService.redirectUrl;
                    let redirect = url ? url : '/';
                    
                    // Set our navigation extras object
                    // that passes on our global query params and fragment
                    let navigationExtras: NavigationExtras = {
                        preserveQueryParams: false,
                        preserveFragment: false
                    };

                    // Redirect the user
                    this.router.navigate([redirect], navigationExtras);
                }
            });
        }
        else {
            this.login();
        }
    }

    setMessage() {
        this.message = '请稍等，正在登录...';
    }

    login() {
        this.message = '请稍等，准备登录中 ...';
        this.setMessage();
        this.authService.startSigninMainWindow();        
    }

    logout() {
        this.authService.startSignoutMainWindow();
        this.setMessage();
    }
}


/*
Copyright 2016 Google Inc. All Rights Reserved.
Use of this source code is governed by an MIT-style license that
can be found in the LICENSE file at http://angular.io/license
*/