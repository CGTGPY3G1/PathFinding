#include "GraphSolver.h"
#include <map>
#include <queue>
#include <limits>

GraphSolver::GraphSolver() {
	Load();
}

GraphSolver::~GraphSolver() {
}

void GraphSolver::Load() {
	if(!font.loadFromFile("arial.ttf")) {
		std::cout << "Couldn't load font" << std::endl;
	}
	window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16), "B00289996 Pathfinding", sf::Style::Default, settings);
	running = true;
	// parse the dot file (only uses data required to build nodes and edges)
	std::ifstream dotFile("random64_4_1485816605.dot");
	if(dotFile.is_open()) {
		while(dotFile.good()) {
			std::string toParse;
			getline(dotFile, toParse); // read the next line of text
			if(toParse.find("fontsize") != std::string::npos) { //  only nodes and edges have the term fontsize in them
				if(toParse.find("--") == std::string::npos) { // nust be a node
					int id = std::stoi(toParse.substr(0, toParse.find_first_of('['))); 
					int index1 = toParse.find_last_of('=') + 2, index2 = toParse.find_last_of(',');
					int positionX = std::stoi(toParse.substr(index1, index2));
					index1 = toParse.find_last_of(',') + 1, index2 = toParse.find_last_of(']');
					int positionY = std::stoi(toParse.substr(index1, index2));
					nodes.push_back(std::make_shared<Node>(positionX, positionY, id));
				}
				else { // nust be an edge
					int nodeID1 = std::stoi(toParse.substr(0, toParse.find_first_of('-')));
					int nodeID2 = std::stoi(toParse.substr(toParse.find_last_of('-') + 1, toParse.find_first_of(' ')));
					int traversalCost = std::stoi(toParse.substr(toParse.find_last_of('=') + 2, toParse.find_last_of('"')));
					std::shared_ptr<Edge> edge = std::make_shared<Edge>(traversalCost, nodeID1, nodeID2);
					// assign same edge 2 both nodes
					nodes[nodeID1]->edges.push_back(edge);				
					nodes[nodeID2]->edges.push_back(edge);
				}
			}
		}
	}
	// Get the original path, store it and print its values to a text file
	aStarPath = GetPathAStar(startIndex, endIndex);
	// Print Report to file
	/*std::ofstream os;
	os.open("Result.txt");
	os << "A-Star Path (" + std::to_string(startIndex) + " - " + std::to_string(endIndex) + ")" << std::endl << GetPathAsString("\n") << std::endl << "Total Cost = " + std::to_string(GetCost()) << std::endl;
	os.close();*/
}

void GraphSolver::Update() {
	// if the Space Bar is pressed once changes to the start and end points will be allowed until the program ends
	if(!allowChanges && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) allowChanges = true;
	if(allowChanges) {
		sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		sf::Vector2f targetPosition = mousePosition - offset;
		targetPosition.y = SCREEN_HEIGHT - targetPosition.y;
		bool changed = false;
		if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) { // change the start point
			int oldStart = startIndex;
			startIndex = GetClosestNode(targetPosition);
			if(oldStart != startIndex) changed = true;
		}
		else if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { // change the end point
			int oldEnd = endIndex;
			endIndex = GetClosestNode(targetPosition);
			if(oldEnd != endIndex) changed = true;
		}
		if(changed) { // if points have changed, get a new path
			aStarPath.clear();
			aStarPath = GetPathAStar(startIndex, endIndex);
		}
	}
	
	sf::Event sfEvent;
	while(window.pollEvent(sfEvent)) {
		switch(sfEvent.type) {
		case sf::Event::Closed:
			window.close();
			running = false;
			break;
		case sf::Event::Resized:
			// store the scale relevant to the original window size
			windowScale.x = sfEvent.size.width / (float)SCREEN_WIDTH;
			windowScale.y = sfEvent.size.height / (float)SCREEN_HEIGHT;
			window.setSize(sf::Vector2u(sfEvent.size.width, sfEvent.size.height)); // Scale The Window 
			break;
		default:
			break;
		}
	}
}

void GraphSolver::Render() {
	window.clear(sf::Color::Black);
	// draw every edge 
	for(int i = 0; i < nodes.size(); i++) {
		for(int j = 0; j < nodes[i]->edges.size(); j++) {
			std::shared_ptr<Edge> e = nodes[i]->edges[j];
			sf::Vertex line [] = {
				sf::Vertex(sf::Vector2f(nodes[e->first]->x, SCREEN_HEIGHT - (nodes[e->first]->y)) + offset, sf::Color::White),
				sf::Vertex(sf::Vector2f(nodes[e->second]->x, SCREEN_HEIGHT - (nodes[e->second]->y)) + offset, sf::Color::White)
			};
			window.draw(line, 2, sf::Lines);
		}
	}
	// draw the edges from the A-Star path
	for(int a = 0; a < aStarPath.size() - 1; a++) {
		std::shared_ptr<Node> n1 = nodes[aStarPath[a]], n2 = nodes[aStarPath[a+1]];
		sf::Vector2f position1 = sf::Vector2f(n1->x, SCREEN_HEIGHT - n1->y) + offset;
		sf::Vector2f position2 = sf::Vector2f(n2->x, SCREEN_HEIGHT - n2->y) + offset;
		sf::Vertex line [] = {
			sf::Vertex(position1, sf::Color::Green),
			sf::Vertex(position2, sf::Color::Green)
		};
		window.draw(line, 2, sf::Lines);	
	}
	// draw the node numbers
	for(int i = 0; i < nodes.size(); i++) {
		DrawText(std::to_string(i), sf::Vector2f(nodes[i]->x + offset.x - 10, nodes[i]->y - offset.y));
	}
	// draw the start and end points
	DrawCircle(sf::Vector2f(nodes[startIndex]->x, SCREEN_HEIGHT - nodes[startIndex]->y) + offset, 4.0f, sf::Color::Blue);
	DrawCircle(sf::Vector2f(nodes[endIndex]->x, SCREEN_HEIGHT - nodes[endIndex]->y) + offset, 4.0f, sf::Color::Green);
	// draw text describing the path
	DrawText("A-Star Path (" + std::to_string(startIndex) + " - " + std::to_string(endIndex) + ")", sf::Vector2f(650, 500));
	DrawText(GetPathAsString(", "), sf::Vector2f(650, 450));
	DrawText("Total Cost = " + std::to_string(GetCost()), sf::Vector2f(650, 400));
	window.display();
}

