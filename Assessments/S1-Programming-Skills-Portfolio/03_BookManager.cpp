#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

class Book
{
private:
    string title, author, id;
    int pages;
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
    void setBorrowed(bool status)
    {
        borrowed = status;
    }

    void display()
    {
        cout << "\nTitle: " << title;
        cout << "\nAuthor: " << author;
        cout << "\nPages: " << pages;
        cout << "\nID: " << id;

        if (borrowed)
            cout << "\nStatus: Borrowed\n";
        else
            cout << "\nStatus: Available\n";
    }

    string toFile()
    {
        string status = "false";

        if (borrowed)
            status = "true";

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

        bool borrowed = false;

        if (status == "true")
            borrowed = true;

        Book book(title, author, stoi(pagesText), id, borrowed);
        books.push_back(book);
    }

    return books;
}

void saveBooks(vector<Book> books)
{
    ofstream file("bookData.txt");

    for (int i = 0; i < books.size(); i++)
        file << books[i].toFile() << endl;
}

void showBooks(vector<Book> books)
{
    for (int i = 0; i < books.size(); i++)
        books[i].display();
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

    if (books.empty())
    {
        cout << "No books were loaded.\n";
        return 1;
    }

    int choice = 0;

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
            showBooks(books);
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
                books[position].display();
            }
            else if (choice == 3)
            {
                if (books[position].isBorrowed())
                {
                    cout << "Book is already borrowed.\n";
                }
                else
                {
                    books[position].setBorrowed(true);
                    saveBooks(books);
                    cout << "Book borrowed successfully.\n";
                }
            }
            else
            {
                if (!books[position].isBorrowed())
                {
                    cout << "Book is already available.\n";
                }
                else
                {
                    books[position].setBorrowed(false);
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