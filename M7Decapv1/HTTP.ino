/*String red_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEADb2VU/oOEhJ/cYUSXgwvAFKErQNYEVoxLIUwjTANpigmZimw6I0mKaedmmJuIDjrksCUqUtAjAxsfAG6/KAyiiHWSAAAAAElFTkSuQmCC";
String green_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEADcs/C/oOEHkmtYkSXgwvAFKErQNYEVoxLIUwjTANpigmZimw6I0mKaedmmJuIDjrksCUqUtAjAxsfAJ6PKAznF8FZAAAAAElFTkSuQmCC";
String yellow_dot = "iVBORw0KGgoAAAANSUhEUgAAAAsAAAALCAYAAACprHcmAAAAAXNSR0IArs4c6QAAAERJREFUKFNjZEAD/0/y/QcJMZp/YkSXgwvAFGEoQNIEVoxLIUwjzBbSFBMyFdl0RpIU087NMDcRHXTIYUtUpKBHBjY+ACJeKAzktf86AAAAAElFTkSuQmCC";
*/
void endConnection(EthernetClient* client_pntr){
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

//This function is called when the decapper can perform the asked action
void answerHttp(EthernetClient* client_pntr,String currentLine){
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  //client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println("<p><a href=\"http://" + StringIP + "/home\">Home</a></p>");
  client_pntr->println();
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

//This function is called when the call won't be performed
void answerHttpNo(EthernetClient* client_pntr,String currentLine, int state){
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  //client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println();
  client_pntr->print("The decapper cannot perform this action now, status=");client_pntr->println(state);
  client_pntr->println("<p><a href=\"http://" + StringIP + "/home\">Home</a></p>");
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

void homePage(EthernetClient* client_pntr){

  int state = getStatus();
  long current_time = millis();
  int seconds = (int) (current_time / 1000) % 60 ;
  int minutes = (int) ((current_time / (1000*60)) % 60);
  int hours   = (int) ((current_time / (1000*60*60)) % 24);
  char c[30];
  int l = sprintf(c, "%02d:%02d:%02d",hours,minutes,seconds);
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->println("<!DOCTYPE HTML>");
  client_pntr->println("<html>");
  client_pntr->println("<body>");
  client_pntr->print("<h1 style=\"text-align:center\">Decapper ");client_pntr->print(DECAP_ID);client_pntr->println("</h1>");
  client_pntr->print("<p>Current Status : ");client_pntr->print(state);
  client_pntr->print("<img src=\"data:image/png;base64," + (state==3 ? yellow_dot:(state<2 ?green_dot:red_dot)) + "\" alt=\"Red dot\" />");
  client_pntr->println("</p>");
  client_pntr->print("<p style=\"text-align:right\">");
  client_pntr->print("<strong> Z position : ");client_pntr->print(ZPos);client_pntr->print("</strong> Target : ");client_pntr->print(ZTarget);
  client_pntr->print((Zstate ?String("<span style=\"background-color:#00ff00;\">"):String("<span style=\"background-color:#ff0000;\">")) + "Encoder : </span>");client_pntr->print(ZPosEnc);
  client_pntr->print(" Angle : ");client_pntr->print(ZAngleEnc);
  client_pntr->print(" Turn : ");client_pntr->print(ZTurnEnc);
  client_pntr->print(" Deviation : ");client_pntr->print(ZDevEnc);
  client_pntr->print("<br /><strong> M position : ");client_pntr->print(MPos);client_pntr->print("</strong> Target : ");client_pntr->print(MTarget);
  client_pntr->print((Mstate ?String("<span style=\"background-color:#00ff00;\">"):String("<span style=\"background-color:#ff0000;\">")) + "Encoder : </span>");client_pntr->print(MPosEnc);
  client_pntr->print(" Angle : ");client_pntr->print(MAngleEnc);
  client_pntr->print(" Turn : ");client_pntr->print(MTurnEnc);
  client_pntr->print(" Deviation : ");client_pntr->print(MDevEnc);
  client_pntr->print("<br /><strong> C position : ");client_pntr->print(CPos);client_pntr->print("</strong> Target : ");client_pntr->print(CTarget);
  client_pntr->print((Cstate ?String("<span style=\"background-color:#00ff00;\">"):String("<span style=\"background-color:#ff0000;\">")) + "Encoder : </span>");client_pntr->print(CPosEnc);
  client_pntr->print(" Angle : ");client_pntr->print(CAngleEnc);
  client_pntr->print(" Turn : ");client_pntr->print(CTurnEnc);
  client_pntr->print(" Deviation : ");client_pntr->print(CDevEnc);
  client_pntr->println("</p>");
  client_pntr->print("<p>Time since last reset : " + String(c));client_pntr->println("</p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/initialize\">Initialize</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/decap\">Decap</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/recap\">Recap</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/capture\">Capture</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/decapperStatus\">Status</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/reset\">Reset</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/stopAllMoves\">Stop</a></p>");
  client_pntr->println("<p> Other functions : </p>");
  client_pntr->println("<p> Set VMAX : /15000ZsetVMAX set Z speed to 15000 Usteps/s = 0.3 RPS 0-" + String(VMAX_LIMIT) + "</p>");
  client_pntr->println("<p> Set Current Scaling : /25MsetCurrentScaling set M current scaling value to 25 0-" +String(CS_LIMIT) + "</p>");
  client_pntr->println("<p> Relative move : /+52000ZrelMove Z axis will perform a relative move of 52000 Usteps // limit=" + String(RELMOVE_LIMIT) + "</p>");
  client_pntr->println("</body>");
  client_pntr->println("</html>");
  
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
//This function returns the status of the decapper
//There is X possible status
//0 : decapper out of order
//1 : decapper currently working
//2 : decapper free (ready for decap)
//3 : decapper used (ready for recap)
//4 : decapper not initialised

void statusHttp(EthernetClient* client_pntr,String currentLine){
  int state = getStatus();
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  //client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->print("status=");client_pntr->println(state);
  if(state >128) {
    client_pntr->print("Decapper in error State ErrorCode=");client_pntr->println(state);
    if(state&0b00000010)client_pntr->println("Error During Init");
    if(state&0b00000100)client_pntr->println("Error During Decap");
    if(state&0b00001000)client_pntr->println("Error During Recap");
    if(state&0b01000000)client_pntr->println("Error on Axis Z");
    if(state&0b00100000)client_pntr->println("Error on Axis M");
    if(state&0b00010000)client_pntr->println("Error on Axis C");
  }
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  delay(10);
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

void answerHttpCapture(EthernetClient* client_pntr,String currentLine){
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  //client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println("<p><a href=\"http://" + StringIP + "/home\">Home</a></p>");
  client_pntr->println();
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());

  
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
