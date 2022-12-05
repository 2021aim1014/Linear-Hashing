#include<iostream>
#include<cstdio>
#include<vector>
#include<fstream>
#include<cstring>
#include<string>
using namespace std;

#define BucketSize 1

unsigned int number_of_buckets = 2;
unsigned int mod_family = 2;

//A structure to represent each linear hash table entry 
struct node{
	int nextToSplit;
	int H_i;
	int H_i_1;
};

/* Function to create dataset
*	Arguments: 
*		size = count of number of records in dataset
*		fileName = name of the file where records are saved
*	Return:
*/
int createDataSet(int size = 4, string fileName = "DataSet.txt"){
	ofstream fp(fileName);
	if(!fp) return 404;
	string nameArr[] = { "aaa", "aab", "aac", "aad", "bba", "bbb", "bbc", "bbd", "bbe", "cca", "ccb", "ccc", "ccd", "dda", "ddb", "ddc", "ddd", "dde" };
	fp << "Transaction ID \t Transaction sale amount \t Customer name \t category of item \n";
	for(int i=1; i<=size; i++){
		fp << i << "\t\t\t" << rand()%80000 << "\t\t\t" << nameArr[rand()%18] << "\t\t" << rand() % 1500 << "\n";
	}
	fp.close();
	return 0;
}

/* Function to return Transaction ID of record
*	Arguments: 
*		record: Whole record as input
*	Return: Transaction ID of the record
*/
int recordTransactionID(string record){
	string temp;
	for(int i=0; i<record.length() && record[i] != '\t'; i++){
		temp += record[i];
	}

	return stoi(temp);
}

/* Function to insert record into data blocks
*	Arguments: 
*		fileName: Name of file where record needs to be inserted
		data: Record 
		overflowNumber: Number of oveflow in that bucket
*	Return: True or False, whether after inserting a record, there is overflow or not
*/
bool insertIntoFile(string fileName, string data, int overflowNumber = 0){
	
	// To use this function recursively to create overflow file name
	string newFileName = overflowNumber > 0 ? fileName + "_" + to_string(overflowNumber)+".txt"  : fileName+".txt";
	
	ifstream ifp(newFileName);
	
	//file doesn't exist
	if(!ifp){
		ofstream ofp(newFileName);
		ofp << data << "\n";
		ofp.close();
		return false;
	}

	//count number of records
	int noOfRecords = 0;
	string temp;
	while(getline(ifp, temp)){
		noOfRecords++;
	}
	ifp.close();

	
	ofstream ofp;
	ofp.open(newFileName, ios::app);

	//file has empty space
	if(noOfRecords < BucketSize){
		ofp << data << "\n";
		ofp.close();
		return false;
	}

	//file is full
	overflowNumber++;
	if(noOfRecords == BucketSize){
		string of_filename = fileName + "_" + to_string(overflowNumber) + ".txt";
		ofp << of_filename;
		ofp.close();
		insertIntoFile(fileName, data, overflowNumber);
		return true;	
	}

	//file has overflow
	ofp.close();
	insertIntoFile(fileName, data, overflowNumber);
	return true;
}

// Function to delete the file after the records been rehashed
void removeEmptyFile(string bucketName){
	char bucketNameArr[bucketName.length()];
	for(int i=0; i<bucketName.length(); i++){
		bucketNameArr[i] = bucketName[i];
	}
	if(remove(bucketNameArr) != 0){
		ofstream ofp(bucketName);
		ofp << "";
		ofp.close();
	}
}

/* Function to rehash the records contain in data block
*	Arguments: 
*		nextToSplit: Name of file where record needs to be inserted
		modVal: Record 
		overflowNumber: Number of oveflow in that bucket
*	Return: True or False, whether after inserting a record, there is overflow or not
*/
void rehashFunction(int nextToSplit, int modVal, int overflowNumber){
	string fileName = to_string(nextToSplit);
	string newFileName = overflowNumber > 0 ? fileName + "_" + to_string(overflowNumber)+".txt"  : fileName+".txt";
	ifstream ifp(newFileName);
	if(!ifp) return;
	string record;
	int numberOfRecords = 1;
	vector<string> recordArray;
	while(getline(ifp, record) && numberOfRecords <= BucketSize){
		recordArray.push_back(record);
		numberOfRecords++;
	}
	ifp.close();

	removeEmptyFile(newFileName);
	

	for(int i=0; i<numberOfRecords-1; i++){
		unsigned int index = recordTransactionID(recordArray[i]) % modVal;
		// the index should never be greater than number of buckets
		if(index > number_of_buckets){
			cout << "\nERROR\n";
			return;
		}
		string fileName = to_string(index);
		insertIntoFile(fileName, recordArray[i]);

	}
	overflowNumber++;
	//Recursion to rehash the overflowing files as well
	rehashFunction(nextToSplit, modVal, overflowNumber);
}

// Function to insert new entry in hash table whenever there is an overflow after inserting a new record
void insertNewHashTableEntry(vector<struct node*> &list){
	number_of_buckets++;
	struct node *lastNode = list[list.size()-1];
	int nextToSplit = lastNode->nextToSplit;
	rehashFunction(nextToSplit, lastNode->H_i_1, 0);

	nextToSplit++;
	struct node *tempStruct = (struct node*) malloc(sizeof(struct node));
	if(nextToSplit % lastNode->H_i == 0){
		tempStruct->nextToSplit = 0;
		tempStruct->H_i = 2*lastNode->H_i;
		tempStruct->H_i_1 = 2*lastNode->H_i_1;
	} else {
		tempStruct->nextToSplit = nextToSplit;
		tempStruct->H_i = lastNode->H_i;
		tempStruct->H_i_1 = lastNode->H_i_1;
	}
	list.push_back(tempStruct);
}

