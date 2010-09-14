#ifndef UNDO_INC
#define UNDO_INC

#include <vector>

class AbstractUndoableEdit {
public:
	virtual void undo() = 0;
	virtual void redo() = 0;
};

class UndoManager {
	std::vector<AbstractUndoableEdit*> edits;
	int position;
public:
	UndoManager();
	~UndoManager();

	void addUndoableEdit(AbstractUndoableEdit* edit);

	void undo();
	void redo();
	bool canUndo();
	bool canRedo();
};

#endif // UNDO_INC