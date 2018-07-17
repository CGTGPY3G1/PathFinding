#pragma once
#include <SFML\Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>



struct Edge {
	int cost = 0, first = 0, second = 0;
	Edge(const int & traversalCost = 0, const int & firstID = 0, const int & secondID = 0) : cost(traversalCost), first(firstID), second(secondID) {}
};

struct HasID {
	HasID(const int & i) : ID(i) {}
	bool operator()(std::shared_ptr<Edge> l) { return l->first == ID || l->second == ID; }
private:
	int ID;
};

struct Node {
	int x, y, g, h, id;
	std::weak_ptr<Node> parent;
	std::vector<std::shared_ptr<Edge>> edges;
	int GetEdgeCost(const int & id) {
		if(this->id == id) return 0;
		std::vector<std::shared_ptr<Edge>>::iterator it = std::find_if(edges.begin(), edges.end(), HasID(id));
		if(it == edges.end()) return 0;
		return (*it)->cost;
	}
	bool isClosed, isOpen;
	Node(const int & posX = 0, const int & posY = 0, const int & iD = 0) : x(posX), y(posY), id(iD), g(0), h(0), isClosed(false), isOpen(false) {}
	bool operator () (const std::shared_ptr<Node> other) { 
		return g + h < other->g + other->h; 
	}
	bool operator < (const std::shared_ptr<Node> other) { 
		return g + h < other->g + other->h; 
	}

	bool operator == (const std::shared_ptr<Node> other) { 
		return id == other->id && x == other->x && y == other->y && g == other->g && h == other->h;
	}
};

class GraphSolver {
public:
	GraphSolver();
	~GraphSolver();
	void Load();
	void Update();
	void Render();
	std::string GetPathAsString(const std::string & postFix = "");
	int GetCost();
	std::vector<int> GetPathAStar(const int & startNode, const int & endNode);
	float GetDistance(std::shared_ptr<Node> current, std::shared_ptr<Node> destination);
	const bool IsRunning() const;
	
private:
	void DrawText(const std::string & text, const sf::Vector2f & position, const sf::Color colour = sf::Color::White, const int & size = 18);
	void DrawCircle(const sf::Vector2f & position, const float & radius, const sf::Color colour);
	int GetClosestNode(const sf::Vector2f & position);
	std::vector<std::shared_ptr<Node>> nodes;
	sf::RenderWindow window;
	sf::ContextSettings settings;
	sf::Font font;
	bool running = true, allowChanges = false;
	sf::Vector2f windowScale = sf::Vector2f(1.0f, 1.0f);
	const int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;
	int startIndex = 0, endIndex = 60;
	std::vector<int> aStarPath;
	sf::Vector2f offset = sf::Vector2f(30.0f, -50.0f);
	sf::Vector2f mousePosition;
};