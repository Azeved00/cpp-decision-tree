#define _DTREE_

#ifndef _BASE_
#include "base.cpp"
#endif

#ifndef _IOH_
#include "ioHandle.cpp"
#endif

//-------------------------------API--------------------------------------
Node C45();
int chooseBestAttribute(Set validEntries, Set validAttributes);
void binarySplit();
Node buildTree(Set validEntries, Set validAttributes);
int classify(Node root, Entry newData);

//------------------------------HELPERS-----------------------------------
double inline xlnx(double n,double d);
vector<int> checkClasses(Set validEntries);
#include <cmath>
#include <algorithm>

//---------------------------IMPLEMENTATION-------------------------------
double inline xlnx(double n,double d)
{
	if(!n || !d) return 0;
	double temp = (double)n/(double)d;
	return -(temp * log(temp)) * LOG2;
}

int chooseBestAttribute(Set validEntries, Set validAttributes)
{

	double globalEntropy = 0;
	double maxInfoRatio = -1;
	double atrInfoRatio = -1;
	vector<vector<vector<int>>> matrix;
	

	//initializes all of the tables with 0 in every place 
	for(size_t i = 0; i < validAttributes.entities.size(); i++)
	{
		if(!validAttributes.entities[i])
		{
			// making sure every table is at the index of 
			// its attribute by putting empty vectors (do not occupy memmory)
			// in place of the attributes that do not belong to the table
			vector<vector<int>> temp;
			matrix.pb(temp);
			continue;
		}	
		int classIndex = dictionary.size() -1;	
		int lastRow = dictionary[i].size;
		int lastCol = dictionary[classIndex].size;

		vector<int> tempL(lastCol+1,0);
		vector<vector<int>> tempM(lastRow + 1, tempL);
		matrix.pb(tempM);
	}

	//put in matrix everything
	for(size_t i = 0; i < validEntries.entities.size(); i++)
	{
		if(!validEntries.entities[i])
				continue;

		for(size_t j = 0; j < validAttributes.entities.size()-1; j++)
		{	
			if(!validAttributes.entities[j])
				continue;
			
			int classIndex = dictionary.size() -1;
			int lastRow = dictionary[j].size;
			int lastCol = dictionary[classIndex].size;
			
			//+1 occurence of attribute and class 
			matrix[j][classified[i][j]][classified[i][classIndex]]++;
			//+1 occurence of class
			matrix[j][lastRow][classified[i][classIndex]]++;
			//+1 occurence of attribute
			matrix[j][classified[i][j]][lastCol]++;
			//+1 entry
			matrix[j][lastRow][lastCol]++;
		}
	}

	
	//for each attribute make calculations
	//save the maximum
	for(size_t i = 0;i < validAttributes.entities.size(); i++)
	{
		if(!validAttributes.entities[i])
				continue;
		int classIndex = dictionary.size() -1;
		int lastRow = dictionary[i].size;
		int lastCol = dictionary[classIndex].size;
		
		//global entropy ( only calculated once)
		if(globalEntropy == 0)	
			for(int j = 0; j < lastRow; j++)
				globalEntropy += xlnx((double)matrix[i][lastRow][j],(double)matrix[i][lastRow][lastCol]);

		//Individual Entropy for each value of the attributes
		double weightedEntropy = 0;
		for(int j = 0; j < dictionary[i].size; j++)
		{
			double entr=0;
	
			for(int k = 0; k < dictionary[classIndex].size; k++)
				entr += xlnx((double)matrix[i][j][k],(double)matrix[i][j][lastCol]);
			
			if(matrix[i][lastRow][lastCol])
				weightedEntropy += ((double)matrix[i][j][lastCol] / (double)matrix[i][lastRow][lastCol]) * entr; 
		}

		//information gain
		double informationGain = globalEntropy - weightedEntropy;
	

		//information split
		double informationSplit = 0;
		for(int j = 0; j < lastRow; j++)
			informationSplit += xlnx((double)matrix[i][j][lastCol],(double)matrix[i][lastRow][lastCol]);

	
		//calculate attribute information ratio
		
		double informationRatio = informationGain / informationSplit;
		if(!informationSplit)
			informationRatio = 0;
		//take the maximum and save the best
		if(informationRatio > maxInfoRatio)
		{
			maxInfoRatio = informationRatio;
			atrInfoRatio = i;
		}
	}

	//return the maximum information ration of each attribute
	return atrInfoRatio;
}

