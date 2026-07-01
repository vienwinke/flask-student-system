#include <iostream>
#include <limits>
#include <windows.h>
#include "Student.h"
#include "Score.h"

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
    std::cout << "\n\u6309\u56de\u8f66\u952e\u7ee7\u7eed...";
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
        std::cout << "\n===== \u5b66\u751f\u6210\u7ee9\u7ba1\u7406\u7cfb\u7edf =====\n"
                  << " 1. \u6dfb\u52a0\u5b66\u751f\n 2. \u6dfb\u52a0\u6210\u7ee9\n 3. \u67e5\u8be2\u5b66\u751f\n"
                  << " 4. \u67e5\u8be2\u6210\u7ee9\n 5. \u4fee\u6539\u59d3\u540d\n 6. \u4fee\u6539\u603b\u8bc4\n"
                  << " 7. \u5220\u9664\u5b66\u751f\n 8. \u5220\u9664\u6210\u7ee9\n 0. \u9000\u51fa\n\u8bf7\u9009\u62e9: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        bool pauseNeeded = true;
        switch (choice) {
        case 1:
            std::cout << "\u5b66\u53f7 \u59d3\u540d: "; std::cin >> sid >> name;
            name = toUTF8(name);
            s.setInfo(sid, name); AddStudent(s);
            std::cout << "\u6dfb\u52a0\u6210\u529f!"; break;
        case 2:
            std::cout << "\u5b66\u53f7 \u79d1\u76ee \u5e73\u65f6 \u671f\u672b \u603b\u8bc4: ";
            std::cin >> sid >> subject >> usual >> end >> final;
            subject = toUTF8(subject);
            sc.setAll(sid, subject, usual, end, final); AddScore(sc);
            std::cout << "\u6dfb\u52a0\u6210\u529f!"; break;
        case 3:
            std::cout << "\u5b66\u53f7: "; std::cin >> sid;
            s = QueryStudentBySid(sid); s.show(); break;
        case 4:
            std::cout << "\u5b66\u53f7 \u79d1\u76ee: "; std::cin >> sid >> subject;
            subject = toUTF8(subject);
            sc = QueryScore(sid, subject); sc.showScore(); break;
        case 5:
            std::cout << "\u5b66\u53f7 \u65b0\u59d3\u540d: "; std::cin >> sid >> name;
            name = toUTF8(name);
            UpdateStudentName(sid, name);
            std::cout << "\u4fee\u6539\u6210\u529f!"; break;
        case 6:
            std::cout << "\u5b66\u53f7 \u79d1\u76ee \u65b0\u603b\u8bc4: "; std::cin >> sid >> subject >> final;
            subject = toUTF8(subject);
            UpdateFinalScore(sid, subject, final);
            std::cout << "\u4fee\u6539\u6210\u529f!"; break;
        case 7:
            std::cout << "\u5b66\u53f7: "; std::cin >> sid;
            DeleteStudent(sid); std::cout << "\u5df2\u5220\u9664!"; break;
        case 8:
            std::cout << "\u5b66\u53f7 \u79d1\u76ee: "; std::cin >> sid >> subject;
            subject = toUTF8(subject);
            DeleteScore(sid, subject); std::cout << "\u5df2\u5220\u9664!"; break;
        case 0:
            std::cout << "\u6b22\u8fce\u4e0b\u6b21\u4f7f\u7528!\n"; return 0;
        default:
            std::cout << "\u65e0\u6548\u9009\u62e9!\n"; pauseNeeded = false;
        }
        if (pauseNeeded) pause();
    }
}