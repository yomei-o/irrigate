/*
Copyright (c) 2016, Yomei Otani <yomei.otani@gmai.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
*/
//
// Debufg print
//
println = function(src)
{
 alert(src);
}
//
//
//
var url = "ws://irrigate.sensprout.net:12345/websocket/chat/1?ki=1&kp=admin";
var command = "gettmstate";
var result = "no result";
//
//
//
createExecButton = function()
{
 $("#button_exec").click(onClickExecButton);
}
setUrlText = function(str)
{
 $("#text_url").val(str);
}
getUrlText = function()
{
 var ret="";
 ret=$("#text_url").val();
 return ret;
}
setCommandText = function(str)
{
 $("#text_command").val(str);
}
getCommandText = function()
{
 var ret = "";
 ret = $("#text_command").val();
 return ret;
}
setResultText = function(str)
{
 $("#text_result").val(str);
}
getResultText = function()
{
 var ret = "";
 ret = $("#text_result").val();
 return ret;
}
addResultText = function(str)
{
 str = getResultText() + str;
 setResultText(str);
}
//
// 
//
onmessage = function(n, str)
{
 addResultText(str);
}
onopen = function(n)
{
 //println("onopen()");
 addResultText("websocket open OK\n");
 websocket_send(0, command + "\n");
}
onerror = function(n)
{
 //println("onerror()");
 websocket_close(0);
 addResultText("websocket open error\n");
}
onclose = function(n)
{
 //println("onclose()");
 addResultText("websocket closed OK\n");
 websocket_close(0);
}
//
//
//
onClickExecButton = function()
{
 var ret;
 //println("onClickExecButton()");
 //println(getUrlText());
 command = getCommandText();
 url = getUrlText();
 result = "";
 setResultText(result);
 if (websocket_isusing(0) == false){
  ret = websocket_open(0, url);
  //println("ret="+ret);
  websocket_onopen(0, onopen);
  websocket_onerror(0, onerror);
  websocket_onclose(0, onclose);
  websocket_onmessage(0, onmessage);
 }
 websocket_send(0, command + "\n");
 //println("onClickExecButton() end");
}
//
//
//
jQuery().ready(function()
{
 //println("hello");
 createExecButton();
 setUrlText(url);
 setCommandText(command);
 setResultText(result);
 //println("end");
}
);
