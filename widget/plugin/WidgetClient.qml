import Qt 4.7
import MeeGo.WidgetInterface 0.1
import "client.js" as WidgetClientJS

Item {
    id: widgetClient

    property string name : ""
    property string type : ""

    //string data for comparing to avoid chaos
    property string lastArrivedData : ""    //currently lastArrivedData = lastSendData or last accepted data
    property string thisArrivedData : ""    //data just arrived
    property string currentData : ""        //data of current states
    property bool enableStartUpHandler: true

    function currentWidgetData()
    {
        return WidgetClientJS.tempWidgetData;
    }

    function setCurrentData(data) {
        WidgetClientJS.tempWidgetData = data;
        updateWidgetData(data);
    }

    function convertDataToTemp() {
        try {
            var targetStringData = "{";
            for(var x = 0; x < WidgetClientJS.widgetData.paths.length; x++) {
                var eventString = "";
                eventString = WidgetClientJS.Obj2JSON(WidgetClientJS.widgetData.paths[x]) + ":"
                        + WidgetClientJS.Obj2JSON(WidgetClientJS.widgetData.data[x]);
                if(x != 0)
                     targetStringData += ","
                targetStringData += eventString
            }
            targetStringData += "}"
            WidgetClientJS.tempWidgetData = WidgetClientJS.JSON2Obj(targetStringData);
        }
        catch(error) {
            console.log("Catch error in convertTempToData()")
            try {
                widgetClient.parent.setDefaultData();
            }
            catch(error) {
                console.log("Catch error in catch(error) in convertTempToData()")
            }
        }
    }

    function updateWidgetData(data) {
        try {
            var orgString = WidgetClientJS.Obj2JSON(data);
            var subString = orgString.substring(1, orgString.length-1);
            var subStringObj = subString.split(",");
            var pathsString = "";
            var dataString = "";
            for(var x = 0; x < subStringObj.length; x++) {
                var sendSubString = subStringObj[x].split("\":");
                if(x != 0) {
                    pathsString += "\",";
                    dataString += ",";
                }
                pathsString += sendSubString[0];
                dataString += sendSubString[1];
            }
            var targetStringData = "{\"paths\":["+pathsString+"\"],\"data\":["+dataString+"]}";
            WidgetClientJS.widgetData = WidgetClientJS.JSON2Obj(targetStringData);
        }
        catch(error) {
            console.log("Catch error in updateWidgetData()")
        }
    }

    function getData(path) {
        var pathIndex = -1;
        for(var x in WidgetClientJS.widgetData.paths)
        {
            if(WidgetClientJS.widgetData.paths[x].indexOf(path)==0 && path.indexOf(WidgetClientJS.widgetData.paths[x])==0)
                pathIndex = x;
        }
        if(pathIndex == -1) {
            pathIndex = WidgetClientJS.widgetData.paths.length;
            WidgetClientJS.widgetData.paths.push(path);
        }
        return WidgetClientJS.widgetData.data[pathIndex];
    }

    function setData(path, data) {
        var pathIndex = -1;
        for(var x in WidgetClientJS.widgetData.paths)
        {
            if(WidgetClientJS.widgetData.paths[x].indexOf(path)==0 && path.indexOf(WidgetClientJS.widgetData.paths[x])==0)
                pathIndex = x;
        }
        if(pathIndex == -1) {
            pathIndex = WidgetClientJS.widgetData.paths.length;
            WidgetClientJS.widgetData.paths.push(path);
        }
        WidgetClientJS.widgetData.data[pathIndex] = data;
    }

    function startup() {
        WidgetClientJS.initRequestInfo();
    }

    function shootData(mode) {
        convertDataToTemp();
        lastArrivedData = WidgetClientJS.Obj2JSON(WidgetClientJS.tempWidgetData);
        if(mode) {
            if(mode == "force")
                widgetHttpAddress.sendDataMode = "force"
            WidgetClientJS.sendDataRequest(lastArrivedData);
            widgetHttpAddress.sendDataMode = "none"
        }
        else
            WidgetClientJS.sendDataRequest(lastArrivedData);
    }

    WidgetAddress {
        id: widgetHttpAddress
        name: widgetClient.name
        type: widgetClient.type
    }
}
