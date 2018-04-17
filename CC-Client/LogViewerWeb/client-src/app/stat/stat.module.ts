import { NgModule }       from "@angular/core";
import { FormsModule }    from "@angular/forms";
import { CommonModule }   from "@angular/common";
import { RouterModule } from "@angular/router";

import { StatComponent }     from "./stat.component";
import { StatService } from "./stat.service";
import { statRouting } from "./stat.routing";
import { NgDatepickerModule } from "ng2-datepicker";
import { MyCommonModule } from "../common/common.module";
import { NgxEchartsModule } from "ngx-echarts";
import { StationRunningStatisticDetailComponent } from "./station-running-statistic-detail.component";

@NgModule({
    imports: [
        CommonModule,
        FormsModule,
        statRouting,
        NgDatepickerModule,
        MyCommonModule,
        NgxEchartsModule
    ],
    declarations: [
        StatComponent,
        StationRunningStatisticDetailComponent
    ],

    providers: [
        StatService,
    ]
})
export class StatModule { }