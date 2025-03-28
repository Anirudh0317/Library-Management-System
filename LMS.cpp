#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <string>
#include <iomanip>
#include <string>
#include <limits>

using namespace std;

// Faculty borrowing rules
const int MAX_STUDENT_BORROW = 3;
const int STUDENT_BORROW_DAYS = 15;
const int STUDENT_FINE_PER_DAY = 10;
const int STUDENT_MAX_OVERDUE_DAYS = 30;

// Faculty borrowing rules
const int MAX_FACULTY_BORROW = 5;
const int FACULTY_BORROW_DAYS = 30;
const int FACULTY_FINE_PER_DAY = 0;
const int FACULTY_MAX_OVERDUE_DAYS = 30;

// Time constants
const int SECONDS_PER_DAY = 86400;

class Library;
class User;
class Librarian;

// CLASSES ................................................................................

class Book
{
private:
    string title;
    string author;
    string publisher;
    string year;
    string ISBN;
    string status;

public:
    Book(const string &tit, const string &auth, const string &pub, const string &yr, const string &isbn)
    {
        title = tit;
        author = auth;
        publisher = pub;
        year = yr;
        ISBN = isbn;
        status = "Available";
    }
    string get_isbn()
    {
        return ISBN;
    }
    void set_isbn(string i)
    {
        ISBN = i;
    }
    string get_Title()
    {
        return title;
    }
    void set_Title(string t)
    {
        title = t;
    }
    string get_Author()
    {
        return author;
    }
    void set_Author(string a)
    {
        author = a;
    }
    string get_Year()
    {
        return year;
    }
    void set_year(string y)
    {
        year = y;
    }
    string get_Publisher()
    {
        return publisher;
    }
    void set_pub(string p)
    {
        publisher = p;
    }
    string get_Status()
    {
        return status;
    }
    void set_Status(string new_status)
    {
        status = new_status;
    }
    bool isAvailable()
    {
        if (status == "Available")
        {
            return true;
        }
        else
            return false;
    }
};

struct Borrow_record
{
    string isbn;
    time_t borrow_date;
    time_t due_date;
    time_t return_date;
    bool returned;
    int fine;
};

class Account
{
private:
    string user_id;
    vector<Borrow_record> currentBorrows;
    vector<Borrow_record> borrowHistory;
    int Total_Fine = 0;
    vector<int> unpaidFines;

public:
    Account(const string &userId)
    {
        user_id = userId;
        Total_Fine;
    }
    string get_ID()
    {
        return user_id;
    }
    bool borrowBook(string &isbn, int borrow_duration)
    {
        Borrow_record newbook;
        newbook.isbn = isbn;
        newbook.borrow_date = time(nullptr);
        newbook.due_date = newbook.borrow_date + (borrow_duration * SECONDS_PER_DAY);
        newbook.returned = false;
        newbook.fine = 0;
        currentBorrows.push_back(newbook);
        return true;
    }
    bool returnBook(string &isbn) {
        for (auto it = currentBorrows.begin(); it != currentBorrows.end(); ++it) {
            if (it->isbn == isbn && !it->returned) {
                
                time_t current_time = time(nullptr);
                if (current_time > it->due_date) {
                    double days_overdue = difftime(current_time, it->due_date) / SECONDS_PER_DAY;
                    int whole_days_overdue = static_cast<int>(days_overdue);
                    int fine_rate = (user_id[0] == 'S') ? STUDENT_FINE_PER_DAY : FACULTY_FINE_PER_DAY;
                    int finalFine = whole_days_overdue * fine_rate;
                    
                    if (finalFine > 0) {
                        unpaidFines.push_back(finalFine);
                        Total_Fine += finalFine;
                    }
                }

                it->returned = true;
                it->return_date = current_time;

                Borrow_record historyRecord = *it;
                borrowHistory.push_back(historyRecord);

                currentBorrows.erase(it);
                return true;
            }
        }
        return false;
    }

    int getTotalFine() {
        update_Fine();
        return Total_Fine;
    }
    bool clearFine()
    {
        for (auto book : currentBorrows)
        {
            if (!book.returned)
            {
                cout << "You need to return books before you pay fines" << endl;
                return false;
            }
        }
        for (const auto& book : currentBorrows) {
            if (!book.returned) {
                cout << "You need to return all books before clearing fines" << endl;
                return false;
            }
        }
        
        unpaidFines.clear();
        Total_Fine = 0;
        return true;
    }

    vector<Borrow_record> &get_currently_borrowed()
    {
        return currentBorrows;
    }
    vector<Borrow_record> &get_borrow_history()
    {
        return borrowHistory;
    }
    int get_current_borrow_count()
    {
        return currentBorrows.size();
    }

