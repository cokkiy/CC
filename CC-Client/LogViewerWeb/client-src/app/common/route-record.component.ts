import { Router, ActivatedRoute, Params } from '@angular/router';
export class RouteRecordComponent {
    constructor(public router: Router) {
        sessionStorage.setItem('redirectUrl', this.router.url);
    }    
}