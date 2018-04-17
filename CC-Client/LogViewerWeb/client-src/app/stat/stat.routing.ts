import { ModuleWithProviders } from "@angular/core";
import { Routes, RouterModule } from "@angular/router";
import { StatComponent } from "./stat.component";
import { StationRunningStatisticDetailComponent } from "./station-running-statistic-detail.component";

const statRoutes: Routes = [
  {
    path: "stat",
    component: StatComponent,
    children: [
      {
        path: "",
        component: StationRunningStatisticDetailComponent
      },
      // {
      //   path: "search/:id",
      //   component: TraceSearchComponent,
      //   resolve: { org: OrganizationResolver }
      //   //canActivate: [OrgManageGuard]
      // },
      // {
      //   path: "opVideo/:code/:id",
      //   component: OperationVideoComponent,
      //   resolve: { track: SourceTrackResolver }
      // },
      // {
      //   path: ":code",
      //   component: TraceItemComponent
      // }
    ]
  }
];

export const statRouting: ModuleWithProviders = RouterModule.forChild(
  statRoutes
);
