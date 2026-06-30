#pragma once

#include <string>
#include <mysql.h>

class Student {
private:
    std::string sid;
    std::string sname;
public:
    void setInfo(std::string id, std::string n);
    void show();
    std::string getSid();
    std::string getSname();
    void setSid(std::string id);
    void setSname(std::string n);
};

MYSQL* getDBConn();
void AddStudent(Student stu);
Student QueryStudentBySid(std::string targetSid);
void UpdateStudentName(std::string targetSid, std::string newName);
void DeleteStudent(std::string targetSid);