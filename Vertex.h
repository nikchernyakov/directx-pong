#pragma once

struct Pos
{
	float x;
	float y;
};

struct ColorRGBA
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

class Vertex
{
public:
	Pos pos;
	ColorRGBA color;
public:
	Vertex() = default;
	Vertex(float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		pos.x = x;
		pos.y = y;
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}
	Vertex operator+(Vertex const v) {
		Vertex vNew;
		vNew.pos.x = v.pos.x + pos.x;
		vNew.pos.y = v.pos.y + pos.y;
		vNew.color = color;
		return vNew;
	}

	Vertex operator*(float const v) {
		Vertex vNew;
		vNew.pos.x = pos.x * v;
		vNew.pos.y = pos.y * v;
		vNew.color = color;
		return vNew;
	}
};