    void update_Fine() {
        time_t current_time = time(nullptr);
        
        Total_Fine = 0;
        for (int fine : unpaidFines) {
            Total_Fine += fine;
        }

        for (auto &book : currentBorrows) {
            if (!book.returned && current_time > book.due_date) {
                double days_overdue = difftime(current_time, book.due_date) / SECONDS_PER_DAY;
                int whole_days_overdue = static_cast<int>(days_overdue);
                int fine_rate = (user_id[0] == 'S') ? STUDENT_FINE_PER_DAY : FACULTY_FINE_PER_DAY;
                book.fine = whole_days_overdue * fine_rate;
                Total_Fine += book.fine;
            }
        }
    }   
    bool any_books_Overdue(int days)
    {
        time_t current_time = time(nullptr);

        for (auto &book : currentBorrows)
        {
            if (!book.returned)
            {
                double days_overdue = difftime(current_time, book.due_date) / SECONDS_PER_DAY;
                int whole_days_overdue = static_cast<int>(days_overdue);
                if (whole_days_overdue > days)
                {
                    return true;
                }
            }
        }
        return false;
    }
    void view_currently_borrowed()
    {
        bool found = false;
        cout << "\nCurrently Borrowed Books:" << endl;
        cout << "------------------------" << endl;
        for (const auto &book : currentBorrows)
        {
            update_Fine();
            if (!book.returned)
            {
                cout << "ISBN: " << book.isbn << endl;
                cout << "Borrow Date: " << ctime(&book.borrow_date);
                cout << "Due Date: " << ctime(&book.due_date);
                cout << "Fine: " << book.fine << endl;
                cout << "------------------------" << endl;
                found = true;
            }
        }
        if (!found)
        {
            cout << "No currently borrowed books.\n";
        }
    }
    void view_borrow_history()
    {
        bool found = false;
        cout << "\nBorrow History:" << endl;
        cout << "------------------------" << endl;
        for (const auto &book : borrowHistory)
        {
            cout << "ISBN: " << book.isbn << endl;
            cout << "Borrow Date: " << ctime(&book.borrow_date);
            cout << "Return Date: " << ctime(&book.return_date);
            cout << "------------------------" << endl;
            found = true;
        }
        if (!found)
        {
            cout << "No history to show.\n";
        }
    }
};

class User
{
protected:
    string username;
    string id;
    string password;
    string role;

protected:
    Account account;
    int borrow_limit;

public:
    User(const string &ID, const string &UserName, const string &pwd) : account(ID)
    {
        id = ID;
        username = UserName;
        password = pwd;
    }
    virtual ~User() = default;
    string getUsername()
    {
        return username;
    }
    void setUsername(string un)
    {
        username = un;
    }
    string getID()
    {
        return id;
    }
    void setID(string ID)
    {
        id = ID;
    }
    string getRole()
    {
        return role;
    }
    void setRole(string ROLE)
    {
        role = ROLE;
    }
    string getPassword()
    {
        return password;
    }
    void setPassword(string pwd)
    {
        password = pwd;
    }
    Account &getAccount()
    {
        return account;
    }
    bool validatePassword(string pwd)
    {
        return pwd == password;
    }
    void showDetails()
    {
        cout << "User ID: " << id << endl;
        cout << "Username: " << username << endl;
        cout << "User Role: " << role << endl;
    }
    virtual bool borrowBook(string isbn) = 0;
    virtual bool returnBook(string isbn) = 0;
};

class Student : public User
{
private:
    static const int borrow_books_limit = MAX_STUDENT_BORROW;
    static const int borrow_days_limit = STUDENT_BORROW_DAYS;
    int fine_per_day = STUDENT_FINE_PER_DAY;

public:
    Student(const string &id, const string &name, const string &pwd) : User(id, name, pwd)
    {
        role = "Student";
    }

    int get_max_books()
    {
        return borrow_books_limit;
    }
    int get_max_borrow_duration()
    {
        return borrow_days_limit;
    }
    double total_fine(int days)
    {
        double ans = STUDENT_FINE_PER_DAY * days;
        return ans;
    }

    Account &get_account()
    {
        return account;
    }
    bool borrowBook(string isbn)
    {
        if (get_account().getTotalFine())
        {
            cout << "Clear pending fines before borrowing a new book" << endl;
            return false;
        }
        if (get_account().any_books_Overdue(FACULTY_MAX_OVERDUE_DAYS))
        {
            cout << "New books cannot be borrowed without returning overdue books" << endl;
            return false;
        }
        cout << "Student " << username << " successfully borrowed book with ISBN: " << isbn << endl;
        return true;
    }

    bool returnBook(string isbn) override
    {
        cout << "Student " << username << " returned book with ISBN: " << isbn << endl;
        return true;
    }
};

class Faculty : public User
{
private:
    static const int borrow_books_limit = MAX_FACULTY_BORROW;
    static const int borrow_days_limit = FACULTY_BORROW_DAYS;
    static const int max_overdue_days = FACULTY_MAX_OVERDUE_DAYS;
    const int fine_per_day = 0;

public:
    Faculty(const string &ID, const string &UserName, const string &pwd) : User(ID, UserName, pwd)
    {
        role = "Faculty";
    }

    int get_borrow_limit()
    {
        return borrow_books_limit;
    }
    int get_max_borrow_duration()
    {
        return borrow_days_limit;
    }
    int get_max_overdue_days()
    {
        return max_overdue_days;
    }
    int Total_Fine(int days)
    {
        return 0;
    }

    Account &get_account()
    {
        return account;
    }

    bool any_overdue_books()
    {
        return get_account().any_books_Overdue(FACULTY_MAX_OVERDUE_DAYS);
    }
    bool canBorrow()
    {
    }
    bool borrowBook(string isbn) override
    {
        cout << "Faculty " << username << " borrowed book with ID: " << isbn << endl;
        return true;
    }

    bool returnBook(string isbn) override
    {
        cout << "Faculty " << username << " returned book with ID: " << isbn << endl;
        return true;
    }
};

