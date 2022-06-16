import { ModuleWithProviders }  from "@angular/core";
import { Routes, RouterModule } from "@angular/router";
import { AppHomeComponent } from "./home/app.home.component";
import { loginRoutes, authProviders } from "./security/login.routing";
import { CanDeactivateGuard } from "./common/can-deactivate-guard.service";
import { AuthGuard }          from "./security/auth-guard.service";
import { PageNotFoundComponent } from "./common/pagenotfound.component";

const appRoutes: Routes = [
    ...loginRoutes,
    { path: "", component: AppHomeComponent },
    { path: "404", component: PageNotFoundComponent },
    { path: "**", component: PageNotFoundComponent }
];

export const routing: ModuleWithProviders = RouterModule.forRoot(appRoutes);

export const appRoutingProviders: any[] = [
    authProviders,
    CanDeactivateGuard
];