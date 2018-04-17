var webpack = require('webpack');
var webpackMerge = require('webpack-merge');
var ExtractTextPlugin = require('extract-text-webpack-plugin');
var commonConfig = require('./webpack.common.js');
var helpers = require('./helpers');
var ChunkManifestPlugin = require("chunk-manifest-webpack-plugin");
var WebpackChunkHash = require("webpack-chunk-hash");

console.log('@@@@@@@@@ USING PRODUCTION @@@@@@@@@@@@@@@');
const ENV = process.env.NODE_ENV = process.env.ENV = 'production';

module.exports = webpackMerge(commonConfig, {
    devtool: 'source-map',
    entry: {
        polyfills: './client-src/app/polyfills.ts',
        vendor: './client-src/app/vendor.ts',
        app: "./client-src/app/main.ts",
    },
    output: {
        path: helpers.root('wwwroot', 'dist/'),
        publicPath: '/dist/',
        //filename: '[name].[chunkhash].js',
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
                            configFileName: helpers.root('tsconfig.json'),
                            transpileOnly: true,
                        },
                    },                    
                    'angular-router-loader',
                    'angular2-template-loader?keepUrl=false',
                ]
            }            
        ]
    },

    plugins: [
        new webpack.NoEmitOnErrorsPlugin(),
        new webpack.optimize.UglifyJsPlugin({ // https://github.com/angular/angular/issues/10618
            mangle: {
                keep_fnames: true
            }
        }),
        new ExtractTextPlugin('[name].[hash].css'),
        new webpack.DefinePlugin({
            'process.env': {
                'ENV': JSON.stringify(ENV)
            }
        }),
        new webpack.LoaderOptionsPlugin({
            htmlLoader: {
                minimize: true // workaround for ng2
            }
        }),

        new webpack.optimize.CommonsChunkPlugin({
            name: ['app', 'uploader', 'vendor', 'polyfills']
        }),
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