class Library
{
private:
    vector<Book *> books;
    vector<User *> users;
    vector<Borrow_record> currentborrows;
    string currentUserID;

public:
    Library()
    {
        loadBooksFromFile();
        loadUsersFromFile();
        loadBorrowingRecordsFromFile();
    };
    ~Library()
    {
        saveBooksToFile();
        saveUsersToFile();
        saveBorrowingRecordsToFile();
        clean();
    };
    void clean()
    {
        for (auto book : books)
            delete book;
        for (auto user : users)
            delete user;
        for (auto record : currentborrows)
            delete &record;
    }
    User *getCurrentUser()
    {
        if (currentUserID.empty())
            return nullptr;

        for (auto user : users)
        {
            if (user->getID() == currentUserID)
            {
                return user;
            }
        }
        return nullptr;
    }

    bool login(const string &userID, const string &pwd)
    {
        for (auto user : users)
        {
            if (user->getID() == userID)
            {
                if (pwd == user->getPassword())
                {
                    currentUserID = userID;
                    return true;
                }
                else
                {
                    cout << "Invalid Password" << endl;
                    return false;
                }
            }
        }
        cout << "Invalid ID" << endl;
        return false;
    }
    void logout()
    {
        currentUserID = "";
    }
    void saveBooksToFile()
    {
        ofstream booksfile("books.txt");
        for (auto book : books)
        {
            booksfile << book->get_Title() << "," << book->get_Author() << "," << book->get_Publisher() << ","
                      << book->get_Year() << "," << book->get_isbn() << "," << book->get_Status() << "\n";
        }
        booksfile.close();
    }
    void saveUsersToFile()
    {
        ofstream studentFile("students.txt");
        ofstream facultyFile("faculty.txt");
        ofstream librarianFile("librarian.txt");

        for (auto user : users)
        {
            if (user->getRole() == "Student")
            {
                studentFile << user->getID() << ","
                            << user->getUsername() << ","
                            << user->getPassword() << endl;
            }
            else if (user->getRole() == "Faculty")
            {
                facultyFile << user->getID() << ","
                            << user->getUsername() << ","
                            << user->getPassword() << endl;
            }
            else if (user->getRole() == "Librarian")
            {
                librarianFile << user->getID() << ","
                              << user->getUsername() << ","
                              << user->getPassword() << endl;
            }
        }

        studentFile.close();
        facultyFile.close();
        librarianFile.close();
    }

    void saveBorrowingRecordsToFile()
    {
        ofstream borrowfile("borrowing.txt");
        if (!borrowfile)
        {
            cerr << "Error: Cannot open borrowing.txt for writing\n";
            return;
        }

        for (const auto &user : users)
        {
            const auto &userBorrows = user->getAccount().get_currently_borrowed();
            for (const auto &record : userBorrows)
            {
                borrowfile << user->getID() << ","
                           << record.isbn << ","
                           << record.borrow_date << ","
                           << record.due_date << ","
                           << (record.returned ? "Returned" : "Not Returned") << "\n";
            }

            const auto &userHistory = user->getAccount().get_borrow_history();
            for (const auto &record : userHistory)
            {
                borrowfile << user->getID() << ","
                           << record.isbn << ","
                           << record.borrow_date << ","
                           << record.due_date << ","
                           << "Returned" << "\n";
            }
        }

        borrowfile.close();
    }

    void loadBooksFromFile()
    {
        ifstream booksfile("books.txt");
        string title, author, publisher, year, ISBN, status;
        while (getline(booksfile, title, ',') && getline(booksfile, author, ',') &&
               getline(booksfile, publisher, ',') && getline(booksfile, year, ',') &&
               getline(booksfile, ISBN, ',') && getline(booksfile, status))
        {
            Book *book = new Book(title, author, publisher, year, ISBN);
            book->set_Status(status);
            books.push_back(book);
        }
        booksfile.close();
    }

