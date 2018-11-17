const fs = require('fs');
const parse = require('csv-parse');
const async = require('async');
const csv = require('fast-csv');
const http = require('http');
const url = require("url");

let resourceRegex = /\/[A-z]+.SVG/;
let CSVRegex = /\/[A-z]+.csv/;
let JSRegex = /\/[A-z]+.js/;

fs.readFile('Web/template.html', (err, data) => {
    if (err)
        console.log(err);
    else
        printHTML(data);
});

function printHTML(htmlContent) {
    http.createServer((req, res) => {
        let pathName = url.parse(req.url).pathname;

        if (pathName == "/") {
            res.writeHead(200, {'Content-Type':'text/html'});
            res.write(htmlContent);
        }
        else if (pathName == "/CSV-FILE") {
            console.log(pathName);
            res.writeHead(200, {'Content-Type':'text'});
        }
        else if (JSRegex.test(pathName)) {
            script = fs.readFileSync("Web/Scripts" + pathName, "utf8");
            res.write(script);
        }
        else if (CSVRegex.test(pathName)) {
            data = fs.readFileSync("Output/" + pathName, "utf8");
            res.write(data);
        }
        else if (resourceRegex.test(pathName)) {
            res.setHeader('Content-Type', 'image/svg+xml');
            data = fs.readFileSync("Resources/Assets/" + pathName);
            res.write(data);
        }
        res.end();
    }).listen(8080);
}

/*
function printRow(row) {
    console.log(row);
    console.log('row printed');
}

let csvStream = csv().on("data", (row) => {
    printRow(row);
});

fs.createReadStream('../Output/sample.csv').pipe(csvStream);
*/