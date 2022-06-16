function buildConfig(env) {
    if (env === 'production') {
        return require('./config/webpack.prod.js');
    }
    else if (env == "aot") {
        return require('./config/webpack.aot.js');
    }
    else {
        return require('./config/webpack.dev.js');
    }
}
module.exports = buildConfig;