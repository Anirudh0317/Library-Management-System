# Library Management System

A comprehensive C++ application designed to manage library operations efficiently. This system provides functionality for managing books, users, borrowing operations, and fine calculations in an academic library setting.

## Table of Contents

1. [Features](#features)
2. [System Architecture](#system-architecture)
3. [Initialization](#initialization)
4. [Installation](#installation)

## Features

### User Management

- **Multi-role support**:
  - Students (limited borrowing privileges)
  - Faculty (extended borrowing privileges)
  - Librarians (administrative access)
- **Authentication system**
  - ID and password required to login.
- **Role-based access control**
  - Only Librarian can access all users and manage them.
  - Only Librarian can access manage books.

### Rules for Borrowing

- Borrowing will be based on ISBN code of book.
- **Student**:

  - Can borrow a maximum of 3 books without returning.
  - Due date is 15 days after the borrow date.
  - Fine amount of rupees 10 per day applies after due date.
  - Have to return books before paying fine.

- **Faculty**:
  - Can borrow a maximum of 5 books without returning.
  - Due date is 30 days after the borrow date.
  - No fine applies.
  - Have to return the book(s) before 60 days of borrow date (30 days after due date ~ Overdue period) before borrowing a new book.

### Book Management

- **Comprehensive book cataloging**:
  - Title,Author,Publisher,Publication Year,ISBN
  - All of the above details will be mentioned without spaces in the books.txt file.
  - There must be no commas in any of the 5 attributes of book, if required - & must be used in place of comma.

### User Management

- **Comprehensive user cataloging**:
  - ID,Username,password
  - All of the above details will be mentioned without spaces in the studens.txt, faculty.txt and librarian.txt files.
  - There must be no commas in any of the 3 attributes of user, if required - & must be used in place of comma.

### Tracking Borrows

- **Comprehensive borrow cataloging**:
  - ISBN,borrow date,due date,Returned/Not Returned
  - Returned date and due date are standard times in seconds recorded from the year 1970, these is just for system reference for storage.

### Fine Management

- **Automatic fine calculation**:
- **Daily fine rate for students**: 10 rupees per day
- **Fine clearance system**: Clears user fines

## System Architecture

### Core Components

1. **User Class Hierarchy**

   - Base User class
   - Student derived class
   - Faculty derived class
   - Librarian derived class

2. **Book Management**

   - Book class
   - Inventory tracking
   - Status management

3. **Account Management**

   - Borrowing records
   - Fine calculation
   - History tracking

4. **Library Management**
   - Central control system
   - Data persistence
   - Operation coordination

## Initialization

### 10 books:

- Title: Gora  
  Author: Rabindranath Tagore  
  Publisher: Macmillan  
  Year of Publication: 1910  
  ISBN: 978-0333904771

- Title: Nirmala  
  Author: Munshi Premchand  
  Publisher: Diamond Books  
  Year of Publication: 1928  
  ISBN: 978-8128811982

- Title: Untouchable  
  Author: Mulk Raj Anand  
  Publisher: Penguin Books  
  Year of Publication: 1935  
  ISBN: 978-0143027805

- Title: Introduction to Computer Application  
  Author: Ashok Arora  
  Publisher: Vikas Publishing  
  Year of Publication: 2022  
  ISBN: 978-9354534317

- Title: Theory of Automata and Formal Languages  
  Author: Anshu Kumar Dwivedi  
  Publisher: Vikas Publishing  
  Year of Publication: 2016  
  ISBN: 978-9325984523

- Title: Multimedia Programming  
  Author: Dr. Siddhartha Bhattacharyya & Dr. Paramartha Dutta  
  Publisher: Vikas Publishing  
  Year of Publication: 2016  
  ISBN: 978-9325984479

- Title: Computer Fundamentals  
  Author: Ashok Arora  
  Publisher: Vikas Publishing  
  Year of Publication: 2015  
  ISBN: 978-9325992276

- Title: Managing Big Data  
  Author: Dr. Chandrakant Naikodi  
  Publisher: Vikas Publishing  
  Year of Publication: 2015  
  ISBN: 978-9325984561

- Title: A First Course in Computers  
  Author: Sanjay Saxena  
  Publisher: Vikas Publishing  
  Year of Publication: 2015  
  ISBN: 978-9325980822

- Title: Database Management System  
  Author: Safa Hamdare, Asim Kumar Sen & Bhavesh Pandya  
  Publisher: Vikas Publishing  
  Year of Publication: 2015  
  ISBN: 978-9325984370

### 5 Students:

- ID: S001  
  Username: Ravi  
  Password: student1

- ID: S002  
  Username: Arun  
  Password: student2

- ID: S003  
  Username: Meera  
  Password: student3

- ID: S004  
  Username: Kiran  
  Password: student4

- ID: S005  
  Username: Deepa  
  Password: student5

### 3 Faculty:

- ID: F001  
  Username: Anil  
  Password: faculty1

- ID: F002  
  Username: Vinay  
  Password: faculty2

- ID: F003  
  Username: Neeraj  
  Password: faculty3

### 1 Librarian

- ID: L001  
  Username: Prakash  
  Password: librarian1

## Installation

### System Requirements

- C++ compiler (C++11 or later)

### Setup Process

1. **Download all the following files and keep them in the same directory**:

   - LMS.cpp
   - books.txt
   - students.txt
   - faculty.txt
   - librarian.txt
   - borrowing.txt

1. **Compile the source code**:
   ```bash
   g++ LMS.cpp -o LMS
   ./LMS
   ```
