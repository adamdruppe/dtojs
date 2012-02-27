// FIXME: implement dynamic type that meshes with JS itself well enough
// FIXME: all user defined classes need constructors, but all JS defined
// classes must not have them....

/*
	Changes to the compiler:

	extern(js) are unmangled, can be variadic with or without parameters. These try to be native in calling convention

	pragma(no_js_output) means don't write *any* of this out. You can't call them, including construction, and dynamic
	casts won't work from D (this is not checked btw); use it just for pure wrappers around native stuff to trim useless rtti.
*/

alias immutable(char)[] string;

extern(js) {
	/* ******************************** */
	/*   Access to Javascript language  */
	/* ******************************** */

	// access to JS keywords. the type isn't a perfect match
	Object __js_this; // this special thing translates directly to the JS keyword "this". May not refer to what you think it does, even if you're familiar with javascript, due to how the D compiler is free to arrange things
	Object[] __js_arguments; // the arguments keyword in JS. ditto on warning.

	// initializer helpers
	Array __js_array_literal; // outputs as []
	Object __js_object_literal; // outputs as {}


	/* ******************************** */
	/*        D runtime functions       */
	/* ******************************** */

	// called if asserts fail
	void __d_assert_msg(string msg, string file, int line) {
		throw new AssertError(msg, file, line);
	}
	void __d_assert(string file, int line) {
		throw new AssertError("Assertion failure", file, line);
	}

	// given an object, should we allow the runtime cast to succeed?
	// if no, return null.
	Object __d_dynamic_cast(Object from, string to) {
		if(from.__d_implements) {
			foreach(s; from.__d_implements)
				if(s == to)
					return from;
			return null;
		}
		// if in doubt, let it pass since we don't want to tie hands

		return from;
	}

	/// This is called after the initializer, in the JS constructor function, with JS's this set to the new object. It is passed a D constructor as the first argument, and the arguments array from the new function as the second argument (remember, the constructor is #1 there)
	Object __d_construct(Function constructor, Object[] arguments) {
		if(!constructor) return __js_this;

		Array constructorArguments = __js_array_literal;
		for(int a = 1; a < arguments.length; a++)
			constructorArguments.push(arguments[a]);
		return constructor.apply(__js_this, constructorArguments);
	}

	/* ******************************** */
	/*          Basic D classes         */
	/* ******************************** */

	// the D language expects these to be around. Let's try to match JS as well as we can.
	class Object {
		Object prototype; /// the prototype object from javascript
		string delegate() toString; /// javascript expects this around, but we can't do it well with a virtual object (yet), so here it is as a property

		/// these are filled in by the D compiler
		string[] __d_implements; // this is important for dynamic casting
		string __d_classname; // can use it for pretty printing
		string __d_mangle; // the constructor can be called as eval("new __d_" ~ that);
	}
	class Throwable : Object { // required by the D compiler
		string message;
		string file;
		int line;
		this(string msg, string file = __FILE__, int line = __LINE__) {
			this.message = msg;
			this.file = file;
			this.line = line;
			super.toString = &toString;
		}
		string toString() {
			return  file ~ ":" ~ String(line) ~ " [" ~ __d_classname ~ "] " ~ message;
		}
	}
	class Error : Throwable { // required by the D compiler
		this(string msg, string file = __FILE__, int line = __LINE__) {
			super(msg, file, line);
		}
	}
	class Exception : Throwable { // required by the D compiler
		this(string msg, string file = __FILE__, int line = __LINE__) {
			super(msg, file, line);
		}
	}

	/// thrown by our assert function
	class AssertError : Error {
		this(string msg, string file = __FILE__, int line = __LINE__) {
			super(msg, file, line);
		}
	}

	/* ***************************************** */
	/* the Javascript language's building blocks */
	/* ***************************************** */

	// not to be confused with D arrays, which mostly work as one of these under the hood
	pragma(no_js_output) class Array {
		void push(...);
		int length;
	}

	// again not to be confused with a D function, but they are implemented as these
	pragma(no_js_output) class Function {
		Object apply(...);
		Object call(...);
	}

	pragma(no_js_output) class Date {}
	pragma(no_js_output) class RegEx {}
	pragma(no_js_output) class Math {}

	pragma(no_js_output) struct Dynamic{}

	// basic native global functions and objects (good for type conversion implementations)
	int Number(string i);
	string String(int i);
	bool Boolean(...);

	pragma(no_js_output) class JSString {
		immutable(char)[] d_str;
		alias d_str this;

		JSString replace(...);
		int indexOf(...);
		string[] split(...);
		int length;
	}


	void eval(string s);



	/* ***************************************** */
	/* Accessing Javascript library environments */
	/* ***************************************** */

	// most this is in the browser package.
	// import browser.window; import browser.document; etc.
	// keep in mind that the global object in the browser is the window object
	// so import browser.window then doing window.function might work.
	// (you can also just declare the prototype yourself with extern(js)

	// dmdscript functions
	void println(...);//string s);

	string encodeURIComponent(string);
	string decodeURIComponent(string);
}
