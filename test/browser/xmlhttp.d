module browser.xmlhttp;

extern(js):
pragma(no_js_output):

import browser.document;

final class XMLHttpRequest {
	void open(string method, string url, bool async, string user = null, string pass = null);
	void setRequestHeader(string name, string value);
	void send(string data = null);

	void delegate() onreadstatechange;
	//void delegate() ontimeout;

	long timeout;
	bool withCredentials;

	void abort();

	int readyState;

	ushort status;
	string statusText;
	string getResponseHeader(string);
	string getAllResponseHeaders(string);

	void overrideMimeType(string);
	string responseType;
	Dynamic response;
	string responseText;
	JSDocument responseXML;
}
