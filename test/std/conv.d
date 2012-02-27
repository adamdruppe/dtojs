module std.conv;

T to(T : string, R)(R r) {
	return String(r);
}

T to(T : int, R)(R r) {
	return Number(r);
}
