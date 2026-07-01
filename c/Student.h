#pragma once

#include <string>
#include <mysql.h>

class Student {
private:
    std::string sid;
    std::string sname;
public:
    void setInfo(const std::string& id, const std::string& n);
    void show();
    std::string getSid();
    std::string getSname();
    void setSid(const std::string& id);
    void setSname(const std::string& n);
};

MYSQL* getDBConn();
void AddStudent(Student stu);
Student QueryStudentBySid(const std::string& targetSid);
void UpdateStudentName(const std::string& targetSid, const std::string& newName);
void DeleteStudent(const std::string& targetSid);