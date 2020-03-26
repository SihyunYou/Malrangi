var objIE = WScript.CreateObject("InternetExplorer.Application");
objIE.navigate("http://192.168.0.1");
objIE.visible = true;     

while(objIE.busy);
while(objIE.Document.readyState != "complete");

objIE.document.getElementsByName('username')[0].value = "root";
objIE.document.getElementsByName('passwd')[0].value = "root";
objIE.document.forms["form"].submit();

while(objIE.busy);
while(objIE.Document.readyState != "complete");

objIE.navigate("http://192.168.0.1/sess-bin/timepro.cgi?tmenu=netconf&smenu=wansetup");

while(objIE.busy);
while(objIE.Document.readyState != "complete");

objIE.document.getElementsByName('hw_dynamic1')[0].value = 48;
objIE.document.getElementsByName('hw_dynamic2')[0].value = 45;
objIE.document.getElementsByName('hw_dynamic3')[0].value = 20;
objIE.document.getElementsByName('hw_dynamic4')[0].value = 81;
objIE.document.getElementsByName('hw_dynamic5')[0].value = 37;
objIE.document.getElementsByName('hw_dynamic6')[0].value = WScript.arguments(0);
objIE.document.getElementById('appbtn').click();

while(objIE.busy);
while(objIE.Document.readyState != "complete");

function sleep (delay) {
   var start = new Date().getTime();
   while (new Date().getTime() < start + delay);
}

sleep(10000);

objIE.navigate("http://192.168.0.1/sess-bin/login_session.cgi?logout=1");
objIE.quit();