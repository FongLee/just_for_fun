var express = require('express');

var app = express();

app.get('/', function(req, res) {
	res.sendfile('./html/20180709-离线存储.html');
});
app.get('/html/20180709-css-import.html', function(req, res) {
	res.sendfile('./html/20180709-mainfest.manifest');
});

app.get('/html/20180709-mainfest.manifest', function(req, res) {
	res.sendfile('./html/20180709-mainfest.manifest');
});

app.get('/html/20180709-import-test.css', function(req, res) {
	res.sendfile('./html/20180709-import-test.css');
});

app.get('/html/20180709-Label.html', function(req, res) {
	res.sendfile('./html/20180709-import-test.css');
});

var server = app.listen(3000, function() {

});