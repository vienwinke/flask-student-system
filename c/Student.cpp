#include "Student.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

MYSQL* getDBConn()
{
    MYSQL* conn = mysql_init(NULL);
    if (!conn) return nullptr;
    if (!mysql_real_connect(conn, "127.0.0.1", "root", "123456", "student_sys", 3306, NULL, 0)) {
        std::cerr << "\u8fde\u63a5MySQL\u5931\u8d25: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return nullptr;
    }
    mysql_set_character_set(conn, "utf8mb4");
    return conn;
}

void Student::setInfo(std::string id, std::string n) { sid = id; sname = n; }
void Student::show() { std::cout << "\u5b66\u53f7\uff1a" << sid << " \u59d3\u540d\uff1a" << sname << std::endl; }
std::string Student::getSid() { return sid; }
std::string Student::getSname() { return sname; }
void Student::setSid(std::string id) { sid = id; }
void Student::setSname(std::string n) { sname = n; }

void AddStudent(Student stu)
{
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char esc[256];
    mysql_real_escape_string(conn, esc, stu.getSname().c_str(), (unsigned long)stu.getSname().length());
    char q[512];
    snprintf(q, sizeof(q), "INSERT INTO student(sid, sname) VALUES('%s', '%s')", stu.getSid().c_str(), esc);
    if (mysql_query(conn, q) != 0) { std::cerr << "SQL error: " << mysql_error(conn) << std::endl; }
    mysql_close(conn);
}

Student QueryStudentBySid(std::string targetSid)
{
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
    mysql_close(conn);
    return res;
}

void UpdateStudentName(std::string targetSid, std::string newName)
{
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char esc[256];
    mysql_real_escape_string(conn, esc, newName.c_str(), (unsigned long)newName.length());
    char q[512];
    snprintf(q, sizeof(q), "UPDATE student SET sname='%s' WHERE sid='%s'", esc, targetSid.c_str());
    if (mysql_query(conn, q) != 0) { std::cerr << "SQL error: " << mysql_error(conn) << std::endl; }
    mysql_close(conn);
}

void DeleteStudent(std::string targetSid)
{
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char q[256];
    snprintf(q, sizeof(q), "DELETE FROM student WHERE sid='%s'", targetSid.c_str());
    if (mysql_query(conn, q) != 0) { std::cerr << "SQL error: " << mysql_error(conn) << std::endl; }
    mysql_close(conn);
}