Linear Hashing
#Author : Sumeet Gupta

Execution Steps:
1. Copy the 2021AIM1014.cpp file in any folder.
2. In linux terminal, run: $g++ 2021AIM1014.cpp 
3. To execute the program again, all the files created needs to be deleted manually.

Assumptions:(These can be changes by changing the variable in code)
BucketSize = 1
number_of_buckets = 2;
mod_family = 2^i * 2;

Starting Program UI:

1. Create data set 	 2.Enter the records manually one by one 	 3.View Hash table 	 4.Search a record 	 5.View overflow buckets 	 6.Exit

Enter your option: 1
Enter the size of data set: 50

Enter your option: 3
Next Bucket to split 	 H(i, k) 	 H(i+1, k)
...                        ....        ...
0			4			8
1			4			8
2			4			8
2			4			8
3			4			8
...                        ...         ...

Enter your option: 4
Enter the record transaction ID: 6
Record found at: 6.txt
Enter your option: 4
Enter the record transaction ID: 69
Record Not found

Enter your option: 5

... ...
11
12
13
14 -> 46
15 -> 47
16 -> 48
17 -> 49
18 -> 50
...   ...
... ....

Enter your option: 6 // Exit


// The records can be entered manually as well.
Enter your option: 2
To stop enter: E
1
2
3
E 
