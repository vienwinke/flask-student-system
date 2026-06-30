#ifndef SYUDENT_HPP
#define SYUDENT_HPP
#include <string>
using namespace std;
class Student {
private:
	int id;
	string name;
	int score;
public:
	void setAll(int i, string n, int s);
	void show();
	int getID();
	void setScore(int s);
	int getScore();

};





#endif