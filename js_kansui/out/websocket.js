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
// debug func
//
println = function(str)
{
 alert(str);
}
strlength = function(src)
{
 var ret = 0;
 if (src == null || src == "")return ret;
 ret = src.length;
 return ret;
}
//
// statics 
//
var ws_handle = new Array();
var ws_url = new Array();
var ws_onopen = new Array();
var ws_onerror = new Array();
var ws_onclose = new Array();
var ws_onmessage = new Array();
var ws_stat = new Array();
//
// functions
//
websocket_isusing=function(n)
{
 var ret = false;
 if (n<0 || n>=20)return ret;
 if (ws_handle[n] == null)return ret;
 ret = true;
 return ret;
}
websocket_open=function(n,str)
{
 var n_ = n;
 if (n<0 || n>=20)return -1;
 if (ws_handle[n]!=null)return -1;
 ws_handle[n] = null;
 ws_url[n] = str;
 ws_onopen[n] = null;
 ws_onerror[n] = null;
 ws_onclose[n] = null;
 ws_onmessage[n] = null;
 ws_stat[n]=0;
 try{
  ws_handle[n] = new WebSocket(str);
 }catch (e){}
 if (ws_handle[n] == null){
  return -1;
 }
 ws_handle[n].onmessage = function(evt){
  if (ws_onmessage[n_] != null)ws_onmessage[n_](n_, evt.data);
 }
 ws_handle[n].onopen = function(evt){
  if (ws_onopen[n_] != null)ws_onopen[n_](n_);
 }
 ws_handle[n].onerror = function(evt){
  if (ws_onerror[n_] != null)ws_onerror[n_](n_);
 }
 ws_handle[n].onclose = function(evt){
  if (ws_onclose[n_] != null)ws_onclose[n_](n_);
 }
 return 0;
}
websocket_close = function(n)
{
 if (n < 0 || n >= 20 || ws_handle[n] == null)return -1;
 try{
  ws_handle[n].close();
 }catch (e){}
 ws_onopen[n] = null;
 ws_onerror[n] = null;
 ws_onmessage[n] = null;
 ws_onclose[n] = null;
 ws_handle[n] = null;
 ws_url[n] = null;
 ws_stat[n] = 0;
 return 0;
}
websocket_send = function(n, str)
{
 var ret = -1;
 if (n < 0 || n >= 20 || ws_handle[n] == null)return -1;
 if (strlength(str) < 1)return -1;
 try{
  ws_handle[n].send(str);
  ret=0;
 }
 catch (e){}
 return ret;
}
//
//
//
websocket_onmessage = function(n, func)
{
 if (n<0 || n >= 20 || ws_handle[n] == null)return;
 ws_onmessage[n] = func;
}
websocket_onclose = function(n, func)
{
 if (n<0 || n >= 20 || ws_handle[n] == null)return;
 ws_onclose[n] = func;
}
websocket_onopen = function(n, func)
{
 if (n<0 || n >= 20 || ws_handle[n] == null)return;
 ws_onopen[n] = func;
}
websocket_onerror = function(n, func)
{
 if (n<0 || n >= 20 || ws_handle[n] == null)return;
 ws_onerror[n] = func;
}
//
//
//
//
//
//
