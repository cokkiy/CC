///<reference path='./models/_models.ts'/>
import { NgModule, Injector } from "@angular/core";
import { CommonModule }   from "@angular/common";
import { BrowserModule } from "@angular/platform-browser";
import { BrowserAnimationsModule } from "@angular/platform-browser/animations";
import { FormsModule } from "@angular/forms";
import { HttpModule, JsonpModule, XHRBackend, Http, RequestOptions } from "@angular/http";
import { NgDatepickerModule } from "ng2-datepicker";

import { routing, appRoutingProviders } from "./app.routing";
import { DialogService }        from "./common/dialog.service";
import { LoginComponent } from "./security/login.component";
import { LogoutComponent } from "./security/logout.component";
import { AppComponent }   from "./app.component";
import { AppHomeComponent } from "./home/app.home.component";
import { PageNotFoundComponent } from "./common/pagenotfound.component";
import { httpFactory } from "./common/http.factory";
import { Router } from "@angular/router";
import { AuthService } from "./security/auth.service";
import { CookieModule } from "ngx-cookie";
import { NgxEchartsModule } from "ngx-echarts";
import { MyCommonModule } from "./common/common.module";
import { NgPipesModule } from "ngx-pipes";
import { StatModule } from "./stat/stat.module";


@NgModule({
    imports: [
        BrowserModule,
        CommonModule,
        BrowserAnimationsModule,
        FormsModule,
        HttpModule,
        JsonpModule,
        routing,
        NgxEchartsModule,
        CookieModule.forRoot(),
        NgDatepickerModule,
        NgPipesModule,
        MyCommonModule,
        StatModule
    ],
    declarations: [
        AppComponent,
        AppHomeComponent,
        LoginComponent,
        LogoutComponent,
        PageNotFoundComponent],
    providers: [
        appRoutingProviders,
        DialogService,
        {
            provide: Http,
            useFactory: httpFactory,
            deps: [XHRBackend, RequestOptions, Router, AuthService]
        },
        ],
    bootstrap: [AppComponent]
})
export class AppModule { }