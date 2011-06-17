import Qt 4.7
import AcerWidgetsDaemonInterface 0.1
import "awd-client.js" as AwdClientJS

Item {
    id: awdClient

    property string name : ""
    property string type : ""

    //string data for comparing to avoid chaos
    property string lastArrivedData : ""    //currently lastArrivedData = lastSendData or last accepted data
    property string thisArrivedData : ""    //data just arrived
    property string currentData : ""        //data of current states

    function currentAwdData()
    {
        return AwdClientJS.tempAwdData;
    }

    function setCurrentData(data) {
        AwdClientJS.tempAwdData = data;
        updateAwdData(data);
    }

    function convertDataToTemp() {
        try {
            var targetStringData = "{";
            for(var x = 0; x < AwdClientJS.awdData.paths.length; x++) {
                var eventString = "";
                eventString = AwdClientJS.Obj2JSON(AwdClientJS.awdData.paths[x]) + ":"
                        + AwdClientJS.Obj2JSON(AwdClientJS.awdData.data[x]);
                if(x != 0)
                     targetStringData += ","
                targetStringData += eventString
            }
            targetStringData += "}"
            AwdClientJS.tempAwdData = AwdClientJS.JSON2Obj(targetStringData);
        }
        catch(error) {
            console.log("Catch error in convertTempToData()")
            try {
                awdClient.parent.setDeafultData();
            }
            catch(error) {
                console.log("Catch error in catch(error) in convertTempToData()")
            }
        }
    }

    function updateAwdData(data) {
        try {
            var orgString = AwdClientJS.Obj2JSON(data);
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
            AwdClientJS.awdData = AwdClientJS.JSON2Obj(targetStringData);
        }
        catch(error) {
            console.log("Catch error in updateAwdData()")
        }
    }

    function getData(path) {
        var pathIndex = -1;
        for(var x in AwdClientJS.awdData.paths)
        {
            if(AwdClientJS.awdData.paths[x].indexOf(path)==0 && path.indexOf(AwdClientJS.awdData.paths[x])==0)
                pathIndex = x;
        }
        if(pathIndex == -1) {
            pathIndex = AwdClientJS.awdData.paths.length;
            AwdClientJS.awdData.paths.push(path);
        }
        return AwdClientJS.awdData.data[pathIndex];
    }

    function setData(path, data) {
        var pathIndex = -1;
        for(var x in AwdClientJS.awdData.paths)
        {
            if(AwdClientJS.awdData.paths[x].indexOf(path)==0 && path.indexOf(AwdClientJS.awdData.paths[x])==0)
                pathIndex = x;
        }
        if(pathIndex == -1) {
            pathIndex = AwdClientJS.awdData.paths.length;
            AwdClientJS.awdData.paths.push(path);
        }
        AwdClientJS.awdData.data[pathIndex] = data;
    }

    function startup() {
        AwdClientJS.initRequestInfo();
    }

    function shootData() {
        convertDataToTemp();
        lastArrivedData = AwdClientJS.Obj2JSON(AwdClientJS.tempAwdData);
        AwdClientJS.sendDataRequest(lastArrivedData);
    }

    AwdAddress {
        id: awdHttpAddress
        name: awdClient.name
        type: awdClient.type
    }
}
