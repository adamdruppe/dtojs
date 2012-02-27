function __d_6microd6MyBase(__d_constructor) {
__d_6Object.call(this);
this.__d_vtbl.length = 3;
this.__d_vtbl[1] = $1$;
this.__d_vtbl[2] = $2$;
this.__d_classname = "microd.MyBase";
this.__d_mangle = "6microd6MyBase";
this.__d_implements = ["6microd6MyBase", "6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function __d_6microd7MyClass(__d_constructor) {
__d_6Object.call(this);
__d_6microd6MyBase.call(this);
this.__d_vtbl.length = 4;
this.__d_vtbl[1] = $3$;
this.__d_vtbl[3] = $4$;
this.__d_vtbl[2] = $5$;
this.__d_classname = "microd.MyClass";
this.__d_mangle = "6microd7MyClass";
this.__d_implements = ["6microd7MyClass", "6microd6MyBase", "6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function __d_6microd12MyOtherClass(__d_constructor) {
__d_6Object.call(this);
__d_6microd6MyBase.call(this);
this.__d_vtbl.length = 3;
this.__d_vtbl[1] = $6$;
this.__d_classname = "microd.MyOtherClass";
this.__d_mangle = "6microd12MyOtherClass";
this.__d_implements = ["6microd12MyOtherClass", "6microd6MyBase", "6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function __d_6Object(__d_constructor) {
this.__d_vtbl = [];
this.__d_vtbl.length = 1;
this.$7$ = null;
this.$8$ = null;
this.$9$ = [];
this.$10$ = "";
this.$11$ = "";
this.__d_classname = "object.Object";
this.__d_mangle = "6Object";
this.__d_implements = ["6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function __d_6object9Throwable(__d_constructor) {
__d_6Object.call(this);
this.__d_vtbl.length = 2;
this.$12$ = "";
this.$13$ = "";
this.$14$ = 0;
this.__d_vtbl[1] = $15$;
this.__d_classname = "object.Throwable";
this.__d_mangle = "6object9Throwable";
this.__d_implements = ["6object9Throwable", "6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function __d_6object5Error(__d_constructor) {
__d_6Object.call(this);
__d_6object9Throwable.call(this);
this.__d_vtbl.length = 2;
this.__d_classname = "object.Error";
this.__d_mangle = "6object5Error";
this.__d_implements = ["6object5Error", "6object9Throwable", "6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function __d_9Exception(__d_constructor) {
__d_6Object.call(this);
__d_6object9Throwable.call(this);
this.__d_vtbl.length = 2;
this.__d_classname = "object.Exception";
this.__d_mangle = "9Exception";
this.__d_implements = ["9Exception", "6object9Throwable", "6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function __d_6object11AssertError(__d_constructor) {
__d_6Object.call(this);
__d_6object9Throwable.call(this);
__d_6object5Error.call(this);
this.__d_vtbl.length = 2;
this.__d_classname = "object.AssertError";
this.__d_mangle = "6object11AssertError";
this.__d_implements = ["6object11AssertError", "6object5Error", "6object9Throwable", "6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function __d_3std4file13FileException(__d_constructor) {
__d_6Object.call(this);
__d_6object9Throwable.call(this);
__d_9Exception.call(this);
this.__d_vtbl.length = 2;
this.__d_classname = "std.file.FileException";
this.__d_mangle = "3std4file13FileException";
this.__d_implements = ["3std4file13FileException", "9Exception", "6object9Throwable", "6Object"];
return __d_construct.call(this, __d_constructor, arguments);}
function $16$() {
var $17$ = 10;
var $18$ = -($17$);
var $19$ = document.getElementById("test");
($19$).className = (($19$).className.concat("sweet"));
if (document.querySelector) {(($19$).textContent = "querySelector works here!");
}else {(($19$).textContent = "sucky");
}$19$.addEventListener("click", function ($20$) {
if(!$20$) var $20$ = this;
(($19$).className = "");
}
);
}
function $21$() {
var $22$ = __d_dynamic_cast(new __d_6microd12MyOtherClass($23$), "6microd6MyBase");
var $24$ = __d_dynamic_cast(new __d_6microd7MyClass($25$), "6microd6MyBase");
switch("as") {
case "a":
window.alert("lol");
break;case "b":
default:
}((window).onload = $16$);
var $26$ = document.createElement("canvas");
$26$.setAttribute("width", "100");
$26$.setAttribute("height", "100");
$26$.setAttribute("style", "width: 500px; height: 500px;");
(document).body.appendChild($26$);
var $27$ = $26$.getContext("2d");
$27$.fillRect(0, 0, 30, 30);
return 0;
}
function $1$($28$) {
if(!$28$) var $28$ = this;
$28$ || __d_assert_msg("null this", "microd.d", 29);
return "my base";
}
function $2$($29$) {
if(!$29$) var $29$ = this;
$29$ || __d_assert_msg("null this", "microd.d", 30);
return "something";
}
function $25$() {
var $30$ = this;
return $30$;
}
function $3$($31$) {
if(!$31$) var $31$ = this;
$31$ || __d_assert_msg("null this", "microd.d", 36);
return "my class";
}
function $4$($32$) {
if(!$32$) var $32$ = this;
$32$ || __d_assert_msg("null this", "microd.d", 38);
}
function $5$($33$) {
if(!$33$) var $33$ = this;
$33$ || __d_assert_msg("null this", "microd.d", 39);
return "roflcopter";
}
function $23$() {
var $34$ = this;
return $34$;
}
function $6$($35$) {
if(!$35$) var $35$ = this;
$35$ || __d_assert_msg("null this", "microd.d", 45);
return "my other class";
}
function __d_assert_msg($36$, $37$, $38$) {
throw new __d_6object11AssertError($39$,$36$,$37$,$38$);}
function __d_assert($40$, $41$) {
throw new __d_6object11AssertError($39$,"Assertion failure",$40$,$41$);}
function __d_dynamic_cast($42$, $43$) {
if (($42$).__d_implements) {for (var $44$ = (($42$).__d_implements).slice(0), $45$ = 0;
($45$ < ($44$.length)); ($45$ += 1)) {var $46$ = $44$[$45$];
if (($46$ == $43$)) {return $42$;
}}
return null;
}return $42$;
}
function __d_construct($47$, $48$) {
if (!($47$)) {return this;
}var $49$ = [];
for (var $50$ = 1;
(($50$) < ($48$.length)); ($50$)++) {$49$.push($48$[($50$)]);
}
return $47$.apply(this, $49$);
}
function $51$($52$, $53$, $54$) {
var $55$ = this;
(($55$).message = $52$);
(($55$).file = $53$);
(($55$).line = $54$);
(($55$).toString = $15$);
return $55$;
}
function $15$($56$) {
if(!$56$) var $56$ = this;
$56$ || __d_assert_msg("null this", "object.d", 90);
return ((((((($56$).file.concat(":")).concat(String(($56$).line))).concat(" [")).concat(($56$).__d_classname)).concat("] ")).concat(($56$).message));
}
function $57$($58$, $59$, $60$) {
var $61$ = this;
$51$.call($61$, $58$, $59$, $60$);
return $61$;
}
function $62$($63$, $64$, $65$) {
var $66$ = this;
$51$.call($66$, $63$, $64$, $65$);
return $66$;
}
function $39$($67$, $68$, $69$) {
var $70$ = this;
$57$.call($70$, $67$, $68$, $69$);
return $70$;
}
function $71$($72$) {
return localStorage.getItem($72$);
}
function $73$($74$, $75$) {
localStorage.setItem($74$, $75$);
}
function $76$($77$) {
localStorage.removeItem($77$);
}
function $78$($79$, $80$, $81$) {
var $82$ = this;
$62$.call($82$, $79$, $80$, $81$);
return $82$;
}
function $83$($84$, $85$, $86$) {
var $87$ = $84$;
return ($87$.replace($85$, $86$)).d_str;
}
function $88$($89$, $90$) {
var $91$ = $89$;
return $91$.indexOf($90$);
}
if($21$ != null) $21$();
