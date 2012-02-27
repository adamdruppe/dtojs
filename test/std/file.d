module std.file;

import browser.storage;

class FileException : Exception {
	this(string message, string file = __FILE__, int line = __LINE__) {
		super(message, file, line);
	}
}

string readText(string name) {
	return localStorage.getItem(name);
}

void write(string name, string data) {
	localStorage.setItem(name, data);
}

void remove(string name) {
	localStorage.removeItem(name);
}
