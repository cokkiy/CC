import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';

//enable product mode, if you want run in develop mode ,just comment the line 5
import { enableProdMode } from '@angular/core';


if (process.env.ENV === 'production') {
    enableProdMode();
}


import { AppModule } from './app.module';

const platform = platformBrowserDynamic();
platform.bootstrapModule(AppModule, []);