// PathFinding.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <fstream>

#define GRID_WIDTH 5
#define GRID_HEIGHT 5
#define WALL 49
using namespace std;

struct Node
{
   
   int rid,cid,f,g,h;
   bool isWalkable;
   vector<Node*> connectedList;
   Node* parent; 
   char label;
   
   void setVar(int row,int col,bool walkable)
    {
      isWalkable = walkable;
      rid = row;
      cid = col;
      parent = NULL;
    }  
};

Node mNode[GRID_HEIGHT][GRID_WIDTH], *startNode = NULL, *endNode = NULL;

vector<Node*> openList, closeList, test;

char sampleNodeList[GRID_HEIGHT][GRID_WIDTH] = {	{'0' , '0' , '0' , '0' , '0'},
													{'0' , '1' , '1' , 'A' , '0'},
													{'0' , '0' , '1' , '1' , '1'},
													{'0' , '0' , '0' , '0' , '0'},
													{'B' , '0' , '0' , '0' , '0'}};


void CreateMazeFromFile(char* FileName)
{
	fstream file;
	file.open(FileName);
	char c;
	int count = 0;
	if(file.is_open())
	{
		while(count<5)
			  {
				   file.getline(sampleNodeList[count],GRID_WIDTH+1);
				   count++;
			  }
	}
			
}




void UpdateScore(Node* start, Node* end)
    {
		//Manhattan Method
			if ((start->parent->rid - start->rid) != 0 && (start->parent->cid - start->cid) !=0)
			{
				//diagnol
				 start->g = start->parent->g + 1.414;
            }
            else
                  start->g = start->parent->g + 1;

			start->h = abs(start->rid - end->rid) + abs(start->cid - end->cid);
          	start->f = start->g + start->h;
    
	}
//for checking connections

void FillConnectedNodes(Node* node)
{	//clock_t begin;
	
	if(!node->isWalkable)
		return;

	//begin = clock();
	if(node->cid >= 1 && mNode[node->rid][node->cid-1].isWalkable) //left
		node->connectedList.push_back(&mNode[node->rid][node->cid-1]);

	if(node->rid >= 1 && mNode[node->rid-1][node->cid].isWalkable) //up
		node->connectedList.push_back(&mNode[node->rid-1][node->cid]);
	
	if(node->cid < GRID_WIDTH - 1 && mNode[node->rid][node->cid + 1].isWalkable) //right
		node->connectedList.push_back(&mNode[node->rid][node->cid + 1]);
	
	if(node->rid < GRID_HEIGHT - 1 && mNode[node->rid+1][node->cid].isWalkable)//down
		node->connectedList.push_back(&mNode[node->rid+1][node->cid]);
	
	if(node->cid >= 1 && node->rid >= 1 && mNode[node->rid-1][node->cid-1].isWalkable)//left up diagnol
		node->connectedList.push_back(&mNode[node->rid-1][node->cid-1]);
	
	if(node->cid >= 1 && node->rid < GRID_HEIGHT -1 && mNode[node->rid + 1][node->cid - 1].isWalkable)//left down diagnol
		node->connectedList.push_back(&mNode[node->rid + 1][node->cid - 1]);

	if(node->cid <GRID_WIDTH -1 && node->rid >=1 && mNode[node->rid -1][node->cid + 1].isWalkable)//right up daignol
		node->connectedList.push_back(&mNode[node->rid -1][node->cid + 1]);

	if(node->cid <=3 && node->rid <=3 && mNode[node->rid + 1][node->cid + 1].isWalkable)//right down daignol
		node->connectedList.push_back(&mNode[node->rid + 1][node->cid + 1]);
	//cout<<" Time : "<<(float)(clock() - begin)/CLOCKS_PER_SEC<<"\n";

}

void InitNodes()
{
	
	for (int i = 0; i < GRID_HEIGHT; ++i)
     {
       for (int j = 0; j < GRID_WIDTH; ++j)
       {
		
          mNode[i][j].label = sampleNodeList[i][j];
		  int label = (int)mNode[i][j].label;
		  
		  mNode[i][j].setVar(i,j,label!=WALL);
		  if (label == 65)
				 startNode = &mNode[i][j];
          else if (label == 66)
          		  endNode = &mNode[i][j];
               
       }
     }

	if(endNode == NULL || startNode == NULL)
	{
		cout<<"No Path Found\n";
		exit(0);
	}
	
	
	for (int i = 0; i < GRID_HEIGHT; ++i)
     {
       for (int j = 0; j < GRID_WIDTH; ++j)
	   {
		   //cout<<"i : "<<i << " j : "<<j<< " ";
	   	   FillConnectedNodes(&mNode[i][j]);
	   }
	 }
}

bool CompareScore(Node* first, Node* second)
{
	return (first->f > second->f);
}


bool FindPath(Node* start , Node* end)
{

	if(!start->isWalkable)
	{
		return false;
	}

	bool found=false;
	int f;
	Node* nodeP, *DummyNodeQ;
	start->f=0;
	start->g=0;
	start->h=0;
	openList.push_back(start);
	while(1)
	{
		if(openList.empty())
			break;
		
		Node* bestNode = openList.back();
		openList.pop_back();
		//is in closed list
		closeList.push_back(bestNode);
		//cout<<bestNode->f<<"\n";
		if(bestNode == end)
		{
				found = true;
				break;
		}
		vector<Node*>::iterator itr;
		for(int i=0;i<bestNode->connectedList.size();++i)
		{
			//if the connected Node is in Closed List
			if(find(closeList.begin(), closeList.end(), bestNode->connectedList[i]) != closeList.end())
				continue;
			//if the connected Node is in open List
			itr = find(openList.begin(), openList.end(), bestNode->connectedList[i]);
			if(itr != openList.end())
			{
				Node scoreTestingNode = *openList[distance(openList.begin(),itr)];
				
				openList[distance(openList.begin(),itr)]->parent = bestNode;
				
				UpdateScore(openList[distance(openList.begin(),itr)],end);
				
				if(openList[distance(openList.begin(),itr)]->f > scoreTestingNode.f)
				{
					*openList[distance(openList.begin(),itr)] = scoreTestingNode;
				}
								
			}
			else
			{
				bestNode->connectedList[i]->parent = bestNode;
				UpdateScore(bestNode->connectedList[i],end);
				openList.push_back(bestNode->connectedList[i]);
				
			}
		}
		
		sort(openList.begin(),openList.end(),CompareScore);
	}
	return found;
}

void TracePath()
{
	Node* Trace = endNode;
	Trace = Trace->parent;
	int number_of_steps = 1;
	while(Trace != startNode)
	{
		number_of_steps++;
		sampleNodeList[Trace->rid][Trace->cid] = 'P';
		Trace = Trace->parent;
	}
	for(int i=0;i<GRID_HEIGHT;++i)
	{
		for(int j=0;j<GRID_WIDTH;++j)
			cout<<sampleNodeList[i][j]<<" ";
		cout<<"\n";
	}
	cout<<"\nShortest Path (P) No of Steps = "<<number_of_steps<<"\n";
}

int main(int argc, char *argv[])
{
	int menuOption;
	cout<<"================= Find Shortest Path ===============\n";
	cout<<"1. Maze in MAZE.txt\n";
	cout<<"2. Maze in default Array\n";
	cout<<"====================================================\n";
	cin>>menuOption;
	system("cls");
	if(menuOption == 1)
			CreateMazeFromFile("MAZE.txt");
	//if wrong option default opton is 2
	InitNodes();
	if(FindPath(startNode,endNode))
		TracePath();
	return 0;
}


