String red_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEADb2VU/oOEhJ/cYUSXgwvAFKErQNYEVoxLIUwjTANpigmZimw6I0mKaedmmJuIDjrksCUqUtAjAxsfAG6/KAyiiHWSAAAAAElFTkSuQmCC";
String green_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEADcs/C/oOEHkmtYkSXgwvAFKErQNYEVoxLIUwjTANpigmZimw6I0mKaedmmJuIDjrksCUqUtAjAxsfAJ6PKAznF8FZAAAAAElFTkSuQmCC";
String yellow_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEAD/0/y/QcJMZp/YkSXgwvAFGEoQNIEVoxLIUwjzBbSFBMyFdl0RpIU087NMDcRHXTIYUtUpKBHBjY+ACJeKAzktf86AAAAAElFTkSuQmCC";

String Header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
/*
String homePageStr = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:100%;} 
    #main {display: table; margin: auto;  padding: 10px 10px 10px 10px; } 
    #content { border: 5px solid blue; border-radius: 15px; padding: 10px 0px 10px 0px;}
    #commands { border: 3px solid yellow; border-radius: 9px; padding: 5px 0px 5px 0px;}
    #Axis {  border: 3px solid green; border-radius: 5px; padding: 5px 0px 5px 0px;}
    h2 {text-align:center; margin: 10px 0px 10px 0px;} 
    p { text-align:center; margin: 5px 0px 10px 0px; font-size: 120%;}
    td { text-align:center; font-size: 120%;}
    #time_P { margin: 10px 0px 15px 0px;}
  </style>
 
  <script> 
    function updateTime() 
    {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;
    }
    //Load the decapper ID from its IP address
    function getDecapperID()
    {
      var ip = location.host;
      var tmpArray = ip.split(".");
      document.getElementById('DECAP_ID').innerHTML = tmpArray[tmpArray.length-1]-100;
    }
    function updateDecapperInfo()
    {
      ajaxLoad('updateDecapperInfo');
    }
    
    var ajaxRequest = null;
    if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
    else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }
 
    function ajaxLoad(ajaxURL)
    {
      if(!ajaxRequest){ alert('AJAX is not supported.'); return; }
 
      ajaxRequest.open('GET',ajaxURL,true);
      ajaxRequest.onreadystatechange = function()
      {
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
        {
          var ajaxResult = ajaxRequest.responseText;
          var tmpArray = ajaxResult.split("|");
          document.getElementById('ZPos').innerHTML = tmpArray[0];
          document.getElementById('ZTarget').innerHTML = tmpArray[1];
          document.getElementById('ZEncPos').innerHTML = tmpArray[2];
          document.getElementById('ZEncDev').innerHTML = tmpArray[3];
          document.getElementById('ZStatus').style.backgroundColor = tmpArray[4]?'lightgreen':'red';
          document.getElementById('MPos').innerHTML = tmpArray[5];
          document.getElementById('MTarget').innerHTML = tmpArray[6];
          document.getElementById('MEncPos').innerHTML = tmpArray[7];
          document.getElementById('MEncDev').innerHTML = tmpArray[8];
          document.getElementById('MStatus').style.backgroundColor = tmpArray[9]?'lightgreen':'red';
          document.getElementById('CPos').innerHTML = tmpArray[10];
          document.getElementById('CTarget').innerHTML = tmpArray[11];
          document.getElementById('CEncPos').innerHTML = tmpArray[12];
          document.getElementById('CEncDev').innerHTML = tmpArray[13];
          document.getElementById('CStatus').style.backgroundColor = tmpArray[14]?'lightgreen':'red';
        }
      }
      ajaxRequest.send();
    }
 
    var myVar1 = setInterval(updateDecapperInfo, 2000);  
    var myVar2 = setInterval(updateTime, 1000);  
    window.onload = getDecapperID;
  </script>

    <title>Decapper <span id='DECAP_ID'>ID</span> UI </title>
 </head>
 
 <body>
   <div id='main'>
     <h2>Decapper <span id='DECAP_ID'>ID</span> UI </h2>
      <p id='P_time'>Time</p>
     <div id='content'> 
     <div id='Axis'>
     <p style="background-color:lightgreen">
        <strong>Axis Z : </strong>ZPos : <span id='ZPos'>Position</span> ZTarget : <span id='ZTarget'>Target</span>
        Encoder Z : <span id='ZEncPos'>Position</span> Encoder Dev : <span id='ZEncDev'>Deviation</span>
     </p>
     </div>
     <div id='Axis'>
     <p>
        Axis M : MPos : <span id='MPos'>Position</span> MTarget : <span id='MTarget'>Target</span>
        Encoder M : <span id='MEncPos'>Position</span> Encoder Dev : <span id='MEncDev'>Deviation</span>
     </p>
     </div>
     <div id='Axis'>
     <p>
        Axis C : CPos : <span id='CPos'>Position</span> CTarget : <span id='CTarget'>Target</span>
        Encoder C : <span id='CEncPos'>Position</span> Encoder Dev : <span id='CEncDev'>Deviation</span>
     </p>
     </div>
     </div>
  <div id='commands'> 
  <p>
    <a href="http://192.168.1.101/showTagList">showTagList</a><br />
  </p>
   </div> 
 </body>
</html>
  )=====";
  */
