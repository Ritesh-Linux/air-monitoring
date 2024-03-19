#include "Arduino.h"

const char htmlPage[] PROGMEM = R"(
<!DOCTYPE html>
<html lang=en>
<head>
<meta charset=UTF-8 />
<meta name=viewport content="width=device-width, initial-scale=1.0" />
<title>esp</title>
<script src=https://cdn.tailwindcss.com></script>
<style>table{width:100%}td{padding:6px}table tr td:nth-child(1){text-align:left}table tr td:nth-child(2){text-align:right}</style>
</head>
<body>
<div class="shadow-[0_20px_50px_rgba(8,_112,_184,_0.7)] w-[320px] h-[220px] mx-auto rounded-xl mt-4">
<div id=connecting class="w-full h-full flex items-center justify-center">
<h3 class=text-lg>Connecting ...</h3>
</div>
<div id=connected class="px-8 py-4 w-full h-full hidden">
<div class="font-bold text-xl mb-4 text-[rgba(8,_112,_184,_1)] text-center">
Monitoring Details
</div>
<table>
<tr>
<td>Humidity :</td>
<td id=humidity>54 %</td>
</tr>
<tr>
<td>Temperature :</td>
<td id=temperature>27 &deg;C</td>
</tr>
<tr>
<td>Air Quality :</td>
<td id=air>80 ppm</td>
</tr>
<tr>
<td>AQI Status :</td>
<td id=status>Good Air</td>
</tr>
</table>
</div>
</div>
<p id=value class="mt-12 text-center"></p>
<script>socket=new WebSocket('ws://192.168.5.110:81')
socket.onopen=(e)=>{console.log('[socket] socket.onopen')}
socket.onerror=(e)=>{console.log('[socket] socket.onerror')}
socket.onmessage=(e)=>{const message=e.data
if(message!=='Connected'){document.getElementById('connected').classList.remove('hidden')
document.getElementById('connecting').classList.add('hidden')
const data=JSON.parse(message)
console.log(data)
document.getElementById('humidity').innerHTML=data.humidity
document.getElementById('temperature').innerHTML=data.temperature
document.getElementById('air').innerHTML=data.air
document.getElementById('status').innerHTML=data.status}else{document.getElementById('connected').classList.add('hidden')
document.getElementById('connecting').classList.remove('hidden')}}</script>
</body>
</html>
)";