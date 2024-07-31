#pragma once
class Color
{
public:
	int red, green, blue, alpha;

	Color() : red(0), green(0), blue(0), alpha(0) {};
	Color(int red, int green, int blue, int alpha) : red(red), green(green), blue(blue), alpha(alpha) {};
};