    void loadBorrowingRecordsFromFile()
    {
        ifstream borrowingfile("borrowing.txt");
        if (!borrowingfile)
        {
            cerr << "Note: No existing borrowing records found.\n";
            return;
        }

        string line;
        while (getline(borrowingfile, line))
        {
            stringstream ss(line);
            string userId, isbn, borrow_date_str, due_date_str, returned_str, fine_str;

            try
            {
                if (!getline(ss, userId, ',') ||
                    !getline(ss, isbn, ',') ||
                    !getline(ss, borrow_date_str, ',') ||
                    !getline(ss, due_date_str, ',') ||
                    !getline(ss, returned_str))
                {

                    continue;
                }

                getline(ss, fine_str);

                Borrow_record record;
                record.isbn = isbn;

                try
                {
                    record.borrow_date = static_cast<time_t>(stoull(borrow_date_str));
                    record.due_date = static_cast<time_t>(stoull(due_date_str));
                }
                catch (const exception &e)
                {
                    cerr << "Error converting dates in record: " << line << "\n";
                    cerr << "Error details: " << e.what() << endl;
                    continue;
                }

                record.returned = (returned_str == "1" || returned_str == "true");

                try
                {
                    record.fine = fine_str.empty() ? 0 : stoi(fine_str);
                }
                catch (const exception &e)
                {
                    cerr << "Error converting fine amount: " << fine_str << endl;
                    record.fine = 0;
                }

                User *user = findUser(userId);
                if (!user)
                {
                    cerr << "Warning: User ID " << userId << " not found. Skipping record.\n";
                    continue;
                }

                Account &acc = user->getAccount();
                if (record.returned)
                {
                    acc.get_borrow_history().push_back(record);
                }
                else
                {
                    acc.get_currently_borrowed().push_back(record);
                }
            }
            catch (const exception &e)
            {
                cerr << "Error processing record: " << line << "\n";
                cerr << "Error details: " << e.what() << endl;
                continue;
            }
        }
        borrowingfile.close();
    }
    void viewLibrary()
    {
        if (books.empty())
        {
            cout << "No books available in the library." << endl;
            return;
        }

        // Define column widths
        const int titleWidth = 40;
        const int authorWidth = 30;
        const int publisherWidth = 25;
        const int yearWidth = 10;
        const int isbnWidth = 20;
        const int statusWidth = 15;

        cout << "\nBooks Available in Library:\n";
        cout << string(titleWidth + authorWidth + publisherWidth + yearWidth + isbnWidth + statusWidth + 7, '-') << endl;

        cout << left
             << setw(titleWidth) << "| Title"
             << setw(authorWidth) << "| Author"
             << setw(publisherWidth) << "| Publisher"
             << setw(yearWidth) << "| Year"
             << setw(isbnWidth) << "| ISBN"
             << setw(statusWidth) << "| Status" << "|" << endl;

        cout << string(titleWidth + authorWidth + publisherWidth + yearWidth + isbnWidth + statusWidth + 7, '-') << endl;

        for (auto &book : books)
        {
            cout << "| " << left
                 << setw(titleWidth - 2) << truncateString(book->get_Title(), titleWidth - 2)
                 << "| " << setw(authorWidth - 2) << truncateString(book->get_Author(), authorWidth - 2)
                 << "| " << setw(publisherWidth - 2) << truncateString(book->get_Publisher(), publisherWidth - 2)
                 << "| " << setw(yearWidth - 2) << truncateString(book->get_Year(), yearWidth - 2)
                 << "| " << setw(isbnWidth - 2) << truncateString(book->get_isbn(), isbnWidth - 2)
                 << "| " << setw(statusWidth - 2) << truncateString(book->get_Status(), statusWidth - 2)
                 << "|" << endl;
        }

        cout << string(titleWidth + authorWidth + publisherWidth + yearWidth + isbnWidth + statusWidth + 7, '-') << endl;
    }

    string truncateString(string str, size_t width)
    {
        if (str.length() > width)
        {
            return str.substr(0, width - 3) + "...";
        }
        return str;
    }
    void viewAvailable()
    {
        bool found = false;

        // Define column widths
        const int titleWidth = 40;
        const int authorWidth = 30;
        const int publisherWidth = 25;
        const int yearWidth = 10;
        const int isbnWidth = 20;
        const int statusWidth = 15;

        const int totalWidth = titleWidth + authorWidth + publisherWidth +
                               yearWidth + isbnWidth + statusWidth + 7;

        cout << "\nAvailable Books in Library:\n";
        cout << string(totalWidth, '-') << endl;

        cout << left
             << "| " << setw(titleWidth - 2) << "Title"
             << "| " << setw(authorWidth - 2) << "Author"
             << "| " << setw(publisherWidth - 2) << "Publisher"
             << "| " << setw(yearWidth - 2) << "Year"
             << "| " << setw(isbnWidth - 2) << "ISBN"
             << "| " << setw(statusWidth - 2) << "Status"
             << "|" << endl;

        cout << string(totalWidth, '-') << endl;

        for (auto &book : books)
        {
            if (book->get_Status() == "Available")
            {
                cout << "| " << left
                     << setw(titleWidth - 2) << truncateString(book->get_Title(), titleWidth - 2)
                     << "| " << setw(authorWidth - 2) << truncateString(book->get_Author(), authorWidth - 2)
                     << "| " << setw(publisherWidth - 2) << truncateString(book->get_Publisher(), publisherWidth - 2)
                     << "| " << setw(yearWidth - 2) << truncateString(book->get_Year(), yearWidth - 2)
                     << "| " << setw(isbnWidth - 2) << truncateString(book->get_isbn(), isbnWidth - 2)
                     << "| " << setw(statusWidth - 2) << truncateString(book->get_Status(), statusWidth - 2)
                     << "|" << endl;
                found = true;
            }
        }

        if (!found)
        {
            cout << "|" << setw(totalWidth - 1) << center_text("No books available right now", totalWidth - 2) << "|" << endl;
        }

        cout << string(totalWidth, '-') << endl;
    }

    string center_text(const string &text, size_t width)
    {
        size_t spaces = width - text.length();
        size_t left_spaces = spaces / 2;
        size_t right_spaces = spaces - left_spaces;
        return string(left_spaces, ' ') + text + string(right_spaces, ' ');
    }