void binarySplit()
{
	for(size_t i = 0; i < dictionary.size();i++)
	{
		if(!dictionary[i].isNum)
			continue;

		

		double maxGain = -1;
		//sort the array in crescending order
		sort(dictionary[i].dvalues.begin(), dictionary[i].dvalues.end());
		//do presum to know how many classes (of a type) were until that point 
		vector<int> temp(dictionary[dictionary.size()-1].size,0);
		temp.pb(1);
		vector<vector<int>> table((int)dictionary[i].dvalues.size(),temp);
		dictionary[i].threshold = dictionary[i].dvalues[0].first;//safety measure

		const int entries = table.size()-1;
		const int classes = table[0].size()-1;
		const int total = entries+1;
		//aka create the table
		for(size_t j = 0; j < dictionary[i].dvalues.size(); j++)
		{
			for(int k = 0; k < dictionary[dictionary.size()-1].size;k++)
			{

				int change = 0;
				
				//presum everything
				if(j > 0)
				{
					table[j][temp.size()-1] = table[j-1][temp.size()-1]+1;
					change += table[j-1][k];
				}

				table[j][k] = change + (classified[dictionary[i].dvalues[j].second][dictionary.size()-1] == k);
			}			
		}

		//calculate global entropy
		double gEntropy=0;
		for(int j = 0; j < classes; j++)
			gEntropy += xlnx((double)table[entries][j],(double)total);

		//value 
		//ignore the first and the last values
		for(int j = 1; j < entries; j++)
		{
			//if there is another value (equal to this) at the left
			//skip this
			if(dictionary[i].dvalues[j].first == dictionary[i].dvalues[j+1].first)
				continue;


			//Individual Entropy for each value of the attributes
			double wEntropy = 0;
			double entr;
			//less than or equal than jth value
			for(int k = 0; k < classes; k++)
				entr = xlnx((double)table[j][k],(double)table[j][classes]);
		
			if(total)
				wEntropy += entr*((double)table[j][classes]/(double)total);
			
			//greater than jth value
			for(int k = 0; k < classes; k++)
				entr = xlnx((double)(table[entries][k] - table[j][k]),(double)(total - table[j][classes]));
			
			if(total)
				wEntropy += entr*((double)(total - table[j][classes])/(double)total);

			double gain = gEntropy - wEntropy;
			//information split
			if(gain > maxGain)
			{
				maxGain = gain;
				dictionary[i].threshold = dictionary[i].dvalues[j].first;
			}

		}
	

		//change the entier collun to YES if <= threshold and NO if > threshold	
		for(auto j : dictionary[i].dvalues)
			classified[j.second][i] = (j.first > dictionary[i].threshold); 
		
		


		//NOTE: only after doing all this do i realize how stupid i am
		//NOTE: for not using the string class
		//Note: but i dont regret it one bit
		
		//clear the array that is in the attribute
		dictionary[i].dvalues.clear();
		dictionary[i].size = 2;
		String tempS;
		//save yes(1) and no(0) in the attribute map
		memset(tempS.value,0,sizeof tempS.value);
		tempS.value[0] = 'n';
		tempS.value[1] = 'o';
		dictionary[i].values.insert({tempS,0});
		memset(tempS.value,0,sizeof tempS.value);
		tempS.value[0] = 'y';
		tempS.value[1] = 'e';
		tempS.value[2] = 's';
		dictionary[i].values.insert({tempS,1});
	}
	return;
}

vector<int> checkClasses(Set validEntries)
{
	//count different classes in Set
	int nClass = dictionary[dictionary.size()-1].values.size();
	vector<int> counters (nClass,0);
	
	for(size_t i = 0; i < validEntries.entities.size(); i++)
	{
		if(!validEntries.entities[i])
			continue;
		
		//dictionary.size()-1 is where the class is stored
		counters[classified[i][dictionary.size()-1]]++;
	}

	counters.pb(validEntries.size);
	return counters;
}

Node C45()
{
	Node root;
	vector<bool> temp1(classified.size(),true);
	Set S = {temp1,(int)classified.size()};
	vector<bool> temp2(dictionary.size()-1,true);
	temp2.pb(false);
	Set A = {temp2,(int)dictionary.size()-1};
	
	binarySplit();

	printData();

	root = buildTree(S, A);
	return root;
}

Node buildTree(Set validEntries, Set validAttributes)
{
	Node root = {false,-1};

	//class here is writen as clas because class is a keyword of c++
	root.counters = checkClasses(validEntries);
	root.attribute = -1;
	for(size_t i = 0; i < root.counters.size() - 1; i++)
	{
		if(root.counters[i] == root.counters[root.counters.size()-1])
		{
			root.attribute = i;
			break;
		}
	}	
	//one is true when there is only one class present in the dataset
	if(root.attribute != -1)
	{
		root.isLeaf = true;
		return root;
	}
	if(!validEntries.size || !validAttributes.size)
	{
		root.isLeaf = true;
		int max = -1;
		root.attribute = 1;
		//iterate trough counters and take the max;
		for(size_t i = 0; i < root.counters.size()-1; i++)
		{
			if(root.counters[i] > max)
			{
				max = root.counters[i];
				root.attribute = i;
			}
		}
		return root;
	}


	//chose the best attribute
	//the root's attribute becomes that 
	int A = chooseBestAttribute(validEntries, validAttributes);
	root.attribute = A;

	//for each value of the chosen attribute
	validAttributes.entities[A] = false;
	validAttributes.size--;

	Node temp = {false,-1};
	for(size_t k = 0;k < dictionary[A].values.size();k++)
		root.children.pb(temp);
	for(auto value : dictionary[A].values)
	{
		Set newEntries = {{},0};
		for(size_t i = 0; i < validEntries.entities.size(); i++)
		{	
			newEntries.entities.pb(validEntries.entities[i] && (classified[i][A]==value.second));
			newEntries.size += (validEntries.entities[i] && (classified[i][A]==value.second));
		}
		root.children[value.second] = buildTree(newEntries,validAttributes);
	}
	return root;
}

int classify(Node root, Entry newData)
{
	if(root.isLeaf)
		return root.attribute;

	//if a new data appears that was not in the dataset
	//check counters and output the class with highest apperance
	size_t index = newData[root.attribute];
	if(index > root.children.size())
	{
		int max = -1, classe;
		for(size_t i = 0; i < root.counters.size()-1; i++ )
		{
			if(root.counters[i]> max)
			{
				max = root.counters[i];
				classe = i;
			}
		}
		return classe;
	}
	return classify(root.children[index],newData);

}
