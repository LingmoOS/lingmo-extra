{
output=function(message){
	var output = document.getElementById("StateResult");
	output.innerHTML = output.innerHTML + message + "\n";
},
getJsonFromInfo=function(info){
	if(info == ""){
		return "{}";
	}
	var info_list = info.split("&");
	var info_json = "{";
	for(var i=0; i<info_list.length;i++){
		var key_value_list = info_list[i].split(":");
		var key = key_value_list[0];
		var value = key_value_list[1];
		if(i==0){
			info_json += "\"" + key + "\"" + ":" + "\"" + value + "\"";
		}else{
			info_json += ",\"" + key + "\"" + ":" + "\"" + value + "\"";
		}
	}
	info_json += "}";
	return info_json;	
},
isEmptyObject=function(obj){
	for (var key in obj) {
		return false;
	}
	return true;
},
selfcheck={
	scConnectedCB:function(){},
	connected:function(callback){
        scConnectedCB = callback;
		var socket = new WebSocket("ws://localhost:12345");
		socket.onopen = function(){
			//output("socket opened");
			new QWebChannel(socket,function(channel){
				scobject = channel.objects.indexpage;
				if(typeof(scConnectedCB) != "undefined"){
					scConnectedCB("ok");
				}
			});
		};
        socket.onclose = function(){
            scConnectedCB("socket close");
        };
        socket.onerror = function(error){
            scConnectedCB(error);
        };
	},
	ShowSelfCheckResultOfAll:function(callback){
		scobject.selfcheck_all_result(callback);
	},
	SelfCheck_DoCheck_All:function(callback){
		scobject.selfcheck_docheck_all(callback);
	}
},
devicecheck={
	getForm:function(){},

	connected:function(callback){

	var socket = new WebSocket("ws://localhost:12345");
	callback();
    socket.onclose = function()
    {
        console.error("web channel closed");
    };
    socket.onerror = function(error)
    {
        console.error("web channel error: " + error);
    };
    socket.onopen = function()
    {
    	console.log("web channel init success.");
    	
        new QWebChannel(socket, function(channel)
        {
            content =channel.objects.hdprobepage;
            // console.log('-++++++-')
            content.HdList.connect(function(form){
					// console.log('1111111111111111');
					// console.log(form);
					getForm(form);
					// console.log('2222222222');
					}),
            // console.log('---------------');
            content.HdProbeRes.connect(function(check_res){
            	// console.log(check_res)
            	getResult(check_res)
            })
        });
    };
		/*
		if(typeof(kyconfpageobject) != "undefined"){
			alert('123123')
			callback();
		}else{
			cmConnectedCB=callback;
			alert(typeof(kyconfpageobject))
			alert('321321')	
		}*/
	},
	deviceList:function(callback){
		getForm = callback
	},
	result:function(callback){
		getResult = callback
	},
	check_now:function(){
		content.do_hardware_probe()
	}
}

}
