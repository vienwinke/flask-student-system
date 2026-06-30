#include <iostream>
#include <windows.h>
#include "Student.h"
using namespace std;

// 最大存储学生数量
const int MAX_STU = 100;
// 学生数组
Student stuList[MAX_STU];
// 当前已存学生个数
int stuCount = 0;

// 1. 添加学生
void addStu()
{
    if (stuCount >= MAX_STU)
    {
        cout << "学生数量已满，无法添加！" << endl;
        return;
    }
    int id, sc;
    string name;
    cout <<"请输入 学号 姓名 分数：";
    cin >> id >> name >> sc;
    stuList[stuCount].setAll(id, name, sc);
    stuCount++;
    cout << "添加成功！" << endl;
}

// 2. 展示全部学生
void showAll()
{
    if (stuCount == 0)
    {
        cout << "暂无学生数据！" << endl;
        return;
    }
    cout << "\n====所有学生信息====" << endl;
    for (int i = 0; i < stuCount; i++)
    {
        stuList[i].show();
    }
}

// 3. 根据学号查找学生，返回下标，找不到返回-1
int findById(int targetId)
{
    for (int i = 0; i < stuCount; i++)
    {
        if (stuList[i].getID() == targetId)
        {
            return i;
        }
    }
    return -1;
}

// 4. 查询单个学生
void searchStu()
{
    int id;
    cout << "输入要查询的学号：";
    cin >> id;
    int index = findById(id);
    if (index == -1)
    {
        cout << "未找到该学生！" << endl;
    }
    else
    {
        cout << "查询结果：";
        stuList[index].show();
    }
}

// 5. 修改学生分数
void modifyScore()
{
    int id, newSc;
    cout << "输入要修改的学号：";
    cin >> id;
    int index = findById(id);
    if (index == -1)
    {
        cout << "未找到该学生！" << endl;
        return;
    }
    cout << "输入新分数：";
    cin >> newSc;
    stuList[index].setScore(newSc);
    cout << "修改完成！" << endl;
}

// 6. 删除学生
void delStu()
{
    int id;
    cout << "输入要删除的学号：";
    cin >> id;
    int index = findById(id);
    if (index == -1)
    {
        cout << "未找到该学生！" << endl;
        return;
    }
    // 后面所有学生往前覆盖一位
    for (int i = index; i < stuCount - 1; i++)
    {
        stuList[i] = stuList[i + 1];
    }
    stuCount--;
    cout << "删除成功！" << endl;
}

// 打印菜单
void printMenu()
{
    cout << "\n========学生信息管理系统========" << endl;
    cout << "1. 添加学生信息" << endl;
    cout << "2. 查看全部学生" << endl;
    cout << "3. 按学号查询学生" << endl;
    cout << "4. 修改学生分数" << endl;
    cout << "5. 删除学生" << endl;
    cout << "0. 退出系统" << endl;
    cout << "请输入功能序号：";
}

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    int op;
    while (true)
    {
        printMenu();
        cin >> op;
        switch (op)
        {
        case 1: addStu(); break;
        case 2: showAll(); break;
        case 3: searchStu(); break;
        case 4: modifyScore(); break;
        case 5: delStu(); break;
        case 0: cout << "程序退出！" << endl; return 0;
        default: cout << "输入错误，请重新选择！" << endl;
        }
    }
}
