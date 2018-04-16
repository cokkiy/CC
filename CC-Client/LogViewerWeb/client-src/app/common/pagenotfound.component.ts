import { Component, OnInit } from '@angular/core';
import { Router, ActivatedRoute, Params } from '@angular/router';
@Component({
    template: '<div class="alert alert-info">404!找不到你要查找的页面。 <a routerLink="/" routerLinkActive="active">转到主页</a></div>'
})
export class PageNotFoundComponent {
}