# Desision tree c4.5 algorithm built in C++

## Details
This program creates a decision tree using the C4.5 algorithm.

### Output:

Data set after binary split (change numerical attributes into nominal ones)
	
The decision tree in the form

	<Attribute1>
		value1: class {counter1 counter2 total}
		value2:
			<Attribute2>
				(...)



## Tested in:
g++ version 11.2.0

linux distro :arch linux x86_64

kernel       : 5.16.15-arch1-1


## compiling: 
`g++ -o driver driver.cpp`

## runing: 
`./driver file1.csv file2.csv`

### Note:
it is advised to run with "./driver file1.csv file2.csv | less" , this will make it so you can start at the top and let you freely move trough the output.
	
Another option is piping the output to a file with "./driver file1.csv file2.csv > tree ", it achives the same result.
