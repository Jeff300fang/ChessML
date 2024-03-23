#pragma once
#include "BoardPiece.h"
#include "Imports.h"

class Node {
public:
	BoardPiece pieceVal;
	std::shared_ptr<Node> prev;
	std::shared_ptr<Node> next;
	Node(BoardPiece val);
};

class DoublyLinkedList
{
public:
	DoublyLinkedList();
	void insert(BoardPiece val);
	void remove(BoardPiece val);
	void displayDoublyLinkedList();
	std::shared_ptr<Node> getKing(bool wantComputerKing);

	class Iterator {
	public:
		Iterator(std::shared_ptr<Node> node) : currentNode(node) {}
		BoardPiece& operator*();
		Iterator& operator++();
		bool operator!=(const Iterator& other) const;
	private:
		std::shared_ptr<Node> currentNode;
	};

	Iterator begin();
	Iterator end();

private:
	std::shared_ptr<Node> head;
	std::shared_ptr<Node> computerKing;
	std::shared_ptr<Node> playerKing;
};

