<html><head><title>WiFi connection</title>
<link rel="stylesheet" type="text/css" href="style.css">
<script type="text/javascript" src="140medley.min.js"></script>
<script type="text/javascript">

var currAp="%currSsid%";

</script>
</head>
<body>
<div id="main">
<p>
Current WiFi mode: %WiFiMode%
</p>
<p>
Note: %WiFiapwarn%
</p>
<form name="wifiform" action="connect.cgi" method="post">
<p>
To connect to a WiFi network, please select one of the detected networks...<br>
<div id="aps">Scanning...</div>
<br>
WiFi SSID: <br />
<input type="text" name="essid" val="%WiFiEssid%"> <br />
<br>
WiFi password, if applicable: <br />
<input type="text" name="passwd" val="%WiFiPasswd%"> <br />
URL Endpoint<br />
<input type="text" name="url" val="%urlendpoint%"> <br />
<br>
Port<br />
<input type="text" name="port" val="%port%"> <br />
<input type="submit" name="connect" value="Connect!">
</p>
</div>
</body>
</html>
