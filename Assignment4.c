/*
COP 3502 Assignment 4: LOYALTY PROGRAM
© 4/3/2023 Giorgio Torregrosa 
*/

//libraries in use.
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAXLEN 19

//structs defined in assignment documentation.
typedef struct customer {
	char name[MAXLEN + 1];
	int points;
} customer;

typedef struct treenode {
	customer* cPtr;
	int size;
	struct treenode* left;
	struct treenode* right;
} treenode;

/*
most of these functions were heavily sourced from Guha's publicly available lecture notes.
credit: https://www.cs.ucf.edu/~dmarino/ucf/cop3502h/spr24/material/ 
with slight motifications I implented these functions as much as possible into my program below.
*/
treenode* createNode(const char* name, int points); //dynamically creates a node to be inserted into the BST.
treenode* insert(treenode* root, treenode* element); //inserts a newly allocated node into the BST in such a way that it balances lexicographically. 
treenode* findNode(struct treenode* root, char* name); //finds a particular node and returns it.
treenode* delete(struct treenode* root, char* name); //deletes a given node and returns the root of the newly balanced BST
treenode* maxVal(treenode* root); //determines the largest lexicographic node in a given root's subtree
int countSmaller(treenode* root, char* name); //determines how many nodes come before a given node lexicographically
void collectCustomers(treenode* root, treenode** array, int* index); //utility function to store the contents of a BST into an array of pointers to nodes
void mergeSortCustomerArray(treenode** array, int l, int r); //utility function that sorts the array of struct pointers by the order from highest to lowest loyalty points (merge sort)

//easy global variable for depth calculations.
int globalDepthCounter = 0;

//START
int main(int argc, char* argv[]) {

	//local variable in main to help contruct the BST and later to sort it.
	treenode* root = NULL;
	treenode* tmp = NULL;
	int totalCommands = 0;
	char command[15] = { NULL };
	char tempName[20] = { NULL };
	int tempPoints = 0;
	int counts = 0;

	//scan in how many commands are to be looped through and at each iteration use strcmp to figure out which command is to be executed. 
	if (scanf("%d", &totalCommands) != 1) {
		return EXIT_FAILURE;
	};
	for (int i = 0; i < totalCommands; i++) {

		if (scanf("%s", &command) != 1) {
			return EXIT_FAILURE;
		};
		//ADD COMMAND
		if (strcmp(command, "add") == 0) {	
			if (scanf("%s%d", &tempName, &tempPoints) != 2) {
				return EXIT_FAILURE;
			};
			tmp = findNode(root, tempName);
			if (tmp == NULL) {
				tmp = createNode(tempName, tempPoints);
				root = insert(root, tmp);
			}
			else {
				tmp->cPtr->points += tempPoints;
			}
			printf("%s %d\n", tmp->cPtr->name, tmp->cPtr->points);
		}
		//SUB COMMAND
		else if (strcmp(command, "sub") == 0) {
			if (scanf("%s%d", &tempName, &tempPoints) != 2) {
				return EXIT_FAILURE;
			};
			tmp = findNode(root, tempName);
			if (tmp == NULL) {
				printf("%s not found\n", tempName);
			}
			else {
				tmp->cPtr->points -= tempPoints;
				if (tmp->cPtr->points < 0) { //if the points they have go negative, default the points to zero
					tmp->cPtr->points = 0;
				}
				printf("%s %d\n", tmp->cPtr->name, tmp->cPtr->points);
			}
		}
		//DEL COMMAND
		else if (strcmp(command, "del") == 0) {
			if (scanf("%s", &tempName) != 1) {
				return EXIT_FAILURE;
			};
			tmp = findNode(root, tempName);
			if (tmp == NULL) {
				printf("%s not found\n", tempName);
			}
			else {
				root = delete(root, tempName);
				printf("%s deleted\n", tempName);
			}
		}
		//SEARCH COMMAND
		else if (strcmp(command, "search") == 0) {
			if (scanf("%s", &tempName) != 1) {
				return EXIT_FAILURE;
			}
			globalDepthCounter = 0;
			tmp = findNode(root, tempName);
			if (tmp != NULL) {	
				printf("%s %d %d\n", tmp->cPtr->name, tmp->cPtr->points, globalDepthCounter);
			}
			else {
				printf("%s not found\n", tempName);
			}

		}
		//COUNT_SMALLER COMMAND
		else if (strcmp(command, "count_smaller") == 0) {
			if (scanf("%s", &tempName) != 1) {
				return EXIT_FAILURE;
			};
			counts = 0;
			tmp = findNode(root, tempName);
			counts = countSmaller(root, tempName);
			printf("%d\n", counts);
		}
	}

	// Allocate an array of pointers to treenode with size equal to the size of the binary tree
	if (root != NULL) {
		treenode** customerArray = (treenode**)malloc(sizeof(treenode*) * root->size);
		if (!customerArray) {
			return EXIT_FAILURE;
		}

		int index = 0; // This will keep track of the next insert position in the array
		collectCustomers(root, customerArray, &index);

		//mergeSort by points
		mergeSortCustomerArray(customerArray, 0, root->size - 1);
		for (int i = 0; i < root->size; i++) {
			printf("%s %d\n", customerArray[i]->cPtr->name, customerArray[i]->cPtr->points);
		}
	}
	return 0; //all groovy

}//end main

