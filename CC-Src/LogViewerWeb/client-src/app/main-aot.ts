declare var System: any;



import { platformBrowser } from '@angular/platform-browser';

import { enableProdMode } from '@angular/core';

import { AppModuleNgFactory } from '../aot/client-src/app/app.module.ngfactory';

enableProdMode();

platformBrowser().bootstrapModuleFactory(AppModuleNgFactory);