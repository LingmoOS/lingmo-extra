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
configmanager={
	cmConnectedCB:function(){},
	init:function(){
		var socket = new WebSocket("ws://localhost:12345");
		socket.onopen = function(){
			//output("socket opened");
			new QWebChannel(socket,function(channel){
				cmobject = channel.objects.configmanager;
				if(typeof(cmConnectedCB) != "undefined"){
					cmConnectedCB();
				}
			});
		};
	},
	connected:function(callback){
		if(typeof(cmobject) != "undefined"){
			callback();
		}else{
			cmConnectedCB=callback;	
		}
	},
	getConf:function(sectionname,callback){
		cmobject.getconf(sectionname,callback);	
	},
	setConf:function(sectionname,data,callback){
		//cmobject.setconf(sectionname,JSON.stringify(data),callback);
		cmobject.setconf(sectionname,data,function(result){
            callback(result.toString());
        });
	},
    reboot:function(isoverlay,callback){
        cmobject.reboot(isoverlay,callback);
    }
},
confpage={
	getForm:function(){},
	confpageForm:function(callback){
		console.log('CALLTRACE : confpageForm Is Running.');
		getForm=callback;
	},
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
            content =channel.objects.confpage;
            // console.log('-++++++-')

            content.ConfpageForm.connect(function(form){

					// console.log('1111111111111111');
					console.log(form);
					getForm(form);
					// console.log('2222222222');
					})
            console.log('---------------');
        });
    };
	},
	commitForm:function(form){
		content.HandleConfPage(form)
	}
}
}
// confpage.init();
configmanager.init();
