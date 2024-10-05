function ShieldF1() {

    window.onhelp = function () { return false }; 
    document.onkeydown=function (event) {
    var key=event.keyCode;
        if(key==112) {
            event.preventDefault(); 
        }
       
    }
}

ShieldF1()

