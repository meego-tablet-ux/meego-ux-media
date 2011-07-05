var widgetData = {
    "paths" : [],
    "data" : []
    }

var tempWidgetData = {};

//basic functions for handling XMLHttpRequest
var xmlHttpStartUpRequest,
    xmlHttpStandByRequest,
    xmlHttpSendDataRequest;

function initRequestInfo()
{
    xmlHttpStartUpRequest=new XMLHttpRequest();
    xmlHttpStartUpRequest.open("Get", widgetHttpAddress.getAddress("StartUp"), true);
    xmlHttpStartUpRequest.onreadystatechange = handleStartUpResponse;
    xmlHttpStartUpRequest.send(null);
}

function standByRequest()
{
    xmlHttpStandByRequest=new XMLHttpRequest();
    xmlHttpStandByRequest.open("Get", widgetHttpAddress.getAddress("StandBy"), true);
    xmlHttpStandByRequest.onreadystatechange = handleStandByResponse;
    xmlHttpStandByRequest.send(null);
}

function sendDataRequest(data)
{
    xmlHttpSendDataRequest=new XMLHttpRequest();
    xmlHttpSendDataRequest.open("Get", widgetHttpAddress.getAddress("SendData", data), true);
    xmlHttpSendDataRequest.onreadystatechange = handleSendDataResponse;
    xmlHttpSendDataRequest.send(null);
}

//converting data between JSON and object
function JSON2Obj(JSONData) {
    var myObject = JSON.parse(JSONData);
    return myObject;
}

function Obj2JSON(myObject) {
    var data = JSON.stringify(myObject, replacer);
    return data
}

function replacer(key, value) {
    if (typeof value === 'number' && !isFinite(value)) {
        return String(value);
    }
    return value;
}

function handleStandByResponse()
{
    if(xmlHttpStandByRequest.readyState == 4)
    {
        if (xmlHttpStandByRequest.status == 200)
        {
            var standByResponseData = xmlHttpStandByRequest.responseText;
            handleStandByData(standByResponseData);            
        }
    }
}

function handleSendDataResponse()
{
    if(xmlHttpSendDataRequest.readyState == 4)
    {
        if (xmlHttpSendDataRequest.status == 200)
        {
            var sendDataResponseData = xmlHttpSendDataRequest.responseText;
            handleSendData(sendDataResponseData);
        }
    }
}

function handleStartUpResponse()
{
    if(xmlHttpStartUpRequest.readyState == 4)
    {
        if (xmlHttpStartUpRequest.status == 200)
        {
            var startUpResponseData = xmlHttpStartUpRequest.responseText;
            handleStartUpData(startUpResponseData);
        }
    }
}

function handleSendData(data)
{    
    xmlHttpSendDataRequest = undefined;
}

function handleStartUpData(data)
{
    widgetClient.currentData = Obj2JSON(tempWidgetData);
    try {
        var recoverTest = JSON.parse(data);
        if(recoverTest.currentContext) {
            data = recoverTest.currentContext.data;
        }
        if(recoverTest.anotherContext) {
            data = recoverTest.anotherContext.data;
        }
        if(data === "" || !data)
            data = widgetClient.currentData;
        console.log("My data : "+data+"\n");
    }
    catch(error) {
        console.log("Catch error... set back to default value~~~\n")
        data = widgetClient.currentData;
    }
    standByRequest();

    widgetClient.thisArrivedData = data;

    if(widgetClient.lastArrivedData == widgetClient.thisArrivedData && widgetClient.thisArrivedData != widgetClient.currentData) {
        console.log("[app::handleStandByData]: Data received is out-of-date. Do nothing!!\n");
    }
    else if(widgetClient.thisArrivedData == widgetClient.currentData) {
        console.log("[app::handleStandByData]: Data received is as same as current one. Do nothing!!\n");
    }
    else if(widgetClient.lastArrivedData != widgetClient.thisArrivedData && widgetClient.thisArrivedData != widgetClient.currentData) {

        widgetClient.lastArrivedData = widgetClient.thisArrivedData;
        console.log("[app::handleStandByData]: " + data);

        //Parse your DATA here.
        tempWidgetData = JSON2Obj(data);
        updateWidgetData(tempWidgetData);
        if(widgetClient.enableStartUpHandler) {
            widgetClient.parent.startUpControlHandle();
            widgetClient.enableStartUpHandler = false;
        }
    }
    xmlHttpStartUpRequest = undefined;
}

function handleStandByData(data)
{
    xmlHttpStandByRequest = undefined;
    standByRequest();
    widgetClient.currentData = Obj2JSON(tempWidgetData);
    try {
        var recoverTest = JSON.parse(data);
        if(recoverTest.currentContext) {
            data = recoverTest.currentContext.data;
        }
        if(recoverTest.anotherContext) {
            data = recoverTest.anotherContext.data;
        }
        if(data === "" || !data)
            data = widgetClient.currentData;
        console.log("My data : "+data+"\n");
    }
    catch(error) {
        console.log("Catch error... set back to default value~~~\n")
        data = widgetClient.currentData;
    }

    widgetClient.thisArrivedData = data;

    if(widgetClient.lastArrivedData == widgetClient.thisArrivedData && widgetClient.thisArrivedData != widgetClient.currentData) {
        console.log("[app::handleStandByData]: Data received is out-of-date. Do nothing!!\n");
    }
    else if(widgetClient.thisArrivedData == widgetClient.currentData) {
        console.log("[app::handleStandByData]: Data received is as same as current one. Do nothing!!\n");
    }
    else if(widgetClient.lastArrivedData != widgetClient.thisArrivedData && widgetClient.thisArrivedData != widgetClient.currentData) {

        widgetClient.lastArrivedData = widgetClient.thisArrivedData;
        console.log("[app::handleStandByData]: " + data);

        //Parse your DATA here.
        tempWidgetData = JSON2Obj(data);
        updateWidgetData(tempWidgetData);
        widgetClient.parent.controlHandle();
    }
}
