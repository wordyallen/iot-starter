const { resolve } = require('path')
const request = require('sync-request')
const res = request('get', 'https://raw.githubusercontent.com/iot-course/org/master/eslintOptions.json')
const options = JSON.parse(res.getBody('utf8'))


module.exports = ({ dev }) => ({
    mode: dev ? 'development' : 'production',
    entry: './index.js',
    devServer: {
        stats: 'minimal',
        contentBase: resolve('web'),
        host: '0.0.0.0',
    },
    output: {
        filename: 'bundle.js',
        path: resolve(__dirname, 'web'),
        publicPath: '/'
    },
    devtool: 'source-map',
    module: {
        rules: [
            { test: /\.js$/, loader: 'babel-loader', exclude: /node_modules/ },
            {test: /\.js$/, loader:'eslint-loader', exclude: /node_modules/, options},
            { test: /\.(gif|jpe?g|png|svg|eot)/, loader: "file-loader" },
            {
                test: /\.ttf/,
                loader: "file-loader",
                include: resolve(__dirname, "node_modules/react-native-vector-icons"),
            },
        ]
    },
    resolve: {
        alias: {
            'react-native': 'react-native-web',
            'react-native-vector-icons/FontAwesome': 'react-native-vector-icons/dist/FontAwesome'
        }
    }

})