// Function to insert new entry in hash table whenever there is no overflow after inserting a new record
void insertNewHashTableEntryWhenNoOverflow(vector<struct node*> &list){
	struct node *lastNode = list[list.size()-1];
	struct node *tempStruct = (struct node*) malloc(sizeof(struct node));
	tempStruct->nextToSplit = lastNode->nextToSplit;
	tempStruct->H_i = lastNode->H_i;
	tempStruct->H_i_1 = lastNode->H_i_1;
	list.push_back(tempStruct);
}

// Insert the records to appropriate data blocks(data file)
void insertRecordInfile(vector<struct node*> &list, string record){
	struct node *lastNode = list[list.size()-1];
	int modVal = lastNode->H_i;
	unsigned int index = recordTransactionID(record) % modVal;
	if(index > number_of_buckets){
			cout << "\nERROR\n";
			return;
	}
	//insert into file
	string fileName = to_string(index);
	bool overflow = insertIntoFile(fileName, record);
	if(overflow){
		insertNewHashTableEntry(list);	
	} else {
		insertNewHashTableEntryWhenNoOverflow(list);
	}
}

// Function to hash records from Data set saved in file
void hashFunction(string fileName, vector<struct node*> &list){
	string record;
	ifstream fp(fileName);
	getline(fp, record);
	while(getline(fp, record)){
		insertRecordInfile(list, record);
	}
	fp.close();
}

// // Function to hash records from Data set read from terminal
void hashFunctionForRecordsFromTerminal(string record, vector<struct node*> &list){
	insertRecordInfile(list, record);
}

// // Function to display the hash table
void displayList(vector<struct node*> &list){
	cout << "Next Bucket to split \t H(i, k) \t H(i+1, k) \n";
	for(int i=0; i<list.size(); i++){
		cout << list[i]->nextToSplit << "\t\t\t" << list[i]->H_i << "\t" << list[i]->H_i_1 << "\n";
	}
}


//Recursive function to search for records in overflow buckets as well
string searchForRecordRecursive(string fileName, int tid){
	ifstream ifp(fileName);
	if(!ifp) return "";
	string record;
	int numberOfRecords = 1;
	while(getline(ifp, record) && numberOfRecords <= BucketSize){
		if(tid == recordTransactionID(record)){
			return fileName;
		}
		numberOfRecords++;
	}
	ifp.close();
	getline(ifp, record);
	return searchForRecordRecursive(record, tid);
}


//Function to search for record 
void searchForRecord(vector<struct node*> &list, int tid){
	struct node *lastNode = list[list.size()-1];
	unsigned int index_1 = tid % lastNode->H_i;
	unsigned int index_2 = tid % lastNode->H_i_1;
	string fileName = to_string(index_1)+".txt";
	string search_index = searchForRecordRecursive(fileName, tid);
	if(search_index == ""){
		string fileName = to_string(index_2)+".txt";
		search_index = searchForRecordRecursive(fileName, tid);
	}
	if(search_index == ""){
		cout << "Record Not found";
		return ;
	} else{
		cout << "Record found at: "<<search_index;
		return ;
	}
	
}

// Function to view the buckets and overflow 
void viewOverFlow(){
	for(int i=0; i<=number_of_buckets; i++){
		string fileName = to_string(i) + ".txt";
		ifstream ifp(fileName);
		if(ifp){
			string record;
			getline(ifp, record);
			if(record == "") break;
			ifp.close();
			cout << recordTransactionID(record) ;
			int overflowNumber = 1;
			while(true){
				fileName = to_string(i) + "_" + to_string(overflowNumber) + ".txt";
				ifstream ifp(fileName);
				if(!ifp){
					break;
				}
				getline(ifp, record);
				if(record == "") break;
				ifp.close();
				cout << " -> " << recordTransactionID(record);
				overflowNumber++;
			}
			cout << "\n";
		} else {
			cout << "\n";
		}
	}
}


int main(){
	// A list to contain linear hash table
	vector<struct node*> list;
	struct node *firstNode = (struct node*) malloc(sizeof(struct node));
	firstNode->nextToSplit = 0;
	firstNode->H_i = mod_family;
	firstNode->H_i_1 = 2*mod_family;
	list.push_back(firstNode);

	cout << "\nLinear Hashing Assignment\n\nChoose appropriate option to continue:\n";
	cout << "\n1. Create data set \t 2.Enter the records manually one by one \t 3.View Hash table \t 4.Search a record \t 5.View overflow buckets \t 6.Exit";
	int option = 0;
	do{
		cout << "\nEnter your option: ";
		cin >> option;
		if(option == 1){
			cout << "Enter the size of data set: ";
			unsigned int numberOfRecords;
			cin >> numberOfRecords;
			createDataSet(numberOfRecords, "DataSet.txt");
			hashFunction("DataSet.txt", list);	
		} else if(option == 2){
			string record;
			cout << "To stop enter: E\n";
			while(cin >> record){
				if(record == "E") break;
				hashFunctionForRecordsFromTerminal(record, list);
			}
		} else if(option == 3){
			displayList(list);
		} else if(option == 4){
			int tid;
			cout << "Enter the record transaction ID: ";
			cin >> tid;
			searchForRecord(list, tid);
		} else if (option == 5){
			viewOverFlow();
		} else if(option == 6){
			return 0;
		} else{
			cout << "Please enter a valid option.\n";
		}
	}while(option != 6);

	return 0;
}
