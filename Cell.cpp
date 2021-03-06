#include "Cell.h"

#include "FactoryState.h"

Cell::Cell(FactoryState *state, bool useConnections) : factory(state), canReceive(all), useConnections(useConnections) {

}

Cell::~Cell() {
	if (box) {
		delete box;
	}
}

void Cell::preTick() {
	if (nextBox) {
		nextBox->slideTo(factory->gridToScreenPosition(gridPosition));
	}
}

void Cell::processTick() {
	box = nextBox;
	if (box) {
		box->setPosition(factory->gridToScreenPosition(gridPosition));
	}
}

bool Cell::takeTick() {
	if (!nextBox) {
		for (Direction direction : takeFrom.listEnabled()) {
			Cell *otherCell = factory->getCellAtGridPosition(gridPosition + directionToVector(direction));
			if (otherCell && otherCell->box && otherCell->nextBox == otherCell->box && (!filtered || otherCell->box->getColor() == filter)) {
				nextBox = otherCell->box;
				otherCell->nextBox = nullptr;
				return true;
			}
		}
		return false;
	}
	else {
		return false;
	}
}

bool Cell::giveTick() {
	if (box && nextBox == box) {
		for (Direction direction : giveTo.listEnabled()) {
			Cell *otherCell = factory->getCellAtGridPosition(gridPosition + directionToVector(direction));
			if (otherCell && otherCell->canReceiveFrom(flipDirection(direction)) && (!otherCell->filtered || box->getColor() == otherCell->filter)) {
				otherCell->nextBox = box;
				nextBox = nullptr;
				return true;
			}
		}
		return false;
	}
	else {
		return false;
	}
}

void Cell::update(sf::Time elapsed) {
	if (box) {
		box->update(elapsed);
	}
}

void Cell::setGridPosition(sf::Vector2i gridPosition) {
	this->gridPosition = gridPosition;
	updateGraphics();
}

void Cell::setGridPosition(int x, int y) {
	setGridPosition(sf::Vector2i(x, y));
}

void Cell::updateGraphics() {
	// Find connections
	if (useConnections) {
		connections.disable(all);
		Cell *cellUp = factory->getCellAtGridPosition(gridPosition + directionToVector(up));
		Cell *cellRight = factory->getCellAtGridPosition(gridPosition + directionToVector(right));
		Cell *cellDown = factory->getCellAtGridPosition(gridPosition + directionToVector(down));
		Cell *cellLeft = factory->getCellAtGridPosition(gridPosition + directionToVector(left));
		if (cellUp && cellUp->giveTo.isEnabled(down)) {
			connections.enable(up);
		}
		if (cellRight && cellRight->giveTo.isEnabled(left)) {
			connections.enable(right);
		}
		if (cellDown && cellDown->giveTo.isEnabled(up)) {
			connections.enable(down);
		}
		if (cellLeft && cellLeft->giveTo.isEnabled(right)) {
			connections.enable(left);
		}
	}
}

bool Cell::canReceiveFrom(int directions) const {
	return !dying && !nextBox && canReceive.isEnabled(directions);
}

void Cell::drawBox(sf::RenderTarget &target) const {
	if (box) {
		target.draw(*box);
	}
}

void Cell::destroy() {
	dying = true;
}

bool Cell::shouldDie() const {
	return dying && !box && !nextBox;
}

void Cell::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	if (useConnections) {
		sf::Sprite connectingSprite;
		connectingSprite.setTexture(factory->loadTexture("Resource/Image/Connection.png"));
		connectingSprite.setPosition(factory->gridToScreenPosition(gridPosition));
		if (connections.isEnabled(up)) {
			connectingSprite.setTextureRect(sf::IntRect(0, 0, 16, 12));
			target.draw(connectingSprite);
		}
		if (connections.isEnabled(right)) {
			connectingSprite.setTextureRect(sf::IntRect(0, 12, 16, 12));
			target.draw(connectingSprite);
		}
		if (connections.isEnabled(down)) {
			connectingSprite.setTextureRect(sf::IntRect(0, 24, 16, 12));
			target.draw(connectingSprite);
		}
		if (connections.isEnabled(left)) {
			connectingSprite.setTextureRect(sf::IntRect(0, 36, 16, 12));
			target.draw(connectingSprite);
		}
	}
}
