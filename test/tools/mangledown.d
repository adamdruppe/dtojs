import std.string;
import std.array;
import std.file;
import core.demangle;

bool isIdentifierChar(char c) {
	if(c == '_')
		return true;
	if(c >= '0' && c <= '9')
		return true;
	if(c >= 'a' && c <= 'z')
		return true;
	if(c >= 'A' && c <= 'Z')
		return true;
	return false;
}

string doReplace(string s) {
	int count;
	string[string] replacements;
  again:
	auto idx = s.indexOf("_D");
	if(idx == -1)
		return s;

	auto start = idx;
	while(idx < s.length && isIdentifierChar(s[idx]))
		idx++;
	auto found = s[start .. idx];

	if(found !in replacements)
		replacements[found] = format("$%d$", ++count);

	s = s[0 .. start] ~ replacements[found] ~ s[idx .. $];

	// FIXME: check for string literals smarter
	//if(start != 0 && s[start-1] == '"')
	//	return null; // string literals a

	goto again;
}

void main(string[] args) {
	auto src = readText(args[1]);

	src = src.doReplace();

	std.file.write(args[1], src);
}
