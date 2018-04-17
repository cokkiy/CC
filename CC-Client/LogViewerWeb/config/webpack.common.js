"use strict";
var webpack = require("webpack");
var HtmlWebpackPlugin = require("html-webpack-plugin");
var ExtractTextPlugin = require("extract-text-webpack-plugin");
var helpers = require("./helpers");

module.exports = {
  //context: helpers.root('client-src'),
  resolve: {
    extensions: [".ts", ".js", "css", "html"]
  },
  module: {
    rules: [
      {
        enforce: "pre",
        test: /\.(js|tsx?)$/,
        exclude: [
          // workaround for this issue
          helpers.root("node_modules", "@angular/compiler")
        ],
        loader: "source-map-loader"
      },
      {
        test: /\.html$/,
        loader: "raw-loader"
      },
      {
        test: /\.(png|jpe?g|gif|svg|woff|woff2|ttf|eot|ico)$/,
        use: {
          loader: "file-loader",
          options: {
            name: "[name].[ext]"
          }
        }
      },
      {
        test: /\.scss$/,
        exclude: helpers.root("client-src", "app"),
        loader: ExtractTextPlugin.extract({
          fallback: "style-loader",
          use: "css-loader?sourceMap!sass-loader"
        })
      },
      {
        test: /\.scss$/,
        include: helpers.root("client-src", "app"),
        use: ["raw-loader", "sass-loader"]
      },
      {
        test: /\.css$/,
        exclude: helpers.root("client-src", "app"),
        loader: ExtractTextPlugin.extract({
          fallback: "style-loader",
          use: "css-loader?sourceMap"
        })
      },
      {
        test: /\.css$/,
        include: helpers.root("client-src", "app"),
        use: ["raw-loader"]
      }
    ]
  },
  plugins: [
    // Workaround for angular/angular#11580
    new webpack.ContextReplacementPlugin(
      // The (\\|\/) piece accounts for path separators in *nix and Windows
      /angular(\\|\/)core(\\|\/)(esm(\\|\/)src|src)(\\|\/)linker/,
      helpers.root("client-src", "app"), // location of your src
      {} // a map of your routes
    ),

    // make $ to globale
    new webpack.ProvidePlugin({
      $: "jquery",
      jquery: "jquery",
      "window.jQuery": "jquery",
      jQuery: "jquery",
      echarts: "echarts", // for echart
      Popper: "popper.js"
    }),

    new HtmlWebpackPlugin({
      template: "./client-src/index.html",
      filename: "../index.html"
    })
  ]
};
