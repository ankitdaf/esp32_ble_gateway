/*
ESP8266 web server - platform-dependent routines, nonos version
*/

#include <esp8266.h>
#include "httpd.h"
#include "platform.h"
#include "httpd-platform.h"

#ifndef FREERTOS

//Listening connection data
static struct espconn httpdConn;
static esp_tcp httpdTcp;

//Set/clear global httpd lock.
//Not needed on nonoos.
void  httpdPlatLock() {
}
void  httpdPlatUnlock() {
}


static void  platReconCb(void *arg, sint8 err) {
	//From ESP8266 SDK
	//If still no response, considers it as TCP connection broke, goes into espconn_reconnect_callback.

	ConnTypePtr conn=arg;
	//Just call disconnect to clean up pool and close connection.
	httpdDisconCb(conn, (char*)conn->proto.tcp->remote_ip, conn->proto.tcp->remote_port);
}

static void  platDisconCb(void *arg) {
	ConnTypePtr conn=arg;
	httpdDisconCb(conn, (char*)conn->proto.tcp->remote_ip, conn->proto.tcp->remote_port);
}

static void  platRecvCb(void *arg, char *data, unsigned short len) {
	ConnTypePtr conn=arg;
	httpdRecvCb(conn, (char*)conn->proto.tcp->remote_ip, conn->proto.tcp->remote_port, data, len);
}

static void  platSentCb(void *arg) {
	ConnTypePtr conn=arg;
	httpdSentCb(conn, (char*)conn->proto.tcp->remote_ip, conn->proto.tcp->remote_port);
}

static void  platConnCb(void *arg) {
	ConnTypePtr conn=arg;
	if (httpdConnectCb(conn, (char*)conn->proto.tcp->remote_ip, conn->proto.tcp->remote_port)) {
		espconn_regist_recvcb(conn, platRecvCb);
		espconn_regist_reconcb(conn, platReconCb);
		espconn_regist_disconcb(conn, platDisconCb);
		espconn_regist_sentcb(conn, platSentCb);
	} else {
		espconn_disconnect(conn);
	}
}


int  httpdPlatSendData(ConnTypePtr conn, char *buff, int len) {
	int r;
	r=espconn_sent(conn, (uint8_t*)buff, len);
	return (r>=0);
}

void  httpdPlatDisconnect(ConnTypePtr conn) {
	espconn_disconnect(conn);
}

void  httpdPlatDisableTimeout(ConnTypePtr conn) {
	//Can't disable timeout; set to 2 hours instead.
	espconn_regist_time(conn, 7199, 1);
}

//Initialize listening socket, do general initialization
void  httpdPlatInit(int port, int maxConnCt) {
	httpdConn.type=ESPCONN_TCP;
	httpdConn.state=ESPCONN_NONE;
	httpdTcp.local_port=port;
	httpdConn.proto.tcp=&httpdTcp;
	espconn_regist_connectcb(&httpdConn, platConnCb);
	espconn_accept(&httpdConn);
	espconn_tcp_set_max_con_allow(&httpdConn, maxConnCt);
}


#endif