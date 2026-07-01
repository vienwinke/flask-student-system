#include <iostream>
#include <limits>
#include <windows.h>
#include "Student.h"
#include "Score.h"
#include "Calculator.h"

std::string toUTF8(const std::string& src) {
    int wlen = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
    wchar_t* wbuf = new wchar_t[wlen];
    MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, wbuf, wlen);
    int ulen = WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, NULL, 0, NULL, NULL);
    char* ubuf = new char[ulen];
    WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, ubuf, ulen, NULL, NULL);
    std::string result(ubuf);
    delete[] wbuf; delete[] ubuf;
    return result;
}

void pause() {
    std::cout << "\n按回车键继续...";
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

int main()
{
    SetConsoleOutputCP(65001);
    int choice, usual, end, final;
    std::string sid, name, subject;
    Student s;
    Score sc;

    while (true) {
        std::cout << "\n===== 学生成绩管理系统 =====\n"
                  << " 1. 添加学生\n 2. 添加成绩\n 3. 查询学生\n"
                  << " 4. 查询成绩\n 5. 修改姓名\n 6. 修改总评\n"
                  << " 7. 删除学生\n 8. 删除成绩\n 9. 计算总评\n 0. 退出\n请选择: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        bool pauseNeeded = true;
        switch (choice) {
        case 1:
            std::cout << "学号 姓名: "; std::cin >> sid >> name;
            name = toUTF8(name);
            s.setInfo(sid, name); AddStudent(s);
            std::cout << "添加成功!"; break;
        case 2:
            std::cout << "学号 科目 平时 期末 总评: ";
            std::cin >> sid >> subject >> usual >> end >> final;
            subject = toUTF8(subject);
            sc.setAll(sid, subject, usual, end, final); AddScore(sc);
            std::cout << "添加成功!"; break;
        case 3:
            std::cout << "学号: "; std::cin >> sid;
            s = QueryStudentBySid(sid); s.show(); break;
        case 4:
            std::cout << "学号 科目: "; std::cin >> sid >> subject;
            subject = toUTF8(subject);
            sc = QueryScore(sid, subject); sc.showScore(); break;
        case 5:
            std::cout << "学号 新姓名: "; std::cin >> sid >> name;
            name = toUTF8(name);
            UpdateStudentName(sid, name);
            std::cout << "修改成功!"; break;
        case 6:
            std::cout << "学号 科目 新总评: "; std::cin >> sid >> subject >> final;
            subject = toUTF8(subject);
            UpdateFinalScore(sid, subject, final);
            std::cout << "修改成功!"; break;
        case 7:
            std::cout << "学号: "; std::cin >> sid;
            DeleteStudent(sid); std::cout << "已删除!"; break;
        case 8:
            std::cout << "学号 科目: "; std::cin >> sid >> subject;
            subject = toUTF8(subject);
            DeleteScore(sid, subject); std::cout << "已删除!"; break;
        case 9: {
            double weightA;
            std::cout << "学号 科目 权重 a: "; std::cin >> sid >> subject >> weightA;
            subject = toUTF8(subject);
            sc = QueryScore(sid, subject);
            if (sc.getSid().empty()) {
                std::cout << "未找到该成绩!";
            } else {
                int newFinal = CalculateFinalScore(sc.getUsual(), sc.getEnd(), weightA);
                UpdateFinalScore(sid, subject, newFinal);
                std::cout << "总评已更新为: " << newFinal;
            }
            break;
        }
        case 0:
            closeDBConn();
            std::cout << "欢迎下次使用!\n"; return 0;
        default:
            std::cout << "无效选择!\n"; pauseNeeded = false;
        }
        if (pauseNeeded) pause();
    }
}