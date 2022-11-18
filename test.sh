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
assert 102 " x; y; x=7; y= x*x + 9*x -10; return y;"
assert 64 " x=2; y=x*x*x*x*x*x; return y;"
assert 75 " x=23; y=(x+2)*3; return y;"
assert 24 " x = 24; if( x<20 ) x=4; return x;"
assert 3 " a = 3; if(a != 3) return a+1; return a;"
assert 3 " z=1; if(z!=2)z=z+1; if(z==2)z=z+1; return z;"
assert 10 " y=2; if(y<=3)y=y*3; if(y>=3)y=y*(3-1)-2; return y;"
assert 20 "for(x=17;x<20;x=x+1)x=x; return x;"
assert 16 "a=0; y=2; for(;a<3;a=a+1)y=y*2; return y;"

echo OK
