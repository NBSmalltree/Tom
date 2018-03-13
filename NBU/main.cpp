#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace std;

#define ll __int64;
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

const double pi = 2 * acos(0.0);
const int inf = 0x3f3f3f3f;

class Employee {
public:
	Employee(char* newName, char* newSex, char* newSSnumber, int newBasesalary, int newPrize);
	Employee(char* newName, char* newSex, char* newSSnumber);

	char* getName() { return name; }
	char* getSex() { return sex; }
	char* getSSNumber() { return ssnumber; }
	int getBaseSalary() { return basesalary; }
	int getPrize() { return prize; }
	int getSalary() { return basesalary + prize; }
private:
	char name[100];
	char sex[10];
	char ssnumber[20];
	int basesalary;
	int prize;
	int salary;
};

Employee::Employee(char* newName, char* newSex, char* newSSnumber, int newBasesalary, int newPrize) {
	strcpy(name, newName);
	strcpy(sex, newSex);
	strcpy(ssnumber, newSSnumber);
	basesalary = newBasesalary;
	prize = newPrize;
};

Employee::Employee(char* newName, char* newSex, char* newSSnumber) {
	strcpy(name, newName);
	strcpy(sex, newSex);
	strcpy(ssnumber, newSSnumber);
};

class HourlyWorker 
	: public 
Employee{
public:
	HourlyWorker(char* newName, char* newSex, char* newSSnumber, int newHourSalary, int newHours) : Employee(newName, newSex, newSSnumber) {
		hoursalary = newHourSalary;
		hours = newHours;
	};

	//char* getName() { return name; }	
	//char* getSex() { return sex; }
	//char* getSSNumber() { return ssnumber; }
	int getHourSalary() { return hoursalary; }
	int getHours() { return hours; }
	int getSalary() { return hours * hoursalary; }
private:
	int hoursalary;
	int hours;
};
int main() {
	Employee ea("张三", "男", "333-33-3333", 5000, 3000);
	cout << "\n姓名:" << ea.getName()			//	张三
		<< "\n性别: " << ea.getSex()			//	男
		<< "\n社保号: " << ea.getSSNumber()		//	333-33-3333
		<< "\n基本工资:" << ea.getBaseSalary()		//	5000
		<< "\n奖金: " << ea.getPrize()			//	3000
		<< "\n应发: " << ea.getSalary() << endl;		//	8000

	HourlyWorker eb("李斯", "女", "333-33-4444", 100, 70);
	cout << "\n姓名:" << eb.getName()			//	李斯
		<< "\n性别: " << eb.getSex()			//	女
		<< "\n社保号: " << eb.getSSNumber()		//	333-33-4444
		<< "\n小时工资:" << eb.getHourSalary()		//	100
		<< "\n工作时间:" << eb.getHours()		//	70
		<< "\n应发: " << eb.getSalary() << endl;		//	7000
	return 0;

	
	//getchar();
	return 1;
}