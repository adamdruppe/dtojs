module std.string;

string replace(string s, string a, string b) {
	JSString js = s;
	return js.replace(a, b);
}

int indexOf(string s, string a) {
	JSString js = s;
	return js.indexOf(a);
}
