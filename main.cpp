#include <iostream>
#include <chrono>
#include <random>
#include <vector>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Image.hpp>

bool done = false;

std::mt19937 generator;
std::uniform_real_distribution<double> heightDist(0, 1);
std::uniform_real_distribution<double> roughDist;
std::uniform_int_distribution<int> startDist(-10000, 10000);

sf::Vector2u size;
std::vector<double> heights;
void divideSquare(bool first, unsigned x, unsigned y, unsigned w, unsigned h, double c1, double c2, double c3, double c4);

int main(int argc, char** argv)
{
	if(argc != 4)
	{
		std::cerr << "Incorrect # of args, must be 3.\n";
		return 1;
	}

	size = {static_cast<unsigned>(std::stoi(argv[1])), static_cast<unsigned>(std::stoi(argv[2]))};
	
	// resize vector to correct size
	heights.resize(size.x * size.y, 0);
	
	double roughness = std::stod(argv[3]);
	
	// clamp roughness variable to 0..1
	if(roughness < 0)
		roughness = 0;
	if(roughness > 1)
		roughness = 1;
	
	roughDist = std::uniform_real_distribution<double>(-roughness, roughness);

	// seed generator
	generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

	// make the image
	sf::Image heightMap;
	heightMap.create(size.x, size.y);

	// begin the recursive algo!
	divideSquare(true, 0, 0, size.x, size.y, -1, -1, -1, -1);

	// fill the image
	for(unsigned i = 0; i < size.x * size.y; i++)
	{
		sf::Uint8 r = 255.0 * heights[i];
		sf::Uint8 g = 255.0 * heights[i];
		sf::Uint8 b = 255.0 * heights[i];

		heightMap.setPixel(i % size.x, i / size.y, {r, g, b});
	}

	heightMap.saveToFile("img" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".png");

	return 0;
}

void divideSquare(bool first, unsigned x, unsigned y, unsigned w, unsigned h, double c1, double c2, double c3, double c4)
{
	// get corners
	double topLeft, topRight, botLeft, botRight;

	if((c1 == -1 && c2 == -1 && c3 == -1 && c4 == -1) || first)
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

	if(first)
	{
		topEdge = heightDist(generator);
		leftEdge = heightDist(generator);
		rightEdge = heightDist(generator);
		botEdge = heightDist(generator);
	}
	else
	{
		topEdge = (topLeft + topRight) / 2.0;
		leftEdge = (topLeft + botLeft) / 2.0;
		rightEdge = (topRight + botRight) / 2.0;
		botEdge = (botLeft + botRight) / 2.0;
	}
	
	// middle point of the square, add a random amount to keep the map less uniform
	double middle;
	
	if(first)
		middle = heightDist(generator);
	else
		//middle = (topLeft + topRight + botLeft + botRight + heightDist(generator)) / 5.0;
		middle = (topLeft + topRight + botLeft + botRight) / 4.0 + roughDist(generator);
	
	// keep bounds
	if(middle < 0)
		middle = 0;
	if(middle > 1)
		middle = 1;
		
	// assign values
	// corners
	heights[(x % size.x) + (y % size.y) * size.x] = topLeft;
	heights[(x + w) % size.x + (y % size.y) * size.x] = topRight;
	heights[(x % size.x) + ((y + h) % size.y) * size.x] = botLeft;
	heights[(x + w) % size.x + ((y + h) % size.y) * size.x] = botRight;

	// midpoints
	heights[(x + (w / 2)) % size.x + (y % size.y) * size.x] = topEdge;
	heights[(x % size.x) + ((y + (h / 2)) % size.y) * size.x] = leftEdge;
	heights[(x + w) % size.x + ((y + (h / 2)) % size.y) * size.x] = rightEdge;
	heights[(x + (w / 2)) % size.x + ((y + h) % size.y) * size.x] = botEdge;
	heights[(x + (w / 2)) % size.x + ((y + (h / 2)) % size.y) * size.x] = middle;

	// if we're too small to continue
	if(!(w > 1 || h > 1))
		return;

	divideSquare(false, x, y, w / 2.0, h / 2.0, topLeft, topEdge, leftEdge, middle);	// top left quad
	divideSquare(false, x + w / 2.0, y, w / 2.0, h / 2.0, topEdge, topRight, middle, rightEdge);	// top right quad
	divideSquare(false, x, y + h / 2.0, w / 2.0, h / 2.0, leftEdge, middle, botLeft, botEdge);	// bot left quad
	divideSquare(false, x + w / 2.0, y + h / 2.0, w / 2.0, h / 2.0, middle, rightEdge, botEdge, botRight);	// bot right quad
}