std::string GraphSolver::GetPathAsString(const std::string & postFix) {
	std::string output;
	for(int a = 0; a < aStarPath.size(); a++) {
		std::shared_ptr<Node> node = nodes[aStarPath[a]];
		output += std::to_string(node->id) + (a < aStarPath.size() - 1 ? postFix : "");
	}
	return output;
}

int GraphSolver::GetCost() {
	int cost = 0;
	for(int a = 0; a < aStarPath.size() - 1; a++) {
		std::shared_ptr<Node> node = nodes[aStarPath[a]];
		cost += node->GetEdgeCost(nodes[aStarPath[a + 1]]->id); // add the cost for moving through the next edge
	}
	return cost;
}

// compares nodes based on their movement cost + distance to the destination
bool CompareNodes(const std::shared_ptr<Node> first, const std::shared_ptr<Node> second) {
	return (first->g + first->h > second->g + second->h);
}

std::vector<int> GraphSolver::GetPathAStar(const int & startNode, const int & endNode) {
	std::vector<int> waypoints;
	bool foundDestination = false; // true if the destination has been reached
	std::shared_ptr<Node> current = nodes[startNode], destination = nodes[endNode];	 
	if(current && destination) {
		std::vector<std::shared_ptr<Node>> open;
		open.push_back(current);
		while(open.size() > 0) { // there are open nodes
			current = open.back();
			int currentID = current->id;
			open.pop_back();
			if(current == destination) { // arrived at destination
				foundDestination = true;
				break;
			}
			current->isOpen = false;
			current->isClosed = true;
			
			std::vector<std::shared_ptr<Edge>> neighbours = current->edges; 
			bool nodesAdded = false;
			for(std::vector<std::shared_ptr<Edge>>::iterator i = neighbours.begin(); i != neighbours.end(); ++i) { // each edge in current node
				std::shared_ptr<Edge> edge = (*i);
				int nextID = (currentID == edge->first ? edge->second : edge->first); // get the id of the neighbour node
				std::shared_ptr<Node> next = nodes[nextID];
				if(next->isClosed) continue; // ignore the node if it's closed
				const float newCost = GetDistance(current, next) + current->g + edge->cost; // get the movement cost
				const bool inOpen = next->isOpen;
				if(newCost < next->g || !inOpen) {
					next->g = newCost;
					next->h = GetDistance(next, destination);
					next->parent = current;
					if(!inOpen) {
						open.push_back(next);
						next->isOpen = true;
					}
					if(!nodesAdded) nodesAdded = true;
				}
			}
			if(nodesAdded) std::sort(open.begin(), open.end(), CompareNodes);
		}
		if(foundDestination) { // if a path was found
			while(current) { 
				waypoints.push_back(current->id); // add each node id
				current = current->parent.lock(); // cycle back through the path
			}
			std::reverse(waypoints.begin(), waypoints.end()); // then reverse it
		}
		for each (std::shared_ptr<Node> node in nodes) { // reset all nodes
			node->isOpen = false;
			node->isClosed = false;
			node->parent = std::weak_ptr<Node>();
		}
	}
	return waypoints;
}

float GraphSolver::GetDistance(std::shared_ptr<Node> current, std::shared_ptr<Node> destination) {
	float x = (float)(destination->x - current->x), y = (float)(destination->y - current->y);
	return std::sqrt((x * x) + (y * y));
}

const bool GraphSolver::IsRunning() const {
	return running;
}

void GraphSolver::DrawText(const std::string & text, const sf::Vector2f & position, const sf::Color colour, const int & size) {
	sf::Text toDraw;
	toDraw.setFont(font);
	toDraw.setString(text);
	toDraw.setCharacterSize(size);
	toDraw.setFillColor(colour);
	toDraw.setOutlineThickness(2);
	toDraw.setOutlineColor(sf::Color::Black);
	float height = toDraw.getLocalBounds().height;
	sf::Vector2i pos = sf::Vector2i(position.x * windowScale.x, (SCREEN_HEIGHT * windowScale.y) - ((position.y + height) * windowScale.y));
	toDraw.setPosition(window.mapPixelToCoords(pos));
	window.draw(toDraw);
}

void GraphSolver::DrawCircle(const sf::Vector2f & position, const float & radius, const sf::Color colour) {
	sf::CircleShape circle;
	circle.setOrigin(radius, radius);
	circle.setPosition(position);
	circle.setRadius(radius);
	circle.setFillColor(colour);
	window.draw(circle);
}

int GraphSolver::GetClosestNode(const sf::Vector2f & position) {
	int toReturn = 0;
	float dist = std::numeric_limits<float>::max();
	for(size_t i = 0; i < nodes.size(); i++) {
		const float x = (position.x - (float)nodes[i]->x), y = (position.y - (float)nodes[i]->y);
		const float squaredDistance = (x * x) + (y * y);
		if(squaredDistance < dist) {
			dist = squaredDistance;
			toReturn = i;
		}
	}
	return toReturn;
}
