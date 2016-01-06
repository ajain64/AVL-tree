#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphvisualize.c"

// Forward declarations
void insert(struct node* aNode, int someValue);
void balanceTree(struct node* aNode, int someValue);
struct node* search(struct node* aNode, int someValue);
void deletion(int someValue);
void balanceAfterDelete(struct node* aNode);
void sort(struct node *aNode);
int getMax(int height1, int height2);
int checkBalance(struct node *aNode);
struct node* rotationRight(struct node *node1);
struct node* rotationLeft(struct node *node1);
void setHeight(struct node* aNode);
void loadCommands();
void createNewFile();


// Global variables
FILE *aFile;
struct node *root = NULL;
int count = 0;

/****************************************************************
* Main function
****************************************************************/
int main(int argc, char *argv[])
{
	aFile = fopen(argv[1], "r");
	loadCommands(); // load file with commands
	printf("\n");
	return 0;
}


/****************************************************************
* Insert value into AVL tree
****************************************************************/
void insert(struct node* aNode, int someValue)
{
	struct node *iterator = aNode; // Iterates through tree
	struct node *element = (struct node *) malloc(sizeof(struct node)); // To be inserted into tree
	
	//...................Initialize......................//
	element->value = someValue;
	element->height = 0;
	element->left = NULL;
	element->right = NULL;
	element->parent = NULL;
	//...................................................//

	//................Binary Tree Insertion..............//
	if (iterator == NULL) {
		root = element; 
		iterator = element;
	}
	else {	
		if (element->value <= iterator->value) {
			if (iterator->left != NULL) {
				insert(iterator->left, element->value);
			}
			else {
				iterator->left = element;
				element->parent = iterator;
				setHeight(iterator->left);
			}
		}
		else {
			if (iterator->right != NULL) {
				insert(iterator->right, element->value);
			}
			else {
				iterator->right = element;
				element->parent = iterator;
				setHeight(iterator->right);
			}
		}
	}
	//...................................................//
	
	//...............Balance if needed...................//
	while (iterator != NULL)
	{
		balanceTree(iterator, someValue);
		iterator = iterator->parent; 
	}
	//...................................................//
}

// Check if tree is balanced and balance if needed
void balanceTree(struct node *aNode, int someValue)
{
	struct node* rootSubTree;
	int balance = checkBalance(aNode); // check balance

	//.....................If tree is deeper on the left side.....................//
	if (balance >= 2) {
		if (someValue <= aNode->left->value) // LL case
		{
			// Rotate right
			rootSubTree = rotationRight(aNode);
			if (rootSubTree->right->parent == NULL) {
				rootSubTree->parent = NULL;
				rootSubTree->right->parent = rootSubTree;
			}
			else {
				rootSubTree->parent = rootSubTree->right->parent;
				if (rootSubTree->right->parent->right == rootSubTree->right) {
					rootSubTree->parent->right = rootSubTree; // *****
				}
				else {
					rootSubTree->parent->left = rootSubTree;
				}
				rootSubTree->right->parent = rootSubTree; 
			}

			// Set new root if necessary
			if (rootSubTree->parent == NULL) {
				root = rootSubTree;
			}
		}
		else // LR case
		{
			// Rotate left
			aNode->left = rotationLeft(aNode->left);
			aNode->left->parent = aNode;
			aNode->left->left->parent = aNode->left; 

			// Rotate right
			rootSubTree = rotationRight(aNode);
			if (rootSubTree->right->parent == NULL) // If rootSubTree's right child was the root of the tree before rotation
			{
				rootSubTree->parent = NULL;
				rootSubTree->right->parent = rootSubTree;
			}
			else {
				rootSubTree->parent = rootSubTree->right->parent;
				if (rootSubTree->right->parent->right == rootSubTree->right) {
					rootSubTree->parent->right = rootSubTree; 
				}
				else {
					rootSubTree->parent->left = rootSubTree;
				}
				rootSubTree->right->parent = rootSubTree;
			}

			// Set new root if necessary
			if (rootSubTree->parent == NULL) {
				root = rootSubTree;
			}
		}
	}
	//............................................................................//


	//.....................If tree is deeper on the right side....................//
	if (balance <= -2) // RL case
	{
		if (someValue <= aNode->right->value) 
		{
			// Rotate right
			aNode->right = rotationRight(aNode->right);
			aNode->right->parent = aNode;
			aNode->right->right->parent = aNode->right;

			// Rotate left
			rootSubTree = rotationLeft(aNode);
			if (rootSubTree->left->parent == NULL) {
				rootSubTree->parent = NULL;
				rootSubTree->left->parent = rootSubTree;
			}
			else {
				rootSubTree->parent = rootSubTree->left->parent;
				if (rootSubTree->left->parent->right == rootSubTree->left) {
					rootSubTree->parent->right = rootSubTree; 
				}
				else {
					rootSubTree->parent->left = rootSubTree;
				}
				rootSubTree->left->parent = rootSubTree;
			}

			// Set new root if necessary
			if (rootSubTree->parent == NULL) {
				root = rootSubTree;
			}
		}
		else // RR case
		{
			// Rotate left
			rootSubTree = rotationLeft(aNode);
			if (rootSubTree->left->parent == NULL) {
				rootSubTree->parent = NULL;
				rootSubTree->left->parent = rootSubTree;
			}
			else {
				rootSubTree->parent = rootSubTree->left->parent;
				if (rootSubTree->left->parent->right == rootSubTree->left) {
					rootSubTree->parent->right = rootSubTree; 
				}
				else {
					rootSubTree->parent->left = rootSubTree;
				}
				rootSubTree->left->parent = rootSubTree;
			}

			// Set new root if necessary
			if (rootSubTree->parent == NULL) {
				root = rootSubTree;
			}
		}
	}
	//............................................................................//
}

