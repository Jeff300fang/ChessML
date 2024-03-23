#pragma once
#include "Imports.h"

class SinglyNode {
public:
	BoardPiece pieceVal;
	std::shared_ptr<SinglyNode> next;
	SinglyNode(BoardPiece val);
};

class SinglyLinkedList
{
public:
	SinglyLinkedList();
	void insert(BoardPiece insertVal);
	void remove(BoardPiece removeVal);
	void displaySinglyLinkedList();
	BoardPiece getHead();

	class SinglyIterator {
	public:
		SinglyIterator(std::shared_ptr<SinglyNode> node) : currentNode(node) {}
		BoardPiece& operator*();
		SinglyIterator& operator++();
		bool operator!=(const SinglyIterator& other) const;
	private:
		std::shared_ptr<SinglyNode> currentNode;
	};

	SinglyIterator begin();
	SinglyIterator end();

	bool isEqual(const SinglyLinkedList& other) const;
	std::shared_ptr<SinglyNode> head;
};

