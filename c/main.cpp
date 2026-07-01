#include <iostream>
#include <limits>
#include <windows.h>
#include "Student.h"
#include "Score.h"
#include "Calculator.h"

std::string toUTF8(const std::string& src) {
    if (src.empty()) return src;
    int wlen = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
    if (wlen <= 0) return src;
    std::wstring wbuf(wlen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, &wbuf[0], wlen);
    int ulen = WideCharToMultiByte(CP_UTF8, 0, wbuf.c_str(), -1, NULL, 0, NULL, NULL);
    if (ulen <= 0) return src;
    std::string ubuf(ulen, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wbuf.c_str(), -1, &ubuf[0], ulen, NULL, NULL);
    ubuf.resize(ulen - 1);
    return ubuf;
}

double getWeight(const std::string& sub) {
    MYSQL* conn = getDBConn();
    if (!conn) return -1.0;
    char esc[256];
    mysql_real_escape_string(conn, esc, sub.c_str(), (unsigned long)sub.length());
    char q[256];
    snprintf(q, sizeof(q), "SELECT weight_ratio FROM subject_weight WHERE subject_name='%s'", esc);
    if (mysql_query(conn, q) != 0) return -1.0;
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res || mysql_num_rows(res) == 0) { if(res) mysql_free_result(res); return -1.0; }
    double w = 0.5;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row && row[0]) w = atof(row[0]);
    mysql_free_result(res);
    return w;
}

bool ensureWeight(const std::string& sub, double& w) {
    w = getWeight(sub);
    if (w >= 0) return true;
    std::cout << "未设权重，是否设置？(y/n): ";
    std::string ans; std::cin >> ans;
    if (ans != "y" && ans != "Y") return false;
    std::cout << "权重 a: "; std::cin >> w;
    MYSQL* conn = getDBConn();
    if (!conn) return false;
    char esc[256];
    mysql_real_escape_string(conn, esc, sub.c_str(), (unsigned long)sub.length());
    char q[256];
    snprintf(q, sizeof(q), "INSERT INTO subject_weight(subject_name, weight_ratio) VALUES('%s', %.2f)", esc, w);
    return mysql_query(conn, q) == 0;
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
                  << " 4. 查询成绩\n 5. 修改姓名\n 6. 修改平时\n"
                  << " 7. 删除学生\n 8. 删除成绩\n 9. 学科权重\n 0. 退出\n请选择: ";
        std::cin >> choice;
        if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }

        bool pauseNeeded = true;
        switch (choice) {
        case 1:
            std::cout << "学号 姓名: "; std::cin >> sid >> name;
            name = toUTF8(name);
            s.setInfo(sid, name); AddStudent(s);
            std::cout << "添加成功!"; break;
        case 2: {
            std::cout << "学号 科目 平时 期末: ";
            std::cin >> sid >> subject >> usual >> end;
            subject = toUTF8(subject);
            double w;
            if (!ensureWeight(subject, w)) break;
            int calcFinal = CalculateFinalScore(usual, end, w);
            sc.setAll(sid, subject, usual, end, calcFinal); AddScore(sc);
            std::cout << "添加成功! 成绩=" << calcFinal
                      << " (平时x" << w << " + 期末x" << (1-w) << ")";
            break;
        }
        case 3:
            std::cout << "学号: "; std::cin >> sid;
            s = QueryStudentBySid(sid); s.show(); break;
        case 4: {
            std::cout << "学号 科目(0=全部): "; std::cin >> sid >> subject;
            subject = toUTF8(subject);
            if (subject == "0") {
                MYSQL* conn = getDBConn();
                if (!conn) break;
                char es[256];
                mysql_real_escape_string(conn, es, sid.c_str(), (unsigned long)sid.length());
                char q[256];
                snprintf(q, sizeof(q), "SELECT subject,usual_score,end_score,final_score FROM score WHERE sid='%s'", es);
                if (mysql_query(conn, q) == 0) {
                    MYSQL_RES* res = mysql_store_result(conn);
                    if (res) {
                        int n = (int)mysql_num_rows(res);
                        if (n == 0) std::cout << "该学号暂无成绩";
                        else {
                            std::cout << "共 " << n << " 条成绩:\n";
                            MYSQL_ROW row;
                            while ((row = mysql_fetch_row(res)))
                                std::cout << "  科目:" << (row[0]?row[0]:"") << " 平时:" << (row[1]?row[1]:"0")
                                          << " 期末:" << (row[2]?row[2]:"0") << " 成绩:" << (row[3]?row[3]:"0") << "\n";
                        }
                        mysql_free_result(res);
                    }
                }
            } else {
                sc = QueryScore(sid, subject);
                if (sc.getSid().empty()) std::cout << "未找到该成绩!";
                else sc.showScore();
            }
            break;
        }
        case 5:
            std::cout << "学号 新姓名: "; std::cin >> sid >> name;
            name = toUTF8(name);
            UpdateStudentName(sid, name);
            std::cout << "修改成功!"; break;
        case 6: {
            std::cout << "学号 科目 新平时: "; std::cin >> sid >> subject >> usual;
            subject = toUTF8(subject);
            sc = QueryScore(sid, subject);
            if (sc.getSid().empty()) { std::cout << "未找到该成绩!"; break; }
            double w;
            if (!ensureWeight(subject, w)) break;
            int newFinal = CalculateFinalScore(usual, sc.getEnd(), w);
            MYSQL* conn = getDBConn();
            if (!conn) break;
            char es[256], es2[256];
            mysql_real_escape_string(conn, es, sid.c_str(), (unsigned long)sid.length());
            mysql_real_escape_string(conn, es2, subject.c_str(), (unsigned long)subject.length());
            char q[512];
            snprintf(q, sizeof(q), "UPDATE score SET usual_score=%d, final_score=%d WHERE sid='%s' AND subject='%s'",
                usual, newFinal, es, es2);
            if (mysql_query(conn, q) != 0) std::cerr << "SQL error: " << mysql_error(conn);
            else std::cout << "修改成功! 平时=" << usual << " 成绩=" << newFinal;
            break;
        }
        case 7:
            std::cout << "学号: "; std::cin >> sid;
            DeleteStudent(sid); std::cout << "已删除!"; break;
        case 8:
            std::cout << "学号 科目: "; std::cin >> sid >> subject;
            subject = toUTF8(subject);
            DeleteScore(sid, subject); std::cout << "已删除!"; break;
        case 9: {
            double w;
            std::cout << "科目 权重 a: "; std::cin >> subject >> w;
            subject = toUTF8(subject);
            MYSQL* conn = getDBConn();
            if (!conn) break;
            char esc[256];
            mysql_real_escape_string(conn, esc, subject.c_str(), (unsigned long)subject.length());
            char q[512];
            snprintf(q, sizeof(q), "REPLACE INTO subject_weight(subject_name, weight_ratio) VALUES('%s', %.2f)", esc, w);
            if (mysql_query(conn, q) != 0) { std::cerr << "SQL error: " << mysql_error(conn); break; }
            snprintf(q, sizeof(q), "UPDATE score SET final_score = ROUND(%.2f * usual_score + %.2f * end_score) WHERE subject='%s'",
                w, 1.0 - w, esc);
            mysql_query(conn, q);
            std::cout << "权重已保存, 已更新 " << (unsigned long long)mysql_affected_rows(conn) << " 条成绩";
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