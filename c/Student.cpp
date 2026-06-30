#include "Student.h"
#include <iostream>
#include <string>
using namespace std;

void Student::setAll(int i, string n, int s) {
	id = i;
	name = n;
	score = s;
}
void Student::show() {
	cout << "学号：" << id << " 姓名：" << name << " 分数：" << score << endl;
}
int Student::getID() {
	return id;
}
void Student::setScore(int s)
{
    if (s >= 0 && s <= 100)
    {
        score = s;
    }
    else
    {
        cout << "分数不合法，修改失败！" << endl;
    }
}

// 获取分数
int Student::getScore()
{
    return score;
}
