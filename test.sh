#!/bin/bash
assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

assert 13 " a=3; b=5*4+3; (a+b)/2;"
assert 0 "0;"
assert 93 " x=3; y=x*x*10+x;"
assert 23 " x=4; y=x*x+3*x-5; "

echo OK
