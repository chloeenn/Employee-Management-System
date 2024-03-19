#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <occi.h>
#include <string.h>
#include <iomanip>
using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;
//header file
struct Employee {
    int employeeNumber;
    char lastName[50];
    char firstName[50];
    char email[100];
    char phone[50];
    char extension[10];
    char reportsTo[100];
    char jobTitle[50];
    char city[50];
    char officeCode[10];
};
//DECLARE FUNCTION PROTOTYPE
int menu();
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp); //1
void displayEmployee(Connection* conn, struct Employee emp); //1
void displayAllEmployees(Connection* conn); //2
void insertEmployee(Connection* conn, struct Employee emp); //3
void updateEmployee(Connection* conn, int employeeNumber); //4
void deleteEmployee(Connection* conn, int employeeNumber); //5
//MAIN
int main()
{
    Employee E;
    Environment* env = nullptr;
    Connection* conn = nullptr;
    std::string str;
    std::string usr = "";
    std::string pass = "";
    std::string srv = "";
    try
    {
        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection(usr, pass, srv);
        std::cout << "Connection is Successful!" << std::endl;
        int selection = 0;
        int empnum = 0;
        int result = 0;
        do {
            selection = menu();
            if (selection == 1) { //Find Emp
                cout << "Enter Employee Number: ";
                cin >> empnum;
                result = findEmployee(conn, empnum, &E);
                if (result == 1) {
                    displayEmployee(conn, E);
                }
                else {
                    std::cout << "Employee " << empnum << " does not exist." << std::endl;
                }
            }
            else if (selection == 2) { //Display All Emp
                displayAllEmployees(conn);
            }
            else if (selection == 3) { //Add Emp
                insertEmployee(conn, E);
            }
            else if (selection == 4) { //Update Emp
                bool isValid = false;
                int tempEmpNumber = 0;
                std::cout << "Employee Number: ";
                do {
                    std::cin >> tempEmpNumber;
                    if (std::cin.fail()) {
                        std::cout << "Invalid Integer! Employee Number: ";
                    }
                    else isValid = true;
                } while (isValid == false);
                updateEmployee(conn, tempEmpNumber);
            }
            else if (selection == 5) { //Remove Emp
                bool isValid = false;
                int tempEmpNumber = 0;
                std::cout << "Employee Number: ";
                do {
                    std::cin >> tempEmpNumber;
                    if (std::cin.fail()) {
                        std::cout << "Invalid Integer! Employee Number: ";
                    }
                    else isValid = true;
                } while (isValid == false);
                deleteEmployee(conn, tempEmpNumber);
            }
        } while (selection != 0);
    }
    catch (SQLException& sqlExcp)
    {
        std::cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
    env->terminateConnection(conn);
    Environment::terminateEnvironment(env);
}
//FUNCTION DEFINITION
int menu(void) {
    int choice; bool flag = false;
    cout << "********************* HR Menu *********************" << endl;
    cout << "1) Find Employee" << endl;
    cout << "2) Employees Report" << endl;
    cout << "3) Add Employee" << endl;
    cout << "4) Update Employee" << endl;
    cout << "5) Remove Employee" << endl;
    cout << "0) Exit" << endl;
    cout << "Select an option (0-5): ";
    do {
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input, retry! Select an option (0-5): ";
        }
        else if (choice < 0 || choice > 5) {
            cout << "Out of Range, retry! Select an option (0-5): ";
        }
        else {
            flag = true;
        }
    } while (!flag);
    return choice;
}
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp) {
    int empNumber = 0;
    int returnindex = 0;
    string lastname, firstname, email, phone, extension, reportto, jobtitle, city;
    Statement* S = conn->createStatement("SELECT e.employeenumber, e.lastname, e.firstname, e.email, o.phone, e.extension, e.reportsTo, e2.firstname || ' ' || e2.lastname managerName, e.jobtitle, o.city FROM employees e JOIN offices o ON e.officecode = o.officecode LEFT JOIN employees e2 ON e.reportsto = e2.employeenumber WHERE e.employeenumber=:1");
    S->setInt(1, employeeNumber); //set the value for the 1st parameter as int
    ResultSet* result = S->executeQuery();
    if (result->next()) { //next() is used to fetch the data
        emp->employeeNumber = result->getInt(1);
        strcpy(emp->lastName, result->getString(2).c_str()); //string to cstring
        strcpy(emp->firstName, result->getString(3).c_str());
        strcpy(emp->email, result->getString(4).c_str());
        strcpy(emp->phone, result->getString(5).c_str());
        strcpy(emp->extension, result->getString(6).c_str());
        strcpy(emp->reportsTo, result->getString(7).c_str());
        strcpy(emp->jobTitle, result->getString(8).c_str());
        strcpy(emp->city, result->getString(9).c_str());
        returnindex = 1;
    }
    return returnindex; //exist: return 1 - not exist: return 0
}
void displayEmployee(Connection* conn, struct Employee emp) {
    std::cout << "employeeNumber = " << emp.employeeNumber << endl;
    std::cout << "lastName = " << emp.lastName << endl;
    std::cout << "firstName = " << emp.firstName << endl;
    std::cout << "email = " << emp.email << endl;
    std::cout << "phone = +" << emp.phone << endl;
    std::cout << "extension = " << emp.extension << endl;
    std::cout << "reportsTo= " << emp.reportsTo << endl;
    std::cout << "jobTitle = " << emp.jobTitle << endl;
    std::cout << "city = " << emp.city << endl;
}
void displayAllEmployees(Connection* conn) {
    Statement* S = conn->createStatement();
    ResultSet* res = S->executeQuery("SELECT e.employeenumber, e.firstname || ' ' || e.lastname employeeName, e.email, o.phone, e.extension, e2.firstname || ' ' || e2.lastname managerName FROM employees e JOIN offices o ON e.officecode = o.officecode LEFT JOIN employees e2 ON e.reportsto = e2.employeenumber ORDER BY e.employeenumber");
    if (!res->next()) {
        std::cout << "There is no employees’ information to be displayed.\n";
    }
    else {
        std::cout << "E    Employee Name     Email                 Phone         Ext    Manager\n";
        std::cout << "------------------------------------------------------------------------------------------------------------------------\n";
        while (res->next()) {
            cout.setf(ios::left);
            cout.width(15);
            cout << res->getInt(1);
            cout.width(20);
            cout << res->getString(2);
            cout.width(20);
            cout << res->getString(3);
            cout.width(20);
            cout << res->getString(4);
            cout.width(10);
            cout << res->getString(5) << endl;
            cout.unsetf(ios::left);
        };
        std::cout << std::endl;
    }
}
//enter the new employee information and store them into a variable of type Employee structure
void insertEmployee(Connection* conn, struct Employee emp) {
    bool isValid = false;
    std::cout << "Employee Number: ";
    do {
        std::cin >> emp.employeeNumber;
        if (std::cin.fail()) {
            std::cout << "Invalid Integer! Employee Number: ";
        }
        else isValid = true;
        std::cin.ignore(); //ignore the next character
    } while (isValid == false);
    if (findEmployee(conn, emp.employeeNumber, &emp) == 1) {
        std::cout << "An employee with the same employee number exists." << std::endl;
    }
    else { //if employee number is not existed
        //get info
        std::cout << "Last Name: ";
        std::cin.getline(emp.lastName, 128);
        std::cout << "First Name: ";
        std::cin.getline(emp.firstName, 128);
        std::cout << "Email: ";
        std::cin.getline(emp.email, 128);
        std::cout << "extension: ";
        std::cin.getline(emp.extension, 128);
        std::cout << "Job Title: ";
        std::cin.getline(emp.jobTitle, 128);
        std::cout << "City: ";
        std::cin.getline(emp.city, 128);
        strcpy(emp.officeCode, "1");
        strcpy(emp.reportsTo, "1002");
        Statement* stm = conn->createStatement();
        stm->setSQL("INSERT INTO employees VALUES(:1,:2,:3,:4,:5,:6,:7,:8)");
        stm->setInt(1, emp.employeeNumber); //get the first column as int
        stm->setString(2, emp.lastName); //get the second column as string
        stm->setString(3, emp.firstName);
        stm->setString(4, emp.extension);
        stm->setString(5, emp.email);
        stm->setString(6, emp.officeCode);
        stm->setString(7, emp.reportsTo);
        stm->setString(8, emp.jobTitle);
        stm->executeUpdate(); //execute the new queries
        conn->commit(); //commit the current statement
        std::cout << "The new employee is added successfully. " << std::endl;
        conn->terminateStatement(stm); //close and free statement object
    }
}
//updates the phone extension for the given employee.
void updateEmployee(Connection* conn, int employeeNumber) {
    struct Employee emp;
    if (findEmployee(conn, employeeNumber, &emp) == 1) { //exist
        std::cout << "New Extension: ";
        std::cin.ignore();
        std::cin.getline(emp.extension, 128);
        Statement* stm = conn->createStatement();
        stm->setSQL("UPDATE employees SET extension=:1 WHERE employeenumber=:2");
        stm->setString(1, emp.extension);
        stm->setInt(2, employeeNumber);
        stm->executeUpdate(); //execute the new queries
        conn->commit(); //commit the current statement
        std::cout << "Extension is updated!" << std::endl;
        conn->terminateStatement(stm); //close and free statement object
    }
    else {
        std::cout << "Employee Number " << employeeNumber << " is not existed!" << std::endl;
    }
}
void deleteEmployee(Connection* conn, int employeeNumber) {
    struct Employee emp;
    if (findEmployee(conn, employeeNumber, &emp) == 1) {
        Statement* stm = conn->createStatement();
        stm->setSQL("DELETE FROM employees WHERE employeenumber = :1");
        stm->setInt(1, employeeNumber);
        stm->executeUpdate();  //execute the new queries
        conn->commit(); //commit the current statement
        std::cout << "The employee is deleted." << std::endl;
        conn->terminateStatement(stm); //close and free statement object
    }
    else {
        std::cout << "The employee does not exist." << std::endl;
    }
}