// Search for element to delete from AVL tree
struct node* search(struct node* aNode, int someValue) 
{
	struct node *iterator = aNode;
	if (iterator->value == someValue) {
		return iterator;
	}
	else if (someValue <= iterator->value) {
		if (iterator->left == NULL) {
			return NULL;
		}
		else {
			search(iterator->left, someValue);
		}
	}
	else {
		if (iterator->right == NULL) {
			return NULL;
		}
		else {
			search(iterator->right, someValue);
		}
	}
}



/****************************************************************
* Remove element from AVL tree
****************************************************************/
void deletion(int someValue)
{
	struct node* newRoot;
	struct node* smallestRightSubTree;
	struct node* smallestRightSubTreeParent;
	struct node* toRemove = search(root, someValue);
	if (toRemove != NULL)
	{
		struct node* parentOfDeleted = toRemove->parent;

		//..............Case 1: If node to delete is a leaf..............//
		if (toRemove->left == NULL && toRemove->right == NULL) 
		{
			if (toRemove == root)
			{
				root = NULL;
				free(toRemove);
				return;
			}
			else if (parentOfDeleted->left == toRemove) {
				parentOfDeleted->left = NULL;
			}
			else {
				parentOfDeleted->right = NULL;
			}

			free(toRemove);
			while (parentOfDeleted != NULL)
			{
				balanceAfterDelete(parentOfDeleted);
				parentOfDeleted = parentOfDeleted->parent;
			}
		}
		//...............................................................//

		//............Case 2: If node to delete has one child............//
		else if (toRemove->left == NULL || toRemove->right == NULL) 
		{
			// *** If root is being removed *** //
			if (toRemove == root)
			{
				// If root has only a left child
				if (toRemove->left != NULL) {
					newRoot = toRemove->left;
					toRemove->left = NULL;
				}
				// If root has only a right child
				else {
					newRoot = toRemove->right;
					toRemove->right = NULL;
				}

				// Set new root
				newRoot->parent = NULL;
				root = newRoot;
				free(toRemove);
				return;
			}

			// *** If node being removed is not the root and has a left child *** //  
			else if (toRemove->left != NULL) {
				toRemove->left->parent = parentOfDeleted;
				if (parentOfDeleted->value < toRemove->left->value) {
					parentOfDeleted->right = toRemove->left;	
				}
				else {
					parentOfDeleted->left = toRemove->left;
				}
			}

			// *** If node being removed is not the root and has a right child *** //
			else {
				toRemove->right->parent = parentOfDeleted;
				if (parentOfDeleted->value < toRemove->right->value) {
					parentOfDeleted->right = toRemove->right;	
				}
				else {
					parentOfDeleted->left = toRemove->right;
				}
			}

			// Delete node from heap and balance tree
			free(toRemove);
			while (parentOfDeleted != NULL) {
				balanceAfterDelete(parentOfDeleted);
				parentOfDeleted = parentOfDeleted->parent;
			}
		}
		//...............................................................//

		//.........Case 3: If node to delete has two children............//
		else
		{
			// Find smallest value in right subtree
			smallestRightSubTree = toRemove->right;
			while (smallestRightSubTree->left != NULL) {
				smallestRightSubTree = smallestRightSubTree->left;
			}
			smallestRightSubTreeParent = smallestRightSubTree->parent; // For use later
		
			// Set the value of the node to remove to the value of the smallest node in the right subtree of the node to remove
			toRemove->value = smallestRightSubTree->value;

			// Delete smallestRightSubTree node
			if (smallestRightSubTree->right != NULL) // if smallest of right subtree has a right child
			{
				smallestRightSubTree->right->parent = smallestRightSubTree->parent;
			}

			// See if smallestRightSubTree is a left or right child
			if (smallestRightSubTree->parent->left == smallestRightSubTree) {
				if (smallestRightSubTree->right != NULL) {
					smallestRightSubTree->parent->left = smallestRightSubTree->right;
					smallestRightSubTree->right = NULL;
				}
				else {
					smallestRightSubTree->parent->left = NULL;
				}
			}
			else {
				if (smallestRightSubTree->right != NULL) {
					smallestRightSubTree->parent->right = smallestRightSubTree->right;
					smallestRightSubTree->right = NULL;
				}
				else {
					smallestRightSubTree->parent->right = NULL;
				}
			}
			free(smallestRightSubTree);
			setHeight(smallestRightSubTreeParent); // set height of the parent of the smallest node of the right sub tree and heights of ancestors
		
			if (parentOfDeleted == NULL)
			{
				balanceAfterDelete(toRemove);
			}
			while (parentOfDeleted != NULL)
			{
				balanceAfterDelete(parentOfDeleted);
				parentOfDeleted = parentOfDeleted->parent;
			}
		}
		//...............................................................//
	}
}

