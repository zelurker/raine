#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "bld.h"
#include "games.h"
#include "files.h"

int use_bld,bld1,bld2;

/* This balanced binary tree algorythm is taken from
 * http://www.geeksforgeeks.org/sorted-array-to-balanced-bst/
 * nice and short ! */

typedef struct {
    long start,end;
    int alpha;
} elem;

/* A Binary Tree node */
struct TNode
{
    elem data;
    struct TNode* left;
    struct TNode* right;
};

static struct TNode *node;

static struct TNode* newNode(elem data)
{
    struct TNode* node = (struct TNode*)
                         malloc(sizeof(struct TNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int get_spr_alpha(long value) {
    if (!use_bld) return 0;
    struct TNode *nd = node;
    while (nd) {
	if (nd->data.start <= value && nd->data.end >= value) {
	    return nd->data.alpha;
	}
	if (value < nd->data.start)
	    nd = nd->left;
	else
	    nd = nd->right;
    }
    return 0;
}

static void free_tree(struct TNode *node) {
    if (node) {
	if (node->left)
	    free_tree(node->left);
	if (node->right)
	    free_tree(node->right);
	free(node);
    }
}

void free_alpha_tree() {
    free_tree(node);
    node = NULL;
}

/* A function that constructs Balanced Binary Search Tree from a sorted array */
static struct TNode* sortedArrayToBST(elem arr[], int start, int end)
{
    /* Base Case */
    if (start > end)
      return NULL;

    /* Get the middle element and make it root */
    int mid = (start + end)/2;
    struct TNode *root = newNode(arr[mid]);

    /* Recursively construct the left subtree and make it
       left child of root */
    root->left =  sortedArrayToBST(arr, start, mid-1);

    /* Recursively construct the right subtree and make it
       right child of root */
    root->right = sortedArrayToBST(arr, mid+1, end);

    return root;
}

void read_bld() {
    char bld[FILENAME_MAX];
    sprintf(bld,"blend/%s.bld",current_game->main_name);
    FILE *f = fopen(get_shared(bld),"r");
    if (!f) return ;
    elem *tab = NULL;
    int alloc = 0, used = 0, started = 0;
    if (node) // in case of reset or change of region !
	free_tree(node);
    while (!feof(f)) {
	char buf[80];
	fgets(buf,80,f);
	int len = strlen(buf)-1;
	while (buf[len] < 32 && len > 0)
	    buf[len--] = 0;
	if (!started) {
	    if (!strncmp(buf,"Game ",5))
		started = 1;
	} else {
	    if (!strncmp(buf,"Name",4))
		continue;
	    char *end;
	    elem a;
	    a.start = strtol(buf,&end,16);
	    if (used && a.start == tab[used-1].start)
		continue;
	    if (*end == '-')
		a.end = strtol(end+1,&end,16);
	    else
		a.end = a.start;
	    if (*end == ' ')
		a.alpha = atoi(end+1);
	    else
		a.alpha = 0;
	    if (a.alpha == 1) // 25% transparent
		a.alpha = 255*(100-bld1)/100;
	    else if (a.alpha == 2) // 50% transparent
		a.alpha = 255*bld2/100;
	    else {
		if (a.alpha != 0) {
		    printf("read_bld: found a.alpha = %d\n",a.alpha);
		    a.alpha = 0; // otherwise no transparency at all
		}
	    }
	    if (used == alloc) {
		alloc += 10;
		tab = realloc(tab,sizeof(elem )*alloc);
	    }
	    tab[used++] = a;
	}
    }
    fclose(f);

    node = sortedArrayToBST(tab,0,used-1);
    free(tab);
}

