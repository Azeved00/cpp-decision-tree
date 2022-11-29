
#ifndef _BASE_
#include "base.cpp"
#endif

#ifndef _IOH_
#include "ioHandle.cpp"
#endif

#ifndef _DTREE_
#include "decisionTree.cpp"
#endif

int main(int argC,char**argV)
{
	if(argC != 2 && argC != 3)
	{
		printf("Wrong usage: ./driver file1.csv file2.csv\n");
		return -1;
	}


	//read the classified data 
	//make the dictionary
	FILE* dataSet1 = fopen(argV[1],"r");
	readClassified(dataSet1);
	fclose(dataSet1);


	//create the new decision tree
	//with C45 agorithm
	DecisionTree tree = C45();

	printTree(tree);
	//read the other input and classify its data
	if(argC == 3)
	{
		FILE* dataSet2 = fopen(argV[2],"r");
		DataSet toClassify = readToClassify(dataSet2);
		for(auto it: toClassify)
		{
			int classe = classify(tree,it);
			for(auto it2: dictionary[dictionary.size()-1].values)
			{
				if(it2.second == classe)
					printf("%s\n",it2.first.value);
			}

		}
	}
	return 0;
}
