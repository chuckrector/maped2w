#include "undo.h"

UndoManager::UndoManager(): position(0) {
	//...
}

UndoManager::~UndoManager() {
	//...
}

void UndoManager::addUndoableEdit(AbstractUndoableEdit* edit) {
	edits.push_back(edit);
	++position;
}

void UndoManager::undo() {
	if (canUndo()) {
		edits[--position]->undo();
	}
}

void UndoManager::redo() {
	if (canRedo()) {
		edits[position++]->redo();
	}
}

bool UndoManager::canUndo() {
	//...
	return false;
}

bool UndoManager::canRedo() {
	//...
	return false;
}