    void viewBorrowed()
    {
        bool found = false;

        const int titleWidth = 40;
        const int authorWidth = 30;
        const int publisherWidth = 25;
        const int yearWidth = 10;
        const int isbnWidth = 20;
        const int statusWidth = 15;

        const int totalWidth = titleWidth + authorWidth + publisherWidth +
                               yearWidth + isbnWidth + statusWidth + 7;

        cout << "\nCurrently Borrowed Books:\n";
        cout << string(totalWidth, '-') << endl;

        cout << left
             << "| " << setw(titleWidth - 2) << "Title"
             << "| " << setw(authorWidth - 2) << "Author"
             << "| " << setw(publisherWidth - 2) << "Publisher"
             << "| " << setw(yearWidth - 2) << "Year"
             << "| " << setw(isbnWidth - 2) << "ISBN"
             << "| " << setw(statusWidth - 2) << "Status"
             << "|" << endl;

        cout << string(totalWidth, '-') << endl;

        for (auto &book : books)
        {
            if (book->get_Status() == "Borrowed")
            {
                cout << "| " << left
                     << setw(titleWidth - 2) << truncateString(book->get_Title(), titleWidth - 2)
                     << "| " << setw(authorWidth - 2) << truncateString(book->get_Author(), authorWidth - 2)
                     << "| " << setw(publisherWidth - 2) << truncateString(book->get_Publisher(), publisherWidth - 2)
                     << "| " << setw(yearWidth - 2) << truncateString(book->get_Year(), yearWidth - 2)
                     << "| " << setw(isbnWidth - 2) << truncateString(book->get_isbn(), isbnWidth - 2)
                     << "| " << setw(statusWidth - 2) << truncateString(book->get_Status(), statusWidth - 2)
                     << "|" << endl;
                found = true;
            }
        }

        if (!found)
        {
            cout << "|" << setw(totalWidth - 1) << center_text("No books borrowed right now", totalWidth - 2) << "|" << endl;
        }

        cout << string(totalWidth, '-') << endl;
    }

    bool addBook(Book *book)
    {
        books.push_back(book);
        saveBooksToFile();
        return true;
    }
    bool removeBook(string isbn)
    {
        for (auto it = books.begin(); it != books.end(); ++it)
        {
            if ((*it)->get_isbn() == isbn)
            {
                delete *it;
                books.erase(it);
                saveBooksToFile();
                return true;
            }
        }
        return false;
    }
    User *findUser(const string &userID)
    {
        for (auto &user : users)
        {
            if (user->getID() == userID)
            {
                return user;
            }
        }
        return nullptr;
    }

    bool addUser(User *user)
    {
        users.push_back(user);
        saveUsersToFile();
        return true;
    }
    bool removeUser(string &id)
    {
        for (auto it = users.begin(); it != users.end(); ++it)
        {
            if ((*it)->getID() == id)
            {
                if ((*it)->getRole() == "Librarian")
                {
                    return false;
                }
                delete *it;
                users.erase(it);
                saveUsersToFile();
                return true;
            }
        }
        return false;
    }
    Book *findBook(const string &ISBN)
    {
        for (auto &book : books)
        {
            if (book->get_isbn() == ISBN)
            {
                return book;
            }
        }
        return nullptr;
    }

    bool borrowBook(string &ISBN)
    {
        Book *book = findBook(ISBN);
        User *user = findUser(currentUserID);

        if (!user || !book)
        {
            cout << "Book not found" << endl;
            return false;
        }

        if (user->getRole() == "Student")
        {
            user->getAccount().update_Fine();
            if (user->getAccount().getTotalFine())
            {
                cout << "Clear fines to borrow new books" << endl;
                return false;
            }
        }

        if (user->getRole() == "Faculty")
        {
            if (user->getAccount().any_books_Overdue(FACULTY_MAX_OVERDUE_DAYS))
            {
                cout<<"You have one or more overdue books for more than 30 days, return them to borrow a new book"<<endl;
                return false;
            }
        }

        if (user->getRole() == "Student" && user->getAccount().get_current_borrow_count() == MAX_STUDENT_BORROW)
        {
            cout << "Borrow Limit reached" << endl;
            return false;
        }
        if (user->getRole() == "Faculty" && user->getAccount().get_current_borrow_count() == MAX_FACULTY_BORROW)
        {
            cout << "Borrow Limit reached" << endl;
            return false;
        }

        if (book->get_Status() != "Available")
        {
            cout << "Book not available to borrow" << endl;
            return false;
        }

        bool success = false;
        int borrowDuration = 0;

        if (auto student = dynamic_cast<Student *>(user))
        {
            borrowDuration = STUDENT_BORROW_DAYS;
            success = student->borrowBook(ISBN);
        }
        else if (auto faculty = dynamic_cast<Faculty *>(user))
        {
            borrowDuration = FACULTY_BORROW_DAYS;
            success = faculty->borrowBook(ISBN);
        }

        if (success)
        {
            book->set_Status("Borrowed");

            Borrow_record newRecord;
            newRecord.isbn = ISBN;
            newRecord.borrow_date = time(nullptr);
            newRecord.due_date = newRecord.borrow_date + (borrowDuration * SECONDS_PER_DAY);
            newRecord.returned = false;
            newRecord.fine = 0;

            user->getAccount().get_currently_borrowed().push_back(newRecord);

            saveBorrowingRecordsToFile();
            saveBooksToFile();
            return true;
        }
        return false;
    }
    bool returnBook(string &ISBN)
    {
        Book *book = findBook(ISBN);
        User *user = findUser(currentUserID);

        if (!user || !book)
        {
            cout << "Book not found" << endl;
            return false;
        }

        bool success = false;

        if (auto student = dynamic_cast<Student *>(user))
        {
            student->get_account().update_Fine();
            success = user->getAccount().returnBook(ISBN);
        }
        else if (auto faculty = dynamic_cast<Faculty *>(user))
        {
            success = user->getAccount().returnBook(ISBN);
        }
        if (success)
        {
            book->set_Status("Available");
            cout << "Book is returned. Thank you!" << endl;
            user->getAccount().update_Fine();
            saveBorrowingRecordsToFile();
            saveBooksToFile();
            return true;
        }
        cout << "Book ISBN invalid or book was not found in your current borrows" << endl;
        return false;
    }
    void loadUsersFromFile();

