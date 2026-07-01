#include "Student.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

static MYSQL* g_conn = nullptr;

MYSQL* getDBConn() {
    if (g_conn) return g_conn;
    g_conn = mysql_init(NULL);
    if (!g_conn) return nullptr;
    if (!mysql_real_connect(g_conn, "127.0.0.1", "root", "123456", "student_sys", 3306, NULL, 0)) {
        std::cerr << "连接MySQL失败: " << mysql_error(g_conn) << std::endl;
        mysql_close(g_conn); g_conn = nullptr; return nullptr;
    }
    if (mysql_set_character_set(g_conn, "utf8mb4") != 0) {
        mysql_close(g_conn); g_conn = nullptr; return nullptr;
    }
    return g_conn;
}

void closeDBConn() {
    if (g_conn) { mysql_close(g_conn); g_conn = nullptr; }
}

void Student::setInfo(const std::string& id, const std::string& n) { sid = id; sname = n; }
void Student::show() { std::cout << "学号：" << sid << " 姓名：" << sname << std::endl; }
std::string Student::getSid() { return sid; }
std::string Student::getSname() { return sname; }
void Student::setSid(const std::string& id) { sid = id; }
void Student::setSname(const std::string& n) { sname = n; }

void AddStudent(Student stu) {
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char esc[256];
    mysql_real_escape_string(conn, esc, stu.getSname().c_str(), (unsigned long)stu.getSname().length());
    char q[512];
    snprintf(q, sizeof(q), "INSERT INTO student(sid, sname) VALUES('%s', '%s')", stu.getSid().c_str(), esc);
    if (mysql_query(conn, q) != 0) std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
}

Student QueryStudentBySid(const std::string& targetSid) {
    Student res;
    MYSQL* conn = getDBConn();
    if (!conn) return res;
    char q[256];
    snprintf(q, sizeof(q), "SELECT sid,sname FROM student WHERE sid='%s'", targetSid.c_str());
    if (mysql_query(conn, q) == 0) {
        MYSQL_RES* result = mysql_store_result(conn);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) res.setInfo(row[0] ? row[0] : "", row[1] ? row[1] : "");
            mysql_free_result(result);
        }
    }
    return res;
}

void UpdateStudentName(const std::string& targetSid, const std::string& newName) {
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char esc[256];
    mysql_real_escape_string(conn, esc, newName.c_str(), (unsigned long)newName.length());
    char q[512];
    snprintf(q, sizeof(q), "UPDATE student SET sname='%s' WHERE sid='%s'", esc, targetSid.c_str());
    if (mysql_query(conn, q) != 0) std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
}

void DeleteStudent(const std::string& targetSid) {
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char q[256];
    snprintf(q, sizeof(q), "DELETE FROM student WHERE sid='%s'", targetSid.c_str());
    if (mysql_query(conn, q) != 0) std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
}