//CREATE NODE
treenode* createNode(const char* name, int points) {

	treenode* temp = (treenode*)malloc(sizeof(treenode));
	if (temp == NULL) {
		// Memory allocation failed
		return NULL;
	}
	// Allocate memory for customer
	temp->cPtr = (customer*)malloc(sizeof(customer));
	if (temp->cPtr == NULL) {
		// If allocation for customer fails, clean up and return NULL
		free(temp);
		return NULL;
	}
	// Copy the provided name and points into the customer struct
	strncpy(temp->cPtr->name, name, MAXLEN);
	temp->cPtr->name[MAXLEN] = '\0'; // Ensure the string is null-terminated
	temp->cPtr->points = points;
	temp->left = NULL;
	temp->right = NULL;

	return temp;
}

//INSERT NODE
treenode* insert(treenode* root, treenode* element) {
	
	if (root == NULL) {
		element->size = 1; // A new node starts with a size of 1, itself.
		return element;
	}
	// Decide whether to insert to the left or right of the current node based off the first letter of the name only, this was 
	if (element->cPtr->name[0] <= root->cPtr->name[0]) {
		root->left = insert(root->left, element);
	}
	else {
		root->right = insert(root->right, element);
	}
	// After insertion, update the size of the current node
	root->size = 1; // Start with 1 for the current node
	if (root->left != NULL) {
		root->size += root->left->size; // Add the size of the left subtree
	}
	if (root->right != NULL) {
		root->size += root->right->size; // Add the size of the right subtree
	}

	return root;
}

//FIND NODE
treenode* findNode(struct treenode* root, char* name) {

	// Check if there are nodes in the tree.
	if (root != NULL) {

		// Found the value at the root.
		if (strcmp(root->cPtr->name, name) == 0)
			return root;

		// Search to the left.
		if (root->cPtr->name[0] >= name[0]) {
			globalDepthCounter++;
			return findNode(root->left, name);
		}
		// Or...search to the right.
		else {
			globalDepthCounter++;
			return findNode(root->right, name);
		}
	}
	else
		return NULL; // No node found.
}

//MAX VAL NODE
treenode* maxVal(treenode* root) {

	// Root stores the maximal value.
	if (root->right == NULL)
		return root;

	// The right subtree of the root stores the maximal value.
	else
		return maxVal(root->right);
}

