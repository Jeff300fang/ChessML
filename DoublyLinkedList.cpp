#include "DoublyLinkedList.h"
Node::Node(BoardPiece value) {
	pieceVal = value;
	prev = nullptr;
	next = nullptr;
}
DoublyLinkedList::DoublyLinkedList() {
	head = nullptr;
	computerKing = nullptr;
	playerKing = nullptr;
}
void DoublyLinkedList::insert(BoardPiece val) {
	std::shared_ptr<Node> newNode = std::make_shared<Node>(val);
	//if king, set king pointer to new node
	if (newNode->pieceVal.pieceType == 'K') {
		if (newNode->pieceVal.isComputerPiece) {
			computerKing = newNode;
		}
		else {
			playerKing = newNode;
		}
	}
	//if linked list empty (head nullptr) set head to new node
	if (!head) {
		head = newNode;
		return;
	}
	//insert in correct order based on board coordinates
	std::shared_ptr<Node> currentNode = head;
	if (newNode->pieceVal.rowCoord < currentNode->pieceVal.rowCoord || (newNode->pieceVal.rowCoord == currentNode->pieceVal.rowCoord && newNode->pieceVal.colCoord < currentNode->pieceVal.colCoord)){ 
		newNode->next = head;
		head->prev = newNode;
		head = newNode;
		return;
	}
	std::shared_ptr<Node> temp;
	while (currentNode->next) {
		//compare current
		//something is wrong here

		if (newNode->pieceVal.rowCoord < currentNode->pieceVal.rowCoord || (newNode->pieceVal.rowCoord == currentNode->pieceVal.rowCoord && newNode->pieceVal.colCoord < currentNode->pieceVal.colCoord)) {
			newNode->next = currentNode;
			newNode->prev = currentNode->prev;
			temp = currentNode->prev;
			temp->next = newNode;
			currentNode->prev = newNode;
			return;
		}
		currentNode = currentNode->next;
	}
	//last node
	if (newNode->pieceVal.rowCoord < currentNode->pieceVal.rowCoord || (newNode->pieceVal.rowCoord == currentNode->pieceVal.rowCoord && newNode->pieceVal.colCoord < currentNode->pieceVal.colCoord)) {
		newNode->prev = currentNode->prev;
		newNode->next = currentNode;
		//currentNode->prev->next = newNode;
		newNode->prev->next = newNode;
		currentNode->prev = newNode;
		return;
	}
	currentNode->next = newNode;
	newNode->prev = currentNode;
	return;
}
void DoublyLinkedList::remove(BoardPiece val) {
	if (head == nullptr) {
		throw std::runtime_error("Tried removing from empty array: " + val.toString());
	}
	//removing pointer if kings
	if (val.pieceType == 'K') {
		if (val.isComputerPiece) {
			computerKing = nullptr;
		}
		else {
			playerKing = nullptr;
		}
	}
	//check if head is the one we want to remove
	if (head->pieceVal.rowCoord == val.rowCoord && head->pieceVal.colCoord == val.colCoord) {
		//remove head
		head = head->next;
		if (head) {
			head->prev = nullptr;
		}
		return;
	}
	std::shared_ptr<Node> currentNode = head->next;
	while (currentNode) {
		if (currentNode->pieceVal.rowCoord == val.rowCoord && currentNode->pieceVal.colCoord == val.colCoord) {
			currentNode->prev->next = currentNode->next;
			//if last node in list, currentNode->next=nullptr
			if (currentNode->next) {
				currentNode->next->prev = currentNode->prev;
			}
			//HERE IF MEMORY LEAK
			return;
		}
		currentNode = currentNode->next;
	}
	throw std::runtime_error("Item not it linked list on removal" + val.toString());
}
void DoublyLinkedList::displayDoublyLinkedList() {
	std::shared_ptr<Node> current = head;
	while (current) {
		std::cout << current->pieceVal.toString() << " ";
		current = current->next;
	}
	return;
}
std::shared_ptr<Node> DoublyLinkedList::getKing(bool wantComputerKing) {
	if (wantComputerKing) {
		return(computerKing);
	}
	return(playerKing);
}
BoardPiece& DoublyLinkedList::Iterator::operator*() {
	return currentNode->pieceVal;
}
DoublyLinkedList::Iterator& DoublyLinkedList::Iterator::operator++() {
	currentNode = currentNode->next;
	return *this;
}
bool DoublyLinkedList::Iterator::operator!=(const Iterator& other) const {
	return (currentNode != other.currentNode);
}
DoublyLinkedList::Iterator DoublyLinkedList::begin() {
	return(Iterator(head));
}
DoublyLinkedList::Iterator DoublyLinkedList::end() {
	return(Iterator(nullptr));
}