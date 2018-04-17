import { Component, OnInit } from '@angular/core';
import {
    Router,
    NavigationExtras, ActivatedRoute, Params
} from '@angular/router';
import { AuthService } from './auth.service';

@Component({
    templateUrl: 'logout.component.html',
    styles: ['logout.component.css'],
})
export class LogoutComponent implements OnInit {
    message: string;
    relogin: boolean;
    callback: boolean;
    constructor(private authService: AuthService, private router: Router,
        private route: ActivatedRoute) {
    }

    ngOnInit() {
        this.route.queryParams.forEach((params: Params) => {
            //console.log(params['relogin']);
            this.relogin = (params['relogin'] === 'true');
            this.callback = !(params['callback'] === 'false');
            //console.log(this.relogin);

            if (this.callback) {
                this.authService.endSignoutMainWindow();
                this.authService.removeUser();
                let navigationExtras: NavigationExtras = {
                    queryParams: { /*'session_id': sessionId,*/'callback': 'false' },
                    //fragment: 'anchor'
                };
                this.router.navigate(['/login'], navigationExtras);
            }
            else {
                if (this.relogin) {
                    this.message = "你没有权限进行当前操作，可以注销后更换具有权限的用户重新登录后进行操作或者返回主页。";
                }
                else {
                    this.message = "确实要注销吗？注销后大部分功能将不可用。";
                }
            }
        });        
    }

    returnToHome() {
        this.router.navigate(["/"]);
    }

    logout() {
        this.authService.startSignoutMainWindow();
    }    
}
