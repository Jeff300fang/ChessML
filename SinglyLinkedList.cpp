#include "SinglyLinkedList.h"
SinglyNode::SinglyNode(BoardPiece val) {
	pieceVal = val;
	next = nullptr;
}
SinglyLinkedList::SinglyLinkedList() {
	head = nullptr;
}
void SinglyLinkedList::insert(BoardPiece insertVal) {
	if (insertVal.pieceType == -1) {
		throw std::runtime_error("Attempted to insert blank square into SinglyLinkedList");
	}
	std::shared_ptr<SinglyNode> newNode = std::make_shared<SinglyNode>(insertVal);
	if (head == nullptr) {
		head = newNode;
		return;
	}
	if (newNode->pieceVal.pieceWeight > head->pieceVal.pieceWeight) {
		newNode->next = head;
		head = newNode;
		return;
	}
	std::shared_ptr<SinglyNode> currentNode = head;
	while (currentNode->next) {
		if (newNode->pieceVal.pieceWeight > currentNode->next->pieceVal.pieceWeight) {
			newNode->next = currentNode->next;
			currentNode->next = newNode;
			return;
		}
		if (newNode->pieceVal.pieceWeight == currentNode->next->pieceVal.pieceWeight
			&& newNode->pieceVal.rowCoord < currentNode->next->pieceVal.rowCoord) {
			newNode->next = currentNode->next;
			currentNode->next = newNode;
			return;
		}
		if (newNode->pieceVal.pieceWeight == currentNode->next->pieceVal.pieceWeight
			&& newNode->pieceVal.rowCoord == currentNode->next->pieceVal.rowCoord
			&& newNode->pieceVal.colCoord < currentNode->next->pieceVal.colCoord) {
			newNode->next = currentNode->next;
			currentNode->next = newNode;
			return;
		}
		currentNode = currentNode->next;
	}
	currentNode->next = newNode;
	return;
}
void SinglyLinkedList::remove(BoardPiece removeVal) {
	if (head == nullptr) {
		throw std::runtime_error("Tried removing from empty singlylinkedlist");
	}
	if (head->pieceVal.rowCoord == removeVal.rowCoord
		&& head->pieceVal.colCoord == removeVal.colCoord) {
		head = head->next;
		return;
	}
	std::shared_ptr<SinglyNode> current = head;
	while (current->next) {
		if (current->next->pieceVal.rowCoord == removeVal.rowCoord
			&& current->next->pieceVal.colCoord == removeVal.colCoord) {
			current->next = current->next->next;
			return;
		}
		current = current->next;
	}
	if (current->pieceVal.rowCoord == removeVal.rowCoord
		&& current->pieceVal.colCoord == removeVal.colCoord) {
		current = nullptr;
		return;
	}
	throw std::runtime_error("Cannot remove from singlylinked list" + removeVal.toString());

}
void SinglyLinkedList::displaySinglyLinkedList() {
	std::shared_ptr<SinglyNode> current = head;
	while (current) {
		std::cout << current->pieceVal.toString() << " ";
		current = current->next;
	}
	std::cout << std::endl;
	return;
}
BoardPiece SinglyLinkedList::getHead() {
	return(head->pieceVal);
}

BoardPiece& SinglyLinkedList::SinglyIterator::operator*() {
	return currentNode->pieceVal;
}
SinglyLinkedList::SinglyIterator& SinglyLinkedList::SinglyIterator::operator++() {
	currentNode = currentNode->next;
	return *this;
}
bool SinglyLinkedList::SinglyIterator::operator!=(const SinglyIterator& other) const {
	return (currentNode != other.currentNode);
}
SinglyLinkedList::SinglyIterator SinglyLinkedList::begin(){
	return(SinglyIterator(head));
}
SinglyLinkedList::SinglyIterator SinglyLinkedList::end() {
	return(SinglyIterator(nullptr));
}
bool SinglyLinkedList::isEqual(const SinglyLinkedList& other) const {
	std::shared_ptr<SinglyNode> currentNode1 = head;
	std::shared_ptr<SinglyNode> currentNode2 = other.head;

	while (currentNode1 && currentNode2) {
		if (currentNode1->pieceVal.pieceType != currentNode2->pieceVal.pieceType
			|| currentNode1->pieceVal.isComputerPiece != currentNode2->pieceVal.isComputerPiece
			|| currentNode1->pieceVal.rowCoord != currentNode2->pieceVal.rowCoord
			|| currentNode1->pieceVal.colCoord != currentNode2->pieceVal.colCoord) {
			return(false);
		}
		if ((currentNode1->pieceVal.pieceType == 'K' || currentNode1->pieceVal.pieceType == 'R')
			&& currentNode1->pieceVal.hasMoved != currentNode2->pieceVal.hasMoved) {
			return(false);
		}
		currentNode1 = currentNode1->next;
		currentNode2 = currentNode2->next;
	}
	if (currentNode1 == nullptr && currentNode2 == nullptr) {
		return(true);
	}
	return(false);
}
