"use strict";

module.exports = {
    rootJsFolder: "./wwwroot/js/",
    rootCssFolder: "./wwwroot/css",
    sources: {
        jsFilesInclSourcePaths: [
            "node_modules/core-js/client/shim.min.js",
            "node_modules/zone.js/dist/zone.js",
            "node_modules/reflect-metadata/Reflect.js",
            "node_modules/systemjs/dist/system.src.js",
            "node_modules/jquery/dist/jquery.js",
            "node_modules/bootstrap/dist/js/bootstrap.js",
            "node_modules/moment/moment.js",
        ],

        jsMapFilesInclSourcePaths: [
            "node_modules/core-js/client/shim.min.js.map",
            "node_modules/reflect-metadata/Reflect.js.map",
            "node_modules/bootstrap/dist/js/bootstrap.js.map",
        ],

        angularRC: "node_modules/@angular/**/*.*",
        Rxjs: "node_modules/rxjs/**/*.*",
        ngx_uploader:"node_modules/ngx-uploader/**/*.*",
        cssfiles: [
            "node_modules/bootstrap/dist/css/bootstrap.min.css",
            "node_modules/bootstrap/dist/css/bootstrap.min.css.map",
        ]
    }
};