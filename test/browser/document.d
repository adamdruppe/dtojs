/**
	Provides access to the browser's DOM api: the
	document object, elements, and events.

	The document object also includes things like cookies,
	local storage, and a bunch of other random stuff.


	Note: this core file makes no attempt at cross-browser
	quirks. What you write here is pretty much directly
	sent to the browser in JS form.

	(You can test for the existence of an object at runtime
	by using the address of operator:

	if(&document.querySelector) {
		// go ahead and use it
	}
*/
module browser.document;

extern(js):
pragma(no_js_output):

JSDocument document;

final class JSDocument {
	JSElement getElementById(string);
	JSElement querySelector(string);
	JSElement[] querySelectorAll(string);

	void write(string);

	JSElement documentElement;
	JSElement __js_body; // alas, body is a keyword in D, so have to do it this way. This refers to document.body directly in Javascript though.
	alias __js_body mainBody;

	JSElement createElement(string);
	JSEvent createEvent(...);

	string cookie;
}

final class JSElement {
	string className;
	string tagName;

	string textContent; // not in old IE

	string innerHTML;
	alias innerHTML innerHtml;

	JSElement getElementById(string);
	JSElement querySelector(string);
	JSElement[] querySelectorAll(string);

	string getAttribute(string);
	void setAttribute(string, string);
	bool hasAttribute(string);

	JSElement appendChild(JSElement);

	JSElement nextSibling;
	JSElement previousSibling;
	JSElement parentNode;

	JSElement offsetParent;
	int offsetWidth;
	int offsetHeight;
	int offsetLeft;
	int offsetTop;

	int scrollLeft;
	int scrollTop;
	int scrollWidth;
	int scrollHeight;

	void addEventListener(...);

	JSCanvasContext getContext(string);
}

final class Image {
	void delegate() onload;
	void delegate() onerror;
	string src;
	int width;
	int height;
}

final class JSCanvasContext {
	string fillStyle;
	string strokeStyle;
	float globalAlpha;
	int lineWidth;
	string lineCap;
	string lineJoin;

	int shadowOffsetX;
	int shadowOffsetY;
	int shadowBlur;
	string shadowColor;
	string font;

	string globalCompositeOperation;
	void clip();

	void fillText(string, int, int);

	void save();
	void restore();
	void translate(float, float);
	void rotate(float);
	void scale(float, float);
	void transform(float, float, float, float, float, float);
	void setTransform(float, float, float, float, float, float);

	void fillRect(int, int, int, int);
	void clearRect(int, int, int, int);
	void strokeRect(int, int, int, int);
	void beginPath();
	void moveTo(int, int);
	void lineTo(int, int);
	void fill();
	void stroke();
	void arc(int, int, int, float, float, bool);
	void quadraticCurveTo(int, int, int, int);
	void bezierCurveTo(int, int, int, int, int, int);

	void rect(int, int, int, int);

	void drawImage(Image, int, int);
	void drawImage(Image, int, int, int, int, int, int, int, int);

	JSGradient createLinearGradient(int, int, int, int);
	JSGradient createRadialGradient(int, int, int, int, int, int);

	string createPattern(Image, string); // FIXME: doesn't *really* return string
}

final class JSGradient {
	string _d_hack;
	alias _d_hack this; // so we can assign it to fillStyle and strokeStyle

	void addColorStop(float, string);
}

final class JSEvent {
	string type;
	bool bubbles;
	bool cancelable;
	JSEvent currentTarget; // aka this in JS i think
	long timeStamp;

	void preventDefault();
	void stopPropagation();

	JSElement target;
	alias target srcElement;

	JSElement relatedTarget;

	int x;
	int y;
	int pageX;
	int pageY;
	int offsetX; // i don't think these are correct on firefox :-(
	int offsetY;
	int clientX;
	int clientY;
	int screenX;
	int screenY;

	bool ctrlKey;
	bool altKey;
	bool shiftKey;
	bool metaKey;
	
	int button;
	int key;
	int which;
}
/*
class JSInputEvent : JSEvent {

}
*/
