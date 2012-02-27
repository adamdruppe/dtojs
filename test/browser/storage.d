module browser.storage;

extern(js):
pragma(no_js_output):

final class JSStorage {
	int length;
	string key(int index);
	string getItem(string key);
	void setItem(string key, string data);
	void removeItem(string key);
	void clear();
}

JSStorage sessionStorage;
JSStorage localStorage;
