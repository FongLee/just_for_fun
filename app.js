var express = require('express');

var app = express();

app.get('/index.html', function(req, res) {
	res.sendfile('./index.html');
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

app.get('/html/20180711-localStorage-test.html', function(req, res) {
	res.sendfile('./html/20180711-localStorage-test.html');
});
	

app.get('/html/20180713-read-localStorage.html', function(req, res) {
	res.sendfile('./html/20180713-read-localStorage.html');
});

var server = app.listen(3000, function() {

});