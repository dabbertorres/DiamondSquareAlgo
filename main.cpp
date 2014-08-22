#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <cmath>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

std::mt19937 generator;
std::uniform_real_distribution<double> heightDist(0, 1);
std::uniform_int_distribution<int> startDist(-10000, 10000);

sf::Vector2u size;
std::vector<double> heights;
void divideSquare(unsigned x, unsigned y, unsigned w, unsigned h, double c1, double c2, double c3, double c4);

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		std::cerr << "Incorrect # of args, must be 2.\n";
		return 1;
	}
	
	size = {static_cast<unsigned>(std::stoi(argv[1])), static_cast<unsigned>(std::stoi(argv[2]))};
	
	// resize vector to correct size
	heights.resize(size.x * size.y);
	
	// seed generator
	generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	
	// begin the recursive algo!
	divideSquare(0, 0, size.x, size.y, -1, -1, -1, -1);
	
	// make the image
	sf::Image heightMap;
	heightMap.create(size.x, size.y, {255, 255, 255});
	
	// fill the image
	for(unsigned i = 0; i < size.x * size.y; i++)
	{
		double heightVal = heights[i];
		
		int r = 255.d * heightVal;
		int g = 255.d * heightVal;
		int b = 255.d * heightVal;
		
		heightMap.setPixel(i % size.x, i / size.y, {r, g, b});
	}
	
	heightMap.saveToFile("img" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".png");
	
	return 0;
}

void divideSquare(unsigned x, unsigned y, unsigned w, unsigned h, double c1, double c2, double c3, double c4)
{	
	// get corners
	double topLeft, topRight, botLeft, botRight;
	
	if(c1 == -1 && c2 == -1 && c3 == -1 && c4 == -1)
	{
		topLeft = heightDist(generator);
		topRight = heightDist(generator);
		botLeft = heightDist(generator);
		botRight = heightDist(generator);
	}
	else
	{
		topLeft = c1;
		topRight = c2;
		botLeft = c3;
		botRight = c4;
	}
	
	// get midpoints on edges
	double topEdge, leftEdge, rightEdge, botEdge;
	
	topEdge = (topLeft + topRight) / 2;
	leftEdge = (topLeft + botLeft) / 2;
	rightEdge = (topRight + botRight) / 2;
	botEdge = (botLeft + botRight) / 2;
	
	// middle point of the square
	double middle = (topLeft + topRight + botLeft + botRight) / 4;
	
	// assign values
	// corners
	heights[x % size.x + y / size.y] = topLeft;
	heights[x % size.x + (w - 1) + y / size.y] = topRight;
	heights[x % size.x + y / size.y + (h - 1)] = botLeft;
	heights[x % size.x + (w - 1) + y / size.y + (h - 1)] = botRight;
	
	// midpoints
	heights[x % size.x + ((w - 1) / 2) + y / size.y] = topEdge;
	heights[x % size.x + y / size.y + ((h - 1) / 2)] = leftEdge;
	heights[x % size.x + (w - 1) + y / size.y + ((h - 1) / 2)] = rightEdge;
	heights[x % size.x + ((w - 1) / 2) + y / size.y + (h - 1)] = botEdge;
	heights[x % size.x + ((w - 1) / 2) + y / size.y + ((h - 1) / 2)] = middle;
	
	// if we're too small to continue
	if(!(w > 2 || h > 2))
		return;
	
	divideSquare(x, y, w / 2, h / 2, topLeft, topEdge, leftEdge, middle);	// top left quad
	divideSquare(x + w / 2, y, w / 2, h / 2, topEdge, topRight, middle, rightEdge);	// top right quad
	divideSquare(x, y + h / 2, w / 2, h / 2, leftEdge, middle, botLeft, botEdge);	// bot left quad
	divideSquare(x + w / 2, y + h / 2, w / 2, h / 2, middle, rightEdge, botEdge, botRight);	// bot right quad
}