import { NgModule } from "@angular/core";
import { CommonModule } from "@angular/common";
import { FormsModule } from "@angular/forms";

import { Pagination } from "./pager/pagination.component";
import { StationCommonService } from "./station.common.service";
import { StationRunningStatisticComponent } from "./stat/station-running-statistic.component";
import { NgxEchartsModule } from "ngx-echarts";
import { NgDatepickerModule } from "ng2-datepicker";

@NgModule({
    imports: [
        FormsModule,
        CommonModule,
        NgDatepickerModule,
        NgxEchartsModule
    ],
    declarations: [
        StationRunningStatisticComponent,
        Pagination
    ],
    providers: [
        StationCommonService,
    ],
    exports: [
        Pagination,
        StationRunningStatisticComponent
        ]
})
export class MyCommonModule { }