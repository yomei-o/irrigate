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

#ifndef JAVASCRIPT
#include "variant.h"
#include "settimeout.h"
#include "websocket.h"
#endif


#ifndef JAVASCRIPT
#if defined(_WIN32) && !defined(__GNUC__)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif /* _WIN32 */
#endif


#ifndef JAVASCRIPT
#define null 0
#define true 1
#define false 0
#endif

#ifdef JAVASCRIPT 
#define BEGIN_TRY() try{
#define END_TRY() }catch(e){}
#else
#define BEGIN_TRY()
#define END_TRY()
#endif

#ifdef JAVASCRIPT 
#define _delete(a,b) delete a[b]
#else
#define _delete(a,b) a.delete_(b)
#endif

//
// Debufg print
//

#ifdef JAVASCRIPT
println = function(src)
#else
void println(var src)
#endif
{
#ifdef JAVASCRIPT
	alert(src);
#else
	printf("%s\n", ((std::string)src).c_str());
#endif
}

#ifndef JAVASCRIPT
void println(const char* a)
{
	var aa;
	aa = a;
	println(aa);
}
#endif

//
//
//

#ifndef JAVASCRIPT
void onClickExecButton();
#endif

var url = "ws://irrigate.sensprout.net:12345/websocket/chat/1?ki=1&kp=admin";
var command = "gettmstate";
var result = "no result";

//
//
//

#ifdef JAVASCRIPT
createExecButton = function()
#else
void createExecButton()
#endif
{
#ifdef JAVASCRIPT
	$("#button_exec").click(onClickExecButton);
#else
	println("createExecButton()");
#endif
}

#ifdef JAVASCRIPT
setUrlText = function(str)
#else
void setUrlText(var str)
#endif
{
#ifdef JAVASCRIPT
	$("#text_url").val(str);
#else
	println("setUrlText(\"" + str + "\")");
#endif
}

#ifdef JAVASCRIPT
getUrlText = function()
#else
var getUrlText()
#endif
{
	var ret="";
#ifdef JAVASCRIPT
	ret=$("#text_url").val();
#endif
	return ret;
}

#ifdef JAVASCRIPT
setCommandText = function(str)
#else
void setCommandText(var str)
#endif
{
#ifdef JAVASCRIPT
	$("#text_command").val(str);
#else
	println("setCommandText(\"" + str + "\")");
#endif
}

#ifdef JAVASCRIPT
getCommandText = function()
#else
var getCommandText()
#endif
{
	var ret = "";
#ifdef JAVASCRIPT
	ret = $("#text_command").val();
#endif
	return ret;
}

#ifdef JAVASCRIPT
setResultText = function(str)
#else
void setResultText(var str)
#endif
{
#ifdef JAVASCRIPT
	$("#text_result").val(str);
#else
	println("setResultText(\"" + str + "\")");
#endif
}

#ifdef JAVASCRIPT
getResultText = function()
#else
var getResultText()
#endif
{
	var ret = "";
#ifdef JAVASCRIPT
	ret = $("#text_result").val();
#endif
	return ret;
}


#ifdef JAVASCRIPT
addResultText = function(str)
#else
void addResultText(var str)
#endif
{
	str = getResultText() + str;
	setResultText(str);
}

//
// 
//



#ifdef JAVASCRIPT
onmessage = function(n, str)
#else
void onmessage(var n, var str)
#endif
{
	addResultText(str);
}


#ifdef JAVASCRIPT
onopen = function(n)
#else
void onopen(var n)
#endif
{
	//println("onopen()");
	addResultText("websocket open OK\n");
	websocket_send(0, command + "\n");
}


#ifdef JAVASCRIPT
onerror = function(n)
#else
void onerror(var n)
#endif
{
	//println("onerror()");
	websocket_close(0);
	addResultText("websocket open error\n");
}


#ifdef JAVASCRIPT
onclose = function(n)
#else
void onclose(var n)
#endif
{
	//println("onclose()");
	addResultText("websocket closed OK\n");
	websocket_close(0);
}


//
//
//



#ifdef JAVASCRIPT
onClickExecButton = function()
#else
void onClickExecButton()
#endif
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

#ifndef JAVASCRIPT
void main_main() {
#endif



#ifndef JAVASCRIPT
}
#endif



#ifdef JAVASCRIPT 
jQuery().ready(function()
#else
void jquery_ready()
#endif
{


	//println("hello");

	createExecButton();
	setUrlText(url);
	setCommandText(command);
	setResultText(result);

	//println("end");


}
#ifdef JAVASCRIPT 
);
#endif




#ifndef JAVASCRIPT
int main(int argc, char* argv[])
{
	main_main();
	jquery_ready();
	return 0;
}
#endif

