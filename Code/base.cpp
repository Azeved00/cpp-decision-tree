#define _BASE_

#include <iostream>
#include <string.h>
#include <vector>
#include <map>

//the size of values 
#define MAX 32
#define CHARS 7
#define LOG2 1.44269504

//usefull macros
#define pb push_back

using namespace std;

//how the Dictionary type is defined
struct String
{
	char value[MAX];

	bool operator()(const String a,const String b)const
	{
		return (strcmp(a.value,b.value) < 0);
	}
};
struct Attribute 
{
	String name;
	int size;
	vector<pair<double,int>> dvalues;
	map<String, int, String> values;
	bool isNum;
	double threshold;
};
typedef vector<Attribute> Dictionary;

//How the DataSet type is defined
typedef vector<int> Entry;
typedef vector<Entry> DataSet;


//the acutal dictionary and classified data
Dictionary dictionary;
DataSet classified;

//the Node class
struct Node
{
	bool isLeaf;
	int attribute;
	vector<int> counters;
	vector<Node> children;
};

//a decision tree can be saved with only the root (since everithing else is a pointer)
typedef Node DecisionTree;

//set class
struct Set
{
	vector<bool> entities;
	int size;
};





