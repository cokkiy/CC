/// <binding />
var gulp = require('gulp');
var runSeq = require('run-sequence');
var del = require('del');
var browserSync = require('browser-sync').create();
var watch = require('gulp-watch');
//var sysBuilder = require('systemjs-builder');
var concat = require('gulp-concat');
var uglify = require('gulp-uglify');
var rename = require("gulp-rename");
var copy = require('gulp-copy');
var buildConfig = require('./config/gulp.config');

gulp.task('init', function (done) {
    runSeq(
        'clean',
        'copy-images',
        //'copy-auth',
        //'copy-oidc-client',
        done);
});
gulp.task('clean', function () {
    del("./wwwroot/dist");
    del("./wwwroot/images/");
    del("./wwwroot/index.html");
    //del("./wwwroot/auth.html");
});

gulp.task("copy-images", function () {
    return gulp.src("./client-src/images/**/*.*")
        .pipe(gulp.dest('./wwwroot/images/'));
});

//gulp.task("copy-oidc-client", function () {
//    return gulp.src("./node_modules/oidc-client/dist/oidc-client.min.js")
//        .pipe(gulp.dest("./wwwroot/dist/"));
//});

//gulp.task("copy-auth", function () {
//    return gulp.src("./client-src/auth.html")
//        .pipe(gulp.dest("./wwwroot/"));
//});


////dev
//gulp.task('get:dev-start', function (done) {
//    runSeq(
//        'clean-Vendor-Js-In-Root',
//        'copy-Vendor-Js-To-Wwwroot-Internal',
//        'copy-js-map',
//        'copy-css',
//        'copy-rename-dev',
//        done);
//});

//// publish 
//gulp.task('get:prod-start', function (done) {
//    runSeq(
//        'clean-Vendor-Js-In-Root',
//        'copy-Vendor-Js-To-Wwwroot-Internal',
//        'bundle:libs',
//        'bundle:app',
//        'copy-js-map',
//        'copy-css',
//        'copy-rename-prod',
//        done);
//});


//gulp.task("copy-images", function () {
//    return gulp.src("./client-src/images/**/*.*")
//        .pipe(gulp.dest('./wwwroot')); 
//});

//gulp.task('clean-Vendor-Js-In-Root', function () {
//    return del(buildConfig.rootJsFolder);
//});

//gulp.task('copy-Vendor-Js-To-Wwwroot-Internal', function (done) {
//    runSeq(
//          'copy-angular',
//          'copy-rxjs',
//          'copy-ngx-uploader',
//          'copy-allOther',
//          done);
//});

//gulp.task('copy-rename-dev', function (done) {
//    runSeq(
//        "copy-index-dev",
//        "rename-index-dev",
//    done);
//});
//gulp.task('copy-index-dev', function () {
//    return gulp.src("index.dev.html")
//    .pipe(gulp.dest('wwwroot'));
//});

//gulp.task('rename-index-dev', function () {
//    return gulp.src("wwwroot/index.dev.html")
//    .pipe(rename('index.html'))
//    .pipe(gulp.dest("wwwroot"));
//});

//gulp.task('copy-rename-prod', function (done) {
//    runSeq(
//        "copy-index-prod",
//        "rename-index-prod",
//    done);
//});
//gulp.task('copy-index-prod', function () {
//    return gulp.src("index.prod.html")
//    .pipe(gulp.dest('wwwroot'));
//});

//gulp.task('rename-index-prod', function () {
//    return gulp.src("wwwroot/index.prod.html")
//    .pipe(rename('index.html'))
//    .pipe(gulp.dest("wwwroot"));
//});

//gulp.task('copy-angular', function () {
//    return gulp.src(buildConfig.sources.angularRC)
//        .pipe(gulp.dest(buildConfig.rootJsFolder + "@angular/"));
//});

//gulp.task('copy-ngx-uploader', function () {
//    return gulp.src(buildConfig.sources.ngx_uploader)
//    .pipe(gulp.dest(buildConfig.rootJsFolder + "ngx-uploader/"));
//});

//gulp.task('copy-rxjs', function () {
//    return gulp.src(buildConfig.sources.Rxjs)
//        .pipe(gulp.dest(buildConfig.rootJsFolder + "rxjs/"));
//});

//gulp.task('copy-allOther', function () {
//    return gulp.src(buildConfig.sources.jsFilesInclSourcePaths)
//       .pipe(gulp.dest(buildConfig.rootJsFolder));
//});

//gulp.task('copy-js-map', function () {
//    return gulp.src(buildConfig.sources.jsMapFilesInclSourcePaths)
//    .pipe(gulp.dest(buildConfig.rootJsFolder));
//})

//gulp.task("copy-css", function () {
//    return gulp.src(buildConfig.sources.cssfiles)
//        .pipe(gulp.dest(buildConfig.rootCssFolder));
//})

//// Bundle dependencies into vendors file
//gulp.task('bundle:libs', function () {
//    return gulp.src(buildConfig.sources.jsFilesInclSourcePaths)
//        .pipe(concat('vendors.min.js'))
//        .pipe(uglify())
//        .pipe(gulp.dest(buildConfig.rootJsFolder));
//});

//// Compile TypeScript to JS
//gulp.task('compile:ts', function () {
//    return gulp
//      .src([
//          "src/**/*.ts",
//          "typings/*.d.ts"
//      ])
//      .pipe(sourcemaps.init())
//      .pipe(tsc({
//          "module": "system",
//          "moduleResolution": "node",
//          "outDir": "public/dist/js",
//          "target": "ES5"
//      }))
//      .pipe(sourcemaps.write('.'))
//      .pipe(gulp.dest('public/dist'));
//});

//// Generate systemjs-based builds
////gulp.task('bundle:app', function () {
////    var builder = new sysBuilder('wwwroot', 'wwwroot/systemjs.config.js');
////    builder.bundle('fileManager & app', 'wwwroot/js/core.min.js');
////    builder.bundle('fileManager - (fileManager & app)', 'wwwroot/js/fileuploader.min.js',
////        { minify: false, mangle: false });
////    return builder.buildStatic('app - (fileManager & app)', 'wwwroot/js/app.min.js',
////        {
////            minify: false, mangle: false
////        });
////});



//// Minify JS bundle already min and uglify
////gulp.task('minify:js', function () {
////    return gulp
////      .src('wwwroot/js/app.min.js')
////      .pipe(uglify())
////      .pipe(gulp.dest(buildConfig.rootJsFolder));
////});

//// bundle and  minify
////gulp.task('bundle-minify', function (done) { runSeq('bundle:js', 'minify:js', done); });

//gulp.task('start:browser-sync', function () {
//    browserSync.init({
//        proxy: "localhost:5000/"
//    });

//    watch("./wwwroot/app/**/*.{html,htm,css,js}", function () {
//        browserSync.reload();
//    });
//    watch("./wwwroot/css/**/*.{html,htm,css,js}", function () {
//        browserSync.reload();
//    });
//    //watch("./wwwroot/**/*.js", function () {
//    //    browserSync.reload();
//    //});
//    //watch("./wwwroot/**/*.css", function () {
//    //    browserSync.reload();
//    //});
//});