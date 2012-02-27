// FIXME: in
// FIXME: enums (I think I'd like them to simply be strings or whatever base type in JS)

import browser.window;
import browser.document;
import browser.xmlhttp;

void runme() {
	auto a = 10;
	auto b = -a;
	auto e = document.getElementById("test");
	e.className ~= "sweet";

	if(&document.querySelector)
		e.textContent = "querySelector works here!";
	else
		e.textContent = "sucky";

	e.addEventListener("click", {
		e.className = "";
	});
	//assert(0, "cool");
}

import std.conv;
import std.file;

class MyBase {
	string myName() { return "my base"; }
	string overrideme() { return "something"; }
}

class MyClass : MyBase {
	this() {}

	override string myName() { return "my class"; }

	void someMethod() { }
	override string overrideme() { return "roflcopter"; }
}

class MyOtherClass : MyBase {
	this() {}

	override string myName() { return "my other class"; }
}

void main() {
	MyBase o2 = new MyOtherClass();
	MyBase o = new MyClass();

	switch("as") {
		case "a":
			window.alert("lol");
		break;
		case "b":
		default: //assert(0);
	}

	window.onload = &runme;


	auto canvas = document.createElement("canvas");
	canvas.setAttribute("width", "100");
	canvas.setAttribute("height", "100");
	canvas.setAttribute("style", "width: 500px; height: 500px;");
	document.mainBody.appendChild(canvas);

	auto ctx = canvas.getContext("2d");

	ctx.fillRect(0, 0, 30, 30);

	//std.file.write("testitem", "cool");

	//window.alert(readText("testitem"));
/*
	auto http = new XMLHttpRequest();
	http.open("GET", "index.html", false);
	http.send();
	document.mainBody.innerHTML = http.responseText;
*/
}
