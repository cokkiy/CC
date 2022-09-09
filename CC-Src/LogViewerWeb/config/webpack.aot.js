var webpack = require('webpack');
var webpackMerge = require('webpack-merge');
var ExtractTextPlugin = require('extract-text-webpack-plugin');
var commonConfig = require('./webpack.common.js');
var helpers = require('./helpers');
var ChunkManifestPlugin = require("chunk-manifest-webpack-plugin");
var WebpackChunkHash = require("webpack-chunk-hash");
//const ngToolsWebpack = require('@ngtools/webpack');

console.log('@@@@@@@@@ USING PRODUCTION AOT @@@@@@@@@@@@@@@');
const ENV = process.env.NODE_ENV = process.env.ENV = 'production';

module.exports = webpackMerge(commonConfig, {
    devtool: 'source-map',
    entry: {
        polyfills: './client-src/app/polyfills.ts',
        vendor: './client-src/app/vendor.ts',
        app: "./client-src/app/main-aot.ts",
    },
    output: {
        path: helpers.root('wwwroot', 'dist/'),
        publicPath: '/dist/',
        //filename: '[name].[chunkhash].js',
        //chunkFilename: '[id].chunk.js'
        filename: '[name].js',
        chunkFilename: '[id].chunk.js'
    },

    module: {
        rules: [
            {
                test: /\.ts$/,
                loaders: [
                    {
                        loader: 'awesome-typescript-loader',
                        options: {
                            configFileName: helpers.root('tsconfig-aot.json'),
                            transpileOnly: true,
                        },
                    },                    
                    'angular-router-loader?aot=true&genDir=client-src/aot',
                ]
            }
            //{
            //    test: /\.ts$/,
            //    loaders: [
            //        '@ngtools/webpack',
            //        'angular-router-loader?aot=true&genDir=wwwroot/aot'
            //    ]
            //},
        ]
    },

    plugins: [
        //new ngToolsWebpack.AotPlugin({
        //    tsConfigPath: helpers.root('tsconfig-aot.json'),
        //    entryModule: helpers.root('wwwroot', 'app/app.module#AppModule'),
        //    typeChecking:false
        //}),
        new webpack.NoEmitOnErrorsPlugin(),
        new webpack.optimize.UglifyJsPlugin({ // https://github.com/angular/angular/issues/10618
            compress: {
                warnings: false
            },
            output: {
                comments: false
            },
            sourceMap: true
        }),
        new ExtractTextPlugin('[name].[hash].css'),
        new webpack.DefinePlugin({
            'process.env': {
                'ENV': JSON.stringify(ENV)
            }
        }),
        new webpack.LoaderOptionsPlugin({
            htmlLoader: {
                minimize: true, // workaround for ng2 fasle
                debug: false
            }
        }),

        new webpack.optimize.CommonsChunkPlugin({
            name: ['app', 'uploader', 'vendor', 'polyfills']
        }),

        // the follow code will cause delay load can't work
        //new webpack.optimize.CommonsChunkPlugin({
        //    name: ['app', 'uploader', 'vendor', 'polyfills','manifest']
        //}),

        //new webpack.HashedModuleIdsPlugin(),
        //new WebpackChunkHash(),
        //new ChunkManifestPlugin({
        //    filename: "chunk-manifest.json",
        //    manifestVariable: "webpackManifest"
        //})

    ]
});