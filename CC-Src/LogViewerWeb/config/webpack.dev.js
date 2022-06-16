"use strict";
var webpack = require('webpack');
var webpackMerge = require('webpack-merge');
var ExtractTextPlugin = require('extract-text-webpack-plugin');
var commonConfig = require('./webpack.common.js');
var helpers = require('./helpers');
var browserSyncPlugin = require('browser-sync-webpack-plugin');  
console.log('@@@@@@@@@ USING DEVELOPMENT @@@@@@@@@@@@@@@');

module.exports = webpackMerge(commonConfig, {
    devtool: 'cheap-module-eval-source-map',
    entry: {
        polyfills: './client-src/app/polyfills.ts',
        vendor: './client-src/app/vendor.ts',
        app: "./client-src/app/main.ts",
    },
    output: {
        path: helpers.root('wwwroot','dist/'),
        publicPath: '/dist/',
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
        new ExtractTextPlugin('[name].css'),
        new browserSyncPlugin({
            // browse to http://localhost:3000/ during development
            host: 'localhost',
            port: 3000, //代理后访问的端口
            proxy: 'localhost:3500',//要代理的端口
        },
            // plugin options
            {
                // prevent BrowserSync from reloading the page
                // and let Webpack Dev Server take care of this
                reload: false
            }),
        new webpack.optimize.CommonsChunkPlugin({
            name: ['app', 'uploader', 'vendor', 'polyfills']
        }),

    ],

    watchOptions: {
        aggregateTimeout: 300,
        poll: 1000,
        ignored: ["../node_modules/","../client-src/app/**/*.js"],
    },

    devServer: {
        historyApiFallback: true,
        contentBase: helpers.root("wwwroot"),
        compress: true,
        port: 3500,
        inline: true,
        proxy: {
            "/api": {
                target: "http://localhost:5000/"
            }
        }
    }
});