// Check if tree is balanced after deletion and balance if needed
void balanceAfterDelete(struct node* aNode)
{
	struct node* rootSubTree;
	int balance = checkBalance(aNode); // check balance

	//.....................If tree is deeper on the left side.....................//
	if (balance >= 2) 
	{
		if (checkBalance(aNode->left) >= 0) // LL case
		{
			// Rotate right
			rootSubTree = rotationRight(aNode);
			if (rootSubTree->right->parent == NULL) {
				rootSubTree->parent = NULL;
				rootSubTree->right->parent = rootSubTree;
			}
			else {
				rootSubTree->parent = rootSubTree->right->parent;
				if (rootSubTree->right->parent->right == rootSubTree->right) {
					rootSubTree->parent->right = rootSubTree; // *****
				}
				else {
					rootSubTree->parent->left = rootSubTree;
				}
				rootSubTree->right->parent = rootSubTree; 
			}

			// Set new root if necessary
			if (rootSubTree->parent == NULL) {
				root = rootSubTree;
			}
		}
		else // LR case
		{
			// Rotate left
			aNode->left = rotationLeft(aNode->left);
			aNode->left->parent = aNode;
			aNode->left->left->parent = aNode->left; 

			// Rotate right
			rootSubTree = rotationRight(aNode);
			if (rootSubTree->right->parent == NULL) // If rootSubTree's right child was the root of the tree before rotation
			{
				rootSubTree->parent = NULL;
				rootSubTree->right->parent = rootSubTree;
			}
			else {
				rootSubTree->parent = rootSubTree->right->parent;
				if (rootSubTree->right->parent->right == rootSubTree->right) {
					rootSubTree->parent->right = rootSubTree; 
				}
				else {
					rootSubTree->parent->left = rootSubTree;
				}
				rootSubTree->right->parent = rootSubTree;
			}

			// Set new root if necessary
			if (rootSubTree->parent == NULL) {
				root = rootSubTree;
			}
		}
	}
	//............................................................................//


	//.....................If tree is deeper on the right side....................//
	if (balance <= -2) 
	{	
		if (checkBalance(aNode->right) > 0) // RL case
		{
			// Rotate right
			aNode->right = rotationRight(aNode->right);
			aNode->right->parent = aNode;
			aNode->right->right->parent = aNode->right;

			// Rotate left
			rootSubTree = rotationLeft(aNode);
			if (rootSubTree->left->parent == NULL) {
				rootSubTree->parent = NULL;
				rootSubTree->left->parent = rootSubTree;
			}
			else {
				rootSubTree->parent = rootSubTree->left->parent;
				if (rootSubTree->left->parent->right == rootSubTree->left) {
					rootSubTree->parent->right = rootSubTree; 
				}
				else {
					rootSubTree->parent->left = rootSubTree;
				}
				rootSubTree->left->parent = rootSubTree;
			}

			// Set new root if necessary
			if (rootSubTree->parent == NULL) {
				root = rootSubTree;
			}
		}
		else // RR case
		{
			// Rotate left
			rootSubTree = rotationLeft(aNode);
			if (rootSubTree->left->parent == NULL) {
				rootSubTree->parent = NULL;
				rootSubTree->left->parent = rootSubTree;
			}
			else {
				rootSubTree->parent = rootSubTree->left->parent;
				if (rootSubTree->left->parent->right == rootSubTree->left) {
					rootSubTree->parent->right = rootSubTree; 
				}
				else {
					rootSubTree->parent->left = rootSubTree;
				}
				rootSubTree->left->parent = rootSubTree;
			}

			// Set new root if necessary
			if (rootSubTree->parent == NULL)
			{
				root = rootSubTree;
			}
		}
	}
}