//DEL NODE
treenode* delete(treenode* root, char* name) {
	
	if (root == NULL) {
		// Base case
		return NULL;
	}
	// Navigate to the node to be deleted based on the first letter comparison.
	if (name[0] < root->cPtr->name[0]) {
		root->left = delete(root->left, name);
	}
	else if (name[0] > root->cPtr->name[0]) {
		root->right = delete(root->right, name);
	}
	else {
		if (strcmp(root->cPtr->name, name) == 0) {

			if (root->left == NULL) {
				treenode* temp = root->right;
				free(root->cPtr);
				free(root);
				return temp;
			}
			else if (root->right == NULL) {
				treenode* temp = root->left;
				free(root->cPtr);
				free(root);
				return temp;
			}

			// Node with two children: Get the inorder predecessor (largest in the left subtree)
			treenode* temp = maxVal(root->left);

			// Copy the inorder predecessor's content to this node
			strcpy(root->cPtr->name, temp->cPtr->name);
			root->cPtr->points = temp->cPtr->points;

			// Delete the inorder predecessor
			root->left = delete(root->left, temp->cPtr->name);
		}
		else {
			root->right = delete(root->right, name);
		}
	}
	// After deletion, update the size of the current node
	if (root != NULL) {
		root->size = 1; // Start with 1 for the current node itself
		if (root->left != NULL) {
			root->size += root->left->size; // Add the size of the left subtree
		}
		if (root->right != NULL) {
			root->size += root->right->size; // Add the size of the right subtree
		}
	}
	return root;
}

int countSmaller(treenode* root, char* name) {
	int count = 0;
	while (root != NULL) {
		// If the current node's name is lexicographically greater or equal to the target name,
		// move to the left subtree to find smaller names.
		if (strcmp(root->cPtr->name, name) >= 0) {
			root = root->left;
		}
		else {
			// If the current node's name is lexicographically smaller than the target name,
			// all nodes in the current node's left subtree plus the current node itself are counted.
			// Then move to the right subtree to continue searching.
			if (root->left != NULL) {
				count += root->left->size;
			}
			count += 1; // for the current node itself
			root = root->right;
		}
	}
	return count;
}

void collectCustomers(treenode* root, treenode** array, int* index) {
	if (root == NULL) {
		return;
	}
	// Traverse left subtree
	collectCustomers(root->left, array, index);
	// Store the current node's customer info
	array[*index] = root;
	(*index)++;
	// Traverse right subtree
	collectCustomers(root->right, array, index);
}

//MERGE SORT
void mergeSortCustomerArray(treenode** array, int l, int r) {
	if (l < r) {
		int m = l + (r - l) / 2;

		mergeSortCustomerArray(array, l, m);
		mergeSortCustomerArray(array, m + 1, r);

		int n1 = m - l + 1;
		int n2 = r - m;

		treenode** L = malloc(n1 * sizeof(treenode*));
		if (!L) {
			return;
		}
		treenode** R = malloc(n2 * sizeof(treenode*));
		if (!R) {
			return;
		}
		for (int i = 0; i < n1; i++)
			L[i] = array[l + i];
		for (int j = 0; j < n2; j++)
			R[j] = array[m + 1 + j];

		int i = 0, j = 0, k = l;
		while (i < n1 && j < n2) {
			// Here we check if the left array points are greater OR points are equal and left array name is lexicographically smaller
			if (L[i]->cPtr->points > R[j]->cPtr->points || (L[i]->cPtr->points == R[j]->cPtr->points && strcmp(L[i]->cPtr->name, R[j]->cPtr->name) < 0)) {
				array[k] = L[i];
				i++;
			}
			else {
				array[k] = R[j];
				j++;
			}
			k++;
		}
		// Copy the remaining elements of L[] and R[], if there are any
		while (i < n1) {
			array[k] = L[i];
			i++;
			k++;
		}
		while (j < n2) {
			array[k] = R[j];
			j++;
			k++;
		}
		free(L);
		free(R);
	}
}
