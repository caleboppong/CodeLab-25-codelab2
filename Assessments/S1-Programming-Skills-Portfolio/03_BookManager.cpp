#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

class Book
{
private:
    string title;
    string author;
    int pages;
    string id;
    bool borrowed;

public:
    Book(string t, string a, int p, string i, bool b)
    {
        title = t;
        author = a;
        pages = p;
        id = i;
        borrowed = b;
    }

    string getId()
    {
        return id;
    }

    bool isBorrowed()
    {
        return borrowed;
    }

    void borrowBook()
    {
        borrowed = true;
    }

    void returnBook()
    {
        borrowed = false;
    }

    void displayBook()
    {
        cout << "\nTitle: " << title << endl;
        cout << "Author: " << author << endl;
        cout << "Pages: " << pages << endl;
        cout << "ID: " << id << endl;

        if (borrowed)
            cout << "Status: Borrowed\n";
        else
            cout << "Status: Available\n";
    }

    string getFileData()
    {
        string status;

        if (borrowed)
            status = "true";
        else
            status = "false";

        return title + "," + author + "," +
               to_string(pages) + "," + id + "," + status;
    }
};

vector<Book> loadBooks()
{
    vector<Book> books;
    ifstream file("bookData.txt");
    string line;

    if (!file)
    {
        cout << "Error opening bookData.txt\n";
        return books;
    }

    while (getline(file, line))
    {
        string title, author, pagesText, id, status;
        stringstream data(line);

        getline(data, title, ',');
        getline(data, author, ',');
        getline(data, pagesText, ',');
        getline(data, id, ',');
        getline(data, status);

        int pages = stoi(pagesText);
        bool borrowed = false;

        if (status == "true")
            borrowed = true;

        Book newBook(title, author, pages, id, borrowed);
        books.push_back(newBook);
    }

    file.close();
    return books;
}

void saveBooks(vector<Book> books)
{
    ofstream file("bookData.txt");

    for (int i = 0; i < books.size(); i++)
    {
        file << books[i].getFileData() << endl;
    }

    file.close();
}

void showAllBooks(vector<Book> books)
{
    for (int i = 0; i < books.size(); i++)
    {
        books[i].displayBook();
    }
}

int findBook(vector<Book> books, string id)
{
    for (int i = 0; i < books.size(); i++)
    {
        if (books[i].getId() == id)
            return i;
    }

    return -1;
}

int main()
{
    vector<Book> books = loadBooks();
    int choice = 0;

    if (books.empty())
    {
        cout << "No books were loaded.\n";
        return 1;
    }

    while (choice != 5)
    {
        cout << "\n=== Book Manager ===\n";
        cout << "1. View all books\n";
        cout << "2. View book by ID\n";
        cout << "3. Borrow book\n";
        cout << "4. Return book\n";
        cout << "5. Quit\n";
        cout << "Choose an option: ";
        cin >> choice;

        if (choice == 1)
        {
            showAllBooks(books);
        }
        else if (choice >= 2 && choice <= 4)
        {
            string id;
            cout << "Enter book ID: ";
            cin >> id;

            int position = findBook(books, id);

            if (position == -1)
            {
                cout << "Book not found.\n";
            }
            else if (choice == 2)
            {
                books[position].displayBook();
            }
            else if (choice == 3)
            {
                if (books[position].isBorrowed())
                {
                    cout << "Book is already borrowed.\n";
                }
                else
                {
                    books[position].borrowBook();
                    saveBooks(books);
                    cout << "Book borrowed successfully.\n";
                }
            }
            else if (choice == 4)
            {
                if (!books[position].isBorrowed())
                {
                    cout << "Book is already available.\n";
                }
                else
                {
                    books[position].returnBook();
                    saveBooks(books);
                    cout << "Book returned successfully.\n";
                }
            }
        }
        else if (choice != 5)
        {
            cout << "Please choose an option from 1 to 5.\n";
        }
    }

    cout << "Book Manager closed.\n";

    return 0;
}