module browser.window;

extern(js):
pragma(no_js_output):

import browser.document;
import browser.storage;

final class JSWindow {
	void alert(string);
	bool confirm(string);
	string prompt(string);

	int setTimeout(...);
	int setInterval(...);

	void clearTimeout(int);
	void clearInterval(int);

	void function() onload;

	JSWindow top;
	JSWindow parent;

	// these tend to be available elsewhere

	JSDocument document;
	JSEvent event;

	JSStorage localStorage;
	JSStorage sessionStorage;
}

JSWindow window;
