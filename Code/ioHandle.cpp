//input handler should be capable of reading any CSV  
#define _IOH_

#ifndef _BASE_
#include "base.cpp"
#endif


//-------------------------------API--------------------------------------
void readClassified(FILE* f);
DataSet readToClassify(FILE* f);
int printTree(DecisionTree n, int tabs);
void printData();



//---------------------------IMPLEMENTATION-------------------------------
void readClassified(FILE* f)
{
	char temp; 

	String value;
	Entry etr;


	//ignore everything ultil the first comma (aka ignore ID)
	while(fscanf(f,"%c",&temp) && temp != ',');

	// read attributes
	while(temp !='\n')
	{
		memset(value.value, 0, sizeof value.value);
			
		//read the value of the attribute
		for(int j = 0; true; j++)
		{
			fscanf(f,"%c",&temp);
			if(temp == ',' || temp == '\n')
				break;
				
			value.value[j] = temp;
			if(temp == '\r')
			{
				value.value[j] = '\0';
				j--;
			}
		}
			Attribute A = {value,0,{},{},true};
			dictionary.pb(A);
	}

	//read entrys 
	while(true)
	{
		
		etr.clear();
		
		//ignore the "ID" attribute 
		//and check if there are no more entries
		fscanf(f,"%c",&temp);
		if(temp == EOF || temp == '\n')
			break;
		while(temp != ',') 
			fscanf(f,"%c",&temp);



		// read attributes
		for(int i = 0; true; i++)
		{
			memset(value.value, 0, sizeof value.value);
			//read the value of the attribute
			for(int j = 0; true; j++)
			{
				fscanf(f,"%c",&temp);
				if(temp == ',' || temp == '\n')
					break;
				value.value[j] = temp;
				dictionary[i].isNum &= ('0'<= temp && temp <= '9') || (temp == '.');
				if(temp == '\r')
				{
					value.value[j] = '\0';
					j--;
				}
			}


			//if value wasnot found in map
			//put inside with a new int
			if(dictionary[i].isNum)
			{
				int n = dictionary[i].dvalues.size();
				dictionary[i].dvalues.pb({atof(value.value),n});
				etr.pb(n);
			}
			else
			{
				auto iter = dictionary[i].values.insert({value,dictionary[i].size});
				if(iter.second) 
					dictionary[i].size++;
			
				etr.pb(iter.first->second);
			}
			if(temp == '\n')
				break; 
		}

		classified.pb(etr);
	}
}

int printTree(DecisionTree n, int tabs = 0)
{
	int missClassified = 0;
	int total = 0;
	if(!tabs)
		printf("---------------------------TREE-------------------------------\n");
	if(n.isLeaf)
	{
		for(auto it: dictionary[dictionary.size()-1].values)
		{
			if(n.attribute == it.second)
				printf("  %s",it.first.value);
		}

		printf(" {");
		for(auto it : dictionary[dictionary.size()-1].values)
		{
			printf("%s: %d, ",it.first.value, n.counters[it.second]);
			if(n.attribute != it.second)
				missClassified += n.counters[it.second];
			
			total += n.counters[it.second];
		}	
		printf("total: %d}",total);
		return missClassified;
	}

	//print the identation
	printf("\n");
	for(int i = 0; i <= tabs; i++)
		printf("    ");
	//print the index of the attribute associated with the node
	printf("<%s",dictionary[n.attribute].name.value);
	if(dictionary[n.attribute].isNum)
		printf(" < %.2lf",dictionary[n.attribute].threshold);
	printf(">");
	//for each {String,Int} pair in the map
	for(auto a : dictionary[n.attribute].values)
	{
		printf("\n");
		//print the identation
		for(int i = 0; i <= tabs+1; i++)
			printf("    ");

		//print the value
		printf("%s:",a.first.value);
		//print the node of that branch
		missClassified += printTree(n.children[a.second], tabs+2);
	}
	printf("\n");


	if(tabs)
		return missClassified;

	printf("--------------------------STATISTICS-------------------------\n");
	printf("Total Entries: %lu\n",classified.size());
	printf("Correctly Classified: %lu\n",classified.size()-missClassified);
	printf("Missclassified: %d\n",missClassified);
	
	return missClassified;
}

DataSet readToClassify(FILE* f)
{

	printf("---------------------------TEST-------------------------------\n");
	char temp; 
	String value;
	Entry etr;
	DataSet toClassify;
	
	
	//read entrys 
	while(true)
	{
		etr.clear();
		
		//check if there are no more entries
		fscanf(f,"%c",&temp);
		if(temp == EOF || temp == '\n')
			break;


		//ignore the "ID" attribute 
		while(temp != ',') 
			fscanf(f,"%c",&temp);



		// read attributes
		for(int i = 0; i <(int) dictionary.size()-1; i++)
		{
			memset(value.value, 0, sizeof value.value);
			
			//read the value of the attribute
			for(int j = 0; true; j++)
			{
				fscanf(f,"%c",&temp);
				if(temp == ',' || temp == '\n')
					break;
				value.value[j] = temp;
			}

			if(dictionary[i].isNum)
				etr.pb(atof(value.value) > dictionary[i].threshold);
			else
			{
				//if value wasnot found in map
				//put inside with a new int
				auto iter = dictionary[i].values.find(value);
				etr.pb(iter->second);
			}
			
		}
		fscanf(f,"%*c");
		//classify the new line
		toClassify.pb(etr);
	}
	return toClassify;
}

void printData()
{
	printf("---------------------------DATA-------------------------------\n");
	for(auto a : dictionary)
		printf("|%*.*s ",CHARS,CHARS,a.name.value);
	printf("|\n");
	
	for(size_t i = 0; i < classified.size(); i++)
	{
		for(size_t j = 0; j < classified[i].size(); j++)
		{
			for(auto it: dictionary[j].values)
			{
				if(classified[i][j] == it.second)
					printf("|%*.*s ",CHARS,CHARS,it.first.value);
			}
		}
		printf("|\n");
	}
	printf("\n");
}
