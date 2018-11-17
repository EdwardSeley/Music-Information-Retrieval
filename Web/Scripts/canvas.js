let sheets = Array();
let currentSheet = 0;
let leftButton = document.querySelector("#leftButton");
let rightButton = document.querySelector("#rightButton");

rightButton.onclick = () => {
    if (currentSheet < sheets.length - 1) {
        sheets[currentSheet].clearSheet()
        sheets[currentSheet+1].drawSheet();
        currentSheet++;
    }
    updateButtons();
}

leftButton.onclick = () => {
    if (currentSheet > 0) {
        sheets[currentSheet].clearSheet()
        sheets[currentSheet-1].drawSheet();
        currentSheet--;
    }
    updateButtons();
}

function updateButtons() {
    if (currentSheet >= sheets.length - 2)
        rightButton.style.opacity = "0.35";
    else
        rightButton.style.opacity = "1";
    
    if (currentSheet == 0)
        leftButton.style.opacity = "0.35";
    else
        leftButton.style.opacity = "1";
}

function processData(data)
{
    let rows = data.split('\n');
    let sheetIndex = 0;
    for (sheetIndex = 0; sheetIndex < rows.length / 100; sheetIndex++) {
        sheets.push(new MusicSheet(rows.slice(sheetIndex * 100, sheetIndex * 100 + 100)));
    }
    if (rows.length % 100 != 0)
        sheets.push(new MusicSheet(rows.slice((sheetIndex-1) * 100, (sheetIndex-1) * 100 + rows.length % 100)));
    sheets[0].drawSheet();
    updateButtons();
}

function getCSVFileName() {
    let httpReq = new XMLHttpRequest();
    httpReq.onreadystatechange = function() { 
        if (httpReq.readyState == 4 && httpReq.status == 200) {
            loadCSV(httpReq.responseText);
        }
    }
    httpReq.open("GET", "/CSV-FILE", true);
    httpReq.send(null);
}

function loadCSV(fileName) {
    $(document).ready(function () {
        $.ajax({
                type: "GET",
                url: fileName,
                dataType: "text",
                success: function(data) {processData(data);}
        });
    });
}

getCSVFileName();


