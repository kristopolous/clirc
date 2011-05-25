//TODO: Add the oh so difficult tree node deletion 
//	Splaying
#ifndef _COMMAND_
#define _COMMAND_
#include"../defines.h"

int ccomlong;
int clres;
//ret is a pointer to the function that it found
//command is the text to search for the command
//com is the set of commands to search through
//res is how many commands were found
int _findcom(struct ccom*com, char*command, struct ccom**ret)
{	
	if(com == NULL)
	{
		return(0);
	}
	if(com->left)
	{
		_findcom(com->left, command, ret);
	}
	if(!strncmp(com->name, command, strlen(command)))
	{
		if(clres == 1)	//The last command did not uniquely match
		{
			message("WARNING: Ambiguous Command!", RED, 0, 0);
			printvalue((*ret)->name, (*ret)->description, QLIN, QMAR);	
		}
		if(clres>0)	//Nor did this one.
		{
			printvalue(com->name, com->description, QLIN, QMAR);	
		}
		(*ret) = com;
		clres++;
	}
	if(com->right)	//Tree traversal
	{
		_findcom(com->right, command, ret);
	}
	if(com != com->head)	//Root node exception
	{
		com = com->head;
	}
	return(clres);
}

int findcom(struct ccom*com, char*command, struct ccom**ret)
{	
	clres = 0;
	return(_findcom(com, command, ret));
}

//Without this working it's incomplete
/*int delcom(struct ccom**com, char*name)
{	struct ccom*todelete, *temp;
	if(!findcom(com, name, &todelete));
		return(0);//No such command
	if(todelete->head != todelete)//If not the root
	{	if(todelete == todelete->head->left)//if left
		{	if(todelete->left)
			{	todelete->head->left = todelete->left;
				if(todelete->right)
				{	temp = todelete->right;
					while(temp)
						temp = temp->left;
					temp->left = 
			else if(todelete->right)
				todelete->head->left = todelete->right;
		}else
		if(todelete == todelete->head->right)
		{	if(todelete->left)
				todelete->head->right = todelete->left;
			else if(todelete->right)
				todelete->head->right = todelete->right;
		}	//Otherwise we are the bottom node
	}
		
}
*/

struct ccom*addcom(
	struct ccom**com, 
	char*name, 
	int(*function)(void*), 
	void*arg, 
	char*desc)
{	
	struct ccom*temp = (*com), 
		*prev = (*com);	
	char flag = 0;

	int res;

	if(strlen(name) > ccomlong)
	{
		ccomlong = strlen(name);
	}
	while(1)	
	{	
		if(temp == NULL)
		{
			break;	
		}
		res = strcmp(temp->name, name);	
		if(res > 0)
		{
			prev = temp;
			temp = temp->left;
			flag = 1;
		}
		else if(res < 0)
		{
			prev = temp;
			temp = temp->right;
			flag = 2;
		}
		else
		{
			break;
		}
	}
	temp		 = (struct ccom*)malloc(sizeof(struct ccom));
	temp->name	 = (char*)malloc(strlen(name)+1);
	temp->left	 = NULL;
	temp->right	 = NULL;
	temp->function	 = function;
	temp->head	 = prev;
	temp->description = NULL;
	temp->arg	 = arg;
	temp->sub	 = NULL;

	if(desc)
	{	
		temp->description = (char*)malloc(strlen(desc) + 1);
		strcpy(temp->description, desc);
	}
	memcpy(temp->name, name, strlen(name));

	if(flag == 1)
	{
		prev->left = temp;
	}
	else if(flag == 2)
	{
		prev->right = temp;
	}
	else
	{
		(*com) = temp;
	}
	return(temp);
}

//Adds a subshell instead of a command
struct ccom*addsub(struct ccom**com, char*name, struct ccom*sub, struct qval*vars)
{	
	struct ccom*temp = (*com), 
		*prev = (*com);	
	char flag = 0;
	int res;
//Longest name
	if(strlen(name) > ccomlong)
	{
		ccomlong = strlen(name);
	}
	while(1)	
	{
		if(temp == NULL)
		{
			break;	
		}
		res = strcmp(temp->name, name);	

		if(res>0)
		{
			prev = temp;
			temp = temp->left;
			flag = 1;
		}
		else if(res<0)
		{
			prev = temp;
			temp = temp->right;
			flag = 2;
		}
		else
			break;
	}

	temp		 = (struct ccom*)malloc(sizeof(struct ccom));
	temp->name	 = (char*)malloc(strlen(name)+1);
	temp->left	 = NULL;
	temp->right	 = NULL;
	temp->function	 = subshell;
	temp->head	 = prev;
	temp->arg	 = temp;
	temp->sub	 = sub;
	temp->subvars	 = vars;
//STILL Needs to change
	temp->description = (char*)malloc(strlen(name)+10);
	strcpy(temp->description, "SUBSHELL ");
	strcat(temp->description, name);
	memcpy(temp->name, name, strlen(name));

	if(flag == 1)
	{
		prev->left = temp;
	}
	else if(flag == 2)
	{
		prev->right = temp;
	}
	else
	{
		(*com) = temp;
	}
	return(temp);
}

int showcom(struct ccom*com)
{
	int margin = ccomlong+3;	

	if(com == NULL)
	{
		return(0);
	}
	if(com->left)
	{
		showcom(com->left);
	}
	printvalue(com->name, com->description, QLIN, margin);

	if(com->right)
	{
		showcom(com->right);
	}
	if(com != com->head)
	{
		com = com->head;
	}
	fflush(stdout);
	return(1);
}
#endif
