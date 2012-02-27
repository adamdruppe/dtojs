module browser.json;

extern(js):
pragma(no_js_output):


final class JSJson {
	Dynamic parse(string);
	string stringify(...);
}

JSJson JSON;