/****************************************************************
* Sort and print elements of AVL tree
****************************************************************/
void sort(struct node *aNode)
{
	struct node *iterator = aNode;

	if (iterator != NULL)
	{
		sort(iterator->left);
		printf("%d\t", iterator->value);
		sort(iterator->right);
	}		
}



/****************************************************************
* Check if tree is balanced
****************************************************************/
int checkBalance(struct node *aNode)
{
	int balance;

	if (aNode == NULL) {
        balance = 0;
	}
    else {
		balance = height(aNode->left) - height(aNode->right); 
	}

	return balance;
}


/****************************************************************
* Do a right rotation
****************************************************************/
struct node* rotationRight(struct node *node1)
{
    struct node *node2 = node1->left;
    struct node *subtree = node2->right;
 
	// Rotation
   	node2->right = node1;
    	node1->left = subtree;

	// Set new parent if subtree exists
	if(subtree != NULL)
	{
		subtree->parent = node1;
	}
 
    	// Set new heights of node1 and node2
	node1->height = getMax(height(node1->left), height(node1->right))+1;
    	node2->height = getMax(height(node2->left), height(node2->right))+1;
	setHeight(node1);
	setHeight(node2); 
    	return node2;
}


/****************************************************************
* Do a left rotation
****************************************************************/
struct node* rotationLeft(struct node *node1)
{
	struct node *node2 = node1->right;
	struct node *subtree = node2->left;
 
    	// Rotate
    	node2->left = node1;
    	node1->right = subtree;

	// Set new parent if subtree exists
	if(subtree != NULL)
	{
		subtree->parent = node1;
	}
 
    	//  Set new heights of node1 and node2
	node1->height = getMax(height(node1->left), height(node1->right)) + 1; 
    	node2->height = getMax(height(node2->left), height(node2->right)) + 1;
	setHeight(node1);
	setHeight(node2);
 
    	return node2;
}


/****************************************************************
* Get greater of two heights (left side and right side of a subtree)
****************************************************************/
int getMax(int height1, int height2)
{
	if (height1 > height2) {
		return height1;
	}
	else {
		return height2;
	}
}


/****************************************************************
* Get height of a node and return 0 is node is NULL
****************************************************************/
int height(struct node *aNode)
{
	int height;

	if (aNode == NULL) {
        height = -1;
	}
	else {
		height = aNode->height;
	}

	return height;
}


/****************************************************************
* Set heights of aNode and its ancestors
****************************************************************/
void setHeight(struct node* aNode)
{
	if (aNode->left == NULL && aNode->right == NULL)
	{
		aNode->height = 0;
	}
	while(aNode->parent != NULL)
	{
		aNode->parent->height = getMax(height(aNode->parent->left), height(aNode->parent->right)) + 1;
		aNode = aNode->parent;
	}
}


/****************************************************************
* Load contents of file with commands
****************************************************************/
void loadCommands()
{
	char someCommand[8];
	int value;
		
	if (aFile != NULL)
	{
		while (fscanf(aFile, "%s %d", someCommand, &value) != EOF) 
		{
			if (strncmp(someCommand, "insert", 6) == 0)
			{
				insert(root, value);
				createNewFile();
			}
			if (strncmp(someCommand, "delete", 6) == 0)
			{
				deletion(value);
				createNewFile();
			}
			if (strncmp(someCommand, "sort", 4) == 0)
			{
				sort(root);
			}
		}
		fclose(aFile);
	}
	else
	{
		printf("Sorry, commands could not be loaded.\n");
	}
} 


/****************************************************************
* Create graph files
****************************************************************/
void createNewFile()
{
	char intToStr[20];
	char filename[20];
	FILE *graph;

	count++;
	sprintf(intToStr, "%d", count);

	strcpy(filename, "avl");
	strcat(filename, "-");
	strcat(filename, intToStr);
	strcat(filename, ".dot");

	graph = fopen(filename, "w");
	
	fprintf(graph, "%s\n", visualize_tree(root));

	fclose(graph);
}
