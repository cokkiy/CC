import { Routes }         from '@angular/router';
import { AuthGuard }      from './auth-guard.service';
import { AuthService } from './auth.service';
import { OrgManageGuard } from './org-manage-guard.service';
import { OrgTracePublishGuard } from './org-trace.publish-guard.service';
import { SystemAdminsGuard } from './system-admins-guard.service';
import { SystemConfigsGuard } from './system-configs-guard.service';
import { SystemSupervisorManagersGuard } from './system-supervisor-managers-guard.service';
import { SystemOrgReviewGuard } from './system-org-review-guard.service';
import { LoginComponent } from './login.component';
import { LogoutComponent } from './logout.component';

export const loginRoutes: Routes = [
    { path: 'login', component: LoginComponent },
    { path: 'logout', component: LogoutComponent }
];

export const authProviders = [
    AuthGuard,
    OrgManageGuard,
    OrgTracePublishGuard,
    SystemAdminsGuard,
    SystemConfigsGuard,
    SystemOrgReviewGuard,
    SystemSupervisorManagersGuard,
    AuthService
];