    void viewUsers()
    {
        if (users.empty())
        {
            cout << "No users found" << endl;
            return;
        }

        const int idWidth = 15;
        const int usernameWidth = 30;
        const int roleWidth = 15;
        const int fineWidth = 15;

        const int totalWidth = idWidth + usernameWidth + roleWidth + fineWidth + 7; // +7 for separators

        cout << "\nRegistered Users in Library:\n";
        cout << string(totalWidth, '-') << endl;

        cout << left
             << "| " << setw(idWidth - 2) << "User ID"
             << "| " << setw(usernameWidth - 2) << "Username"
             << "| " << setw(roleWidth - 2) << "Role"
             << "| " << setw(fineWidth - 2) << "Total Fine"
             << "|" << endl;

        cout << string(totalWidth, '-') << endl;

        for (auto &user : users)
        {
            user->getAccount().update_Fine();
            cout << "| " << left
                 << setw(idWidth - 2) << truncateString(user->getID(), idWidth - 2)
                 << "| " << setw(usernameWidth - 2) << truncateString(user->getUsername(), usernameWidth - 2)
                 << "| " << setw(roleWidth - 2) << truncateString(user->getRole(), roleWidth - 2)
                 << "| " << setw(fineWidth - 2) << user->getAccount().getTotalFine()
                 << "|" << endl;
        }

        cout << string(totalWidth, '-') << endl;
    }
    void displayUserFines()
    {
        if (users.empty())
        {
            cout << "No users found" << endl;
            return;
        }
        cout << "--------------------------------" << endl;
        cout << "User ID\t|\tTotal Fine" << endl;
        cout << "--------------------------------" << endl;
        for (auto &user : users)
        {
            user->getAccount().update_Fine();
            cout << user->getID() << "\t|\t" << user->getAccount().getTotalFine() << endl;
        }
    }
};

class Librarian : public User
{
private:
    Library *library;

public:
    Librarian(const string &id, const string &name, const string &pwd) : User(id, name, pwd), library(nullptr)
    {
        role = "Librarian";
    }

    void add_Book(string t, string a, string p, string yr, string isbn)
    {
        Book *temp = new Book(t, a, p, yr, isbn);
        library->addBook(temp);
        library->saveBooksToFile();
    }
    void setLibrary(Library *lib)
    {
        library = lib;
    }
    void remove_Book(string isbn)
    {
        library->removeBook(isbn);
        library->saveBooksToFile();
    }
    bool borrowBook(string isbn)
    {
        cout << "Librarian can't borrow books" << endl;
        return false;
    }
    bool returnBook(string isbn)
    {
        return true;
    }
};

void Library::loadUsersFromFile()
{
    for (auto user : users)
    {
        delete user;
    }
    users.clear();

    bool filesExist = false;
    {
        ifstream checkStudent("students.txt");
        ifstream checkFaculty("faculty.txt");
        ifstream checkLibrarian("librarian.txt");

        if (checkStudent.good() || checkFaculty.good() || checkLibrarian.good())
        {
            filesExist = true;
        }

        checkStudent.close();
        checkFaculty.close();
        checkLibrarian.close();
    }

    if (!filesExist)
    {
        cout << "No user files found. Creating new user database." << endl;
        saveUsersToFile();
        return;
    }

    ifstream studentFile("students.txt");
    if (studentFile)
    {
        string line;
        while (getline(studentFile, line))
        {
            stringstream ss(line);
            string id, name, password;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, password);

            users.push_back(new Student(id, name, password));
        }
        studentFile.close();
    }

    ifstream facultyFile("faculty.txt");
    if (facultyFile)
    {
        string line;
        while (getline(facultyFile, line))
        {
            stringstream ss(line);
            string id, name, password;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, password);

            users.push_back(new Faculty(id, name, password));
        }
        facultyFile.close();
    }

    ifstream librarianFile("librarian.txt");
    if (librarianFile)
    {
        string line;
        while (getline(librarianFile, line))
        {
            stringstream ss(line);
            string id, name, password;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, password);

            Librarian *librarian = new Librarian(id, name, password);
            librarian->setLibrary(this);
            users.push_back(librarian);
        }
        librarianFile.close();
    }
}
// END OF CLASSES...................................................................................

// FUNCTIONS USED IN MAIN...........................................................................
void pauseExecution()
{
    cout << "\nPress Enter to continue, or '0' to logout: ";
    string input;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, input);
    if (input == "0")
    {
        throw runtime_error("logout");
    }
}
void pauseExecution_default()
{
    cout << "\nPress Enter to continue: ";
    string input;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, input);
}
string getInputLine(const string &prompt)
{
    string input;
    cout << prompt;

    if (cin.peek() == '\n')
    {
        cin.ignore();
    }

    getline(cin, input);

    if (input.empty())
    {
        cin >> input;
    }

    return input;
}
void displayDefaultMenu()
{
    cout << "------------------------------------------------" << endl;
    cout << "     Welcome to Library Management System" << endl;
    cout << "------------------------------------------------" << endl;
    cout << "                     Menu" << endl;
    cout << "                     ----" << endl;
    cout << "1. Login" << endl;
    cout << "2. View all books" << endl;
    cout << "3. View available books" << endl;
    cout << "4. Exit" << endl;
    cout << "Choose one of the options (Enter an option number): " << endl;
}

void displayMainMenu(User *user)
{
    cout << "-----------------------------------" << endl;
    cout << "              Welcome!             " << endl;
    cout << "             Main Menu             " << endl;
    cout << "-----------------------------------" << endl;
    if (user->getRole() == "Student" || user->getRole() == "Faculty")
    {
        cout << "1. Display all Books " << endl;
        cout << "2. Display Available Books " << endl;
        cout << "3. Borrow a Book" << endl;
        cout << "4. Return a Book" << endl;
        cout << "5. View my Borrow History" << endl;
        cout << "6. View my Currently borrowed books" << endl;
        cout << "7. Calculate my Total fines" << endl;
        cout << "8. Pay fines" << endl;
    }
    if (user->getRole() == "Librarian")
    {
        cout << "1. Display all Books " << endl;
        cout << "2. Display Available Books " << endl;
        cout << "3. Display Borrowed Books " << endl;
        cout << "4. Display all Users" << endl;
        cout << "5. Manage Books" << endl;
        cout << "6. Manage Users" << endl;
        cout << "7. Display user fines" << endl;
    }
    cout << "0. Logout" << endl;
    cout << "Enter your choice: " << endl;
}

void StudentFacultyResponse(Library &lib, User *currentuser, int choice)
{
    try
    {
        string code;
        bool f;
        switch (choice)
        {
        case 1:
            lib.viewLibrary();
            pauseExecution();
            break;
        case 2:
            lib.viewAvailable();
            pauseExecution();
            break;
        case 3:
            cout << "Enter ISBN of the book you want to borrow: " << endl;
            cin >> code;
            lib.borrowBook(code);
            pauseExecution();
            break;
        case 4:
            cout << "Enter ISBN of the book you want to return: " << endl;
            cin >> code;
            lib.returnBook(code);
            pauseExecution();
            break;
        case 5:
            currentuser->getAccount().view_borrow_history();
            pauseExecution();
            break;
        case 6:
            currentuser->getAccount().view_currently_borrowed();
            pauseExecution();
            break;
        case 7:
            cout << "Total fine: " << currentuser->getAccount().getTotalFine() << endl;
            pauseExecution();
            break;
        case 8:
            f = currentuser->getAccount().clearFine();
            if (f)
            {
                cout << "Fines cleared" << endl;
            }
            pauseExecution();
            break;
        case 0:
            lib.logout();
            break;
        default:
            cout << "Invalid Choice" << endl;
            pauseExecution();
            break;
        }
    }
    catch (runtime_error &e)
    {
        if (string(e.what()) == "logout")
        {
            lib.logout();
        }
    }
}

void LibrarianResponse(Library &lib, int choice)
{
    try
    {
        string code;
        string new_user_id, new_user_username, new_user_pwd, new_user_role;
        string edit_user_id, edit_user_username, edit_user_pwd, edit_user_role;
        string remove_user_id;
        string book_title, book_auth, book_pub, book_isbn, book_year, book_status;

        switch (choice)
        {
        case 1:
            lib.viewLibrary();
            pauseExecution();
            break;
        case 2:
            lib.viewAvailable();
            pauseExecution();
            break;
        case 3:
            lib.viewBorrowed();
            pauseExecution();
            break;
        case 4:
            lib.viewUsers();
            pauseExecution();
            break;
        case 6:
        {
            cout << "Options: " << endl;
            cout << "1. Add user" << endl;
            cout << "2. Edit user" << endl;
            cout << "3. Remove user" << endl;
            cout << "4. Exit" << endl;
            cout << "Enter your choice: ";
            int in;
            cin >> in;
            switch (in)
            {
            case 1:
                cout << "Enter ID of new user: ";
                cin >> new_user_id;
                new_user_username = getInputLine("Enter Name of new user: ");
                cout << "Enter password of new user: ";
                cin >> new_user_pwd;
                cout << "Enter the role of the new user (Student/Faculty/Librarian): ";
                cin >> new_user_role;
                if (new_user_role == "Student")
                {
                    lib.addUser(new Student(new_user_id, new_user_username, new_user_pwd));
                    cout << "User added succesfully" << endl;
                }
                else if (new_user_role == "Faculty")
                {
                    lib.addUser(new Faculty(new_user_id, new_user_username, new_user_pwd));
                    cout << "User added succesfully" << endl;
                }
                else if (new_user_role == "Librarian")
                {
                    Librarian *librarian = new Librarian(new_user_id, new_user_username, new_user_pwd);
                    librarian->setLibrary(&lib);
                    lib.addUser(librarian);
                    cout << "User added succesfully" << endl;
                }
                else
                {
                    cout << "Invalid role entered" << endl;
                }
                pauseExecution();
                break;
            case 2:
            {
                cout << "Enter ID of the User you want to edit: ";
                cin >> edit_user_id;
                User *edit_user = lib.findUser(edit_user_id);
                if (!edit_user)
                {
                    cout << "Invalid User ID" << endl;
                    break;
                }
                cout << "Enter new ID: ";
                cin >> edit_user_id;
                edit_user_username = getInputLine("Enter New Name: ");
                cout << "Enter new password: ";
                cin >> edit_user_pwd;

                edit_user->setID(edit_user_id);
                edit_user->setUsername(edit_user_username);
                edit_user->setPassword(edit_user_pwd);

                cout << "Succesfully edited user" << endl;
                pauseExecution();
                break;
            }
            case 3:
            {
                cout << "Enter the ID of the user you want to remove: ";
                cin >> remove_user_id;
                User *remove_user = lib.findUser(remove_user_id);
                if (!remove_user)
                {
                    cout << "Invalid User ID" << endl;
                    break;
                }
                if (remove_user->getRole() == "Librarian")
                {
                    cout << "Librarian cannot be removed" << endl;
                    break;
                }
                lib.removeUser(remove_user_id);
                cout << "User removed successfully" << endl;
                pauseExecution();
                break;
            }
            default:
                cout << "Invalid choice" << endl;
                pauseExecution();
                break;
            }
            break;
        }
        case 5:
        {
            cout << "Options: " << endl;
            cout << "1. Add Book" << endl;
            cout << "2. Edit Book" << endl;
            cout << "3. Remove Book" << endl;
            cout << "4. Exit" << endl;
            cout << "Enter your choice: ";
            int in;
            cin >> in;
            switch (in)
            {
            case 1:
            {
                string book_title, book_auth, book_pub, book_isbn, book_year;

                book_title = getInputLine("Enter Title of new Book: ");
                book_auth = getInputLine("Enter Author of new Book: ");
                book_pub = getInputLine("Enter Publisher of new Book: ");
                book_isbn = getInputLine("Enter ISBN of new Book: ");
                book_year = getInputLine("Enter Year of new Book: ");

                lib.addBook(new Book(book_title, book_auth, book_pub, book_year, book_isbn));
                cout << "Book added successfully" << endl;
                pauseExecution();
                break;
            }
            case 2:
            {
                cout << "Enter ISBN of the Book you want to edit: ";
                cin >> book_isbn;
                Book *edit_book = lib.findBook(book_isbn);
                if (!edit_book)
                {
                    cout << "Invalid Book ISBN" << endl;
                    break;
                }
                cout << "Enter new ISBN: ";
                cin >> book_isbn;
                book_title = getInputLine("Enter new Title: ");
                book_auth = getInputLine("Enter new author: ");
                book_pub = getInputLine("Enter new Publisher: ");
                cout << "Enter new Year: ";
                cin >> book_year;
                cout << "Enter status (Available/Reserved): ";
                cin >> book_status;

                edit_book->set_isbn(book_isbn);
                edit_book->set_pub(book_pub);
                edit_book->set_Status(book_status);
                edit_book->set_Title(book_title);
                edit_book->set_year(book_year);
                edit_book->set_Author(book_auth);

                cout << "Succesfully edited Book" << endl;
                pauseExecution();
                break;
            }
            case 3:
            {
                cout << "Enter the ISBN of the Book you want to remove: ";
                cin >> book_isbn;
                Book *remove_book = lib.findBook(book_isbn);
                if (!remove_book)
                {
                    cout << "Invalid Book ISBN" << endl;
                    break;
                }
                lib.removeBook(book_isbn);
                cout << "Book removed successfully" << endl;
                pauseExecution();
                break;
            }
            default:
                cout << "Invalid choice" << endl;
                pauseExecution();
                break;
            }
            break;
        }

        case 7:
            lib.displayUserFines();
            pauseExecution();
            break;
        case 0:
            lib.logout();
            break;
        default:
            cout << "Invalid Choice" << endl;
            pauseExecution();
            break;
        }
    }
    catch (runtime_error &e)
    {
        if (string(e.what()) == "logout")
        {
            lib.logout();
        }
    }
}
// END OF FUNCTIONS USED IN MAIN.........................................................................

// MAIN FUNCTION..........................................................................................
int main()
{
    Library lib;

    while (true)
    {
        try
        {
            if (lib.getCurrentUser() == nullptr)
            {
                displayDefaultMenu();
                int choice;
                std::cin >> choice;

                switch (choice)
                {
                case 1:
                {
                    string userID, password;
                    cout << "Enter User ID: ";
                    cin >> userID;
                    cout << "Enter Password: ";
                    cin >> password;

                    if (lib.login(userID, password))
                    {
                        cout << "Login successful!\n";
                    }
                    break;
                }

                case 2:
                    lib.viewLibrary();
                    pauseExecution_default();
                    break;

                case 3:
                    lib.viewAvailable();
                    pauseExecution_default();
                    break;

                case 4:
                    cout << "Thank you for using the Library Management System!\n";
                    return 0;

                default:
                    cout << "Invalid choice.\n";
                    pauseExecution_default();
                    break;
                }
            }
            else
            {
                User *currentUser = lib.getCurrentUser();
                displayMainMenu(currentUser);
                int choice;
                cin >> choice;

                if (choice == 0)
                {
                    lib.logout();
                }

                if (currentUser->getRole() == "Student" || currentUser->getRole() == "Faculty")
                {
                    StudentFacultyResponse(lib, currentUser, choice);
                }
                else if (currentUser->getRole() == "Librarian")
                {
                    LibrarianResponse(lib, choice);
                }
            }
        }
        catch (runtime_error &e)
        {
            if (string(e.what()) == "logout")
            {
                lib.logout();
                continue;
            }
        }
    }
    return 0;
}
// END OF MAIN FUNCTION.................................................................................