
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

//  ATTACHMENT CLASS 
class Attachment {
    string fileName;
    double fileSize;

public:
    Attachment(string name, double size) {
        fileName = name;
        fileSize = size;
    }

    void displayAttachment() const {
        cout << "  Attachment: " << fileName
             << " (" << fileSize << " KB)" << endl;
    }
};

// EMAIL CLASS 
class Email {
    int emailId;
    string sender, receiver, subject, message;
    vector<Attachment> attachments;

public:
    Email(int id, string s, string r,
          string sub, string msg) {
        emailId = id;
        sender = s;
        receiver = r;
        subject = sub;
        message = msg;
    }

    int getId() const {
        return emailId;
    }

    string getSender() const {
        return sender;
    }

    string getSubject() const {
        return subject;
    }

    void addAttachment(Attachment a) {
        attachments.push_back(a);
    }

    void displayEmail() const {
        cout << "\nEmail ID: " << emailId;
        cout << "\nFrom: " << sender;
        cout << "\nTo: " << receiver;
        cout << "\nSubject: " << subject;
        cout << "\nMessage: " << message << endl;

        if (attachments.empty()) {
            cout << "No attachments." << endl;
        }

        for (const auto &a : attachments) {
            a.displayAttachment();
        }

        cout << "-----------------------------\n";
    }
};

// FOLDER CLASS 
class Folder {
    string folderName;
    vector<Email> emails;

public:
    Folder(string name) {
        folderName = name;
    }

    string getName() const {
        return folderName;
    }

    void addEmail(Email e) {
        emails.push_back(e);
    }

    bool removeEmail(int id) {
        for (auto it = emails.begin();
             it != emails.end(); ++it) {

            if (it->getId() == id) {
                emails.erase(it);
                return true;
            }
        }
        return false;
    }

    Email* findEmail(int id) {
        for (auto &e : emails) {
            if (e.getId() == id) {
                return &e;
            }
        }
        return nullptr;
    }

    void displayFolder() const {
        cout << "\n========== " << folderName
             << " ==========\n";

        if (emails.empty()) {
            cout << "Folder is empty.\n";
            return;
        }

        for (const auto &e : emails) {
            e.displayEmail();
        }
    }

    vector<Email>& getEmails() {
        return emails;
    }
};

// EMAIL CLIENT CLASS 
class EmailClient {
    string userEmail;
    vector<Folder> folders;
    int nextId;

    Folder* getFolder(string name) {
        for (auto &f : folders) {
            if (f.getName() == name) {
                return &f;
            }
        }
        return nullptr;
    }

    void composeEmail(bool send) {
        string receiver, subject, message;
        int choice;

        cout << "\nEnter receiver email: ";
        getline(cin, receiver);

        cout << "Enter subject: ";
        getline(cin, subject);

        cout << "Enter message: ";
        getline(cin, message);

        Email e(nextId++, userEmail, receiver,
                subject, message);

        cout << "Add attachment? (1 = Yes, 0 = No): ";
        cin >> choice;
        cin.ignore();

        while (choice == 1) {
            string name;
            double size;

            cout << "Enter file name: ";
            getline(cin, name);

            cout << "Enter file size (KB): ";
            cin >> size;
            cin.ignore();

            if (size < 0) {
                cout << "Invalid file size.\n";
            } else {
                e.addAttachment(Attachment(name, size));
            }

            cout << "Add another attachment? (1 = Yes, 0 = No): ";
            cin >> choice;
            cin.ignore();
        }

        if (send) {
            getFolder("Sent")->addEmail(e);
            cout << "\nEmail sent successfully (simulated)!\n";
        } else {
            getFolder("Drafts")->addEmail(e);
            cout << "\nEmail saved to Drafts!\n";
        }
    }

    void searchEmail() {
        string keyword;
        bool found = false;

        cout << "\nEnter sender or subject to search: ";
        getline(cin, keyword);

        for (auto &f : folders) {
            for (auto &e : f.getEmails()) {

                if (e.getSender().find(keyword) != string::npos ||
                    e.getSubject().find(keyword) != string::npos) {

                    cout << "\nFound in folder: "
                         << f.getName() << endl;

                    e.displayEmail();
                    found = true;
                }
            }
        }

        if (!found) {
            cout << "No matching emails found.\n";
        }
    }

    void deleteEmail() {
        int id;

        cout << "Enter Email ID to delete: ";
        cin >> id;
        cin.ignore();

        Folder* trash = getFolder("Trash");

        for (auto &f : folders) {
            if (f.getName() == "Trash")
                continue;

            Email* e = f.findEmail(id);

            if (e != nullptr) {
                Email copy = *e;
                f.removeEmail(id);
                trash->addEmail(copy);

                cout << "Email moved to Trash.\n";
                return;
            }
        }

        cout << "Email not found.\n";
    }

    void createFolder() {
        string name;

        cout << "Enter new folder name: ";
        getline(cin, name);

        if (name.empty() || getFolder(name) != nullptr) {
            cout << "Invalid or existing folder name.\n";
            return;
        }

        folders.push_back(Folder(name));
        cout << "Folder created successfully!\n";
    }

    void viewFolders() {
        cout << "\nAvailable Folders:\n";

        for (auto &f : folders) {
            cout << "- " << f.getName()
                 << " (" << f.getEmails().size()
                 << " emails)\n";
        }

        string name;
        cout << "\nEnter folder name to open: ";
        getline(cin, name);

        Folder* f = getFolder(name);

        if (f != nullptr) {
            f->displayFolder();
        } else {
            cout << "Folder not found.\n";
        }
    }

    void receiveEmail() {
        string sender, subject, message;

        cout << "Enter sender email: ";
        getline(cin, sender);

        cout << "Enter subject: ";
        getline(cin, subject);

        cout << "Enter message: ";
        getline(cin, message);

        Email e(nextId++, sender, userEmail,
                subject, message);

        getFolder("Inbox")->addEmail(e);

        cout << "Received email added to Inbox.\n";
    }

    void moveEmail() {
        int id;
        string sourceName, destinationName;

        cout << "Enter source folder: ";
        getline(cin, sourceName);

        cout << "Enter destination folder: ";
        getline(cin, destinationName);

        Folder* source = getFolder(sourceName);
        Folder* destination = getFolder(destinationName);

        if (source == nullptr || destination == nullptr ||
            source == destination) {
            cout << "Invalid folder selection.\n";
            return;
        }

        cout << "Enter Email ID: ";
        cin >> id;
        cin.ignore();

        Email* e = source->findEmail(id);

        if (e == nullptr) {
            cout << "Email not found.\n";
            return;
        }

        Email copy = *e;
        source->removeEmail(id);
        destination->addEmail(copy);

        cout << "Email moved successfully!\n";
    }

public:
    EmailClient(string email) {
        userEmail = email;
        nextId = 1;

        folders.push_back(Folder("Inbox"));
        folders.push_back(Folder("Sent"));
        folders.push_back(Folder("Drafts"));
        folders.push_back(Folder("Trash"));
    }

    void run() {
        int choice;

        do {
            cout << "\n========== EMAIL CLIENT ==========\n";
            cout << "Logged in as: " << userEmail << endl;

            cout << "\n1. Compose and Send Email";
            cout << "\n2. Save Email as Draft";
            cout << "\n3. View Folders";
            cout << "\n4. Search Email";
            cout << "\n5. Delete Email";
            cout << "\n6. Create New Folder";
            cout << "\n7. Move Email";
            cout << "\n8. Add Received Email";
            cout << "\n9. Exit";

            cout << "\n\nEnter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    composeEmail(true);
                    break;

                case 2:
                    composeEmail(false);
                    break;

                case 3:
                    viewFolders();
                    break;

                case 4:
                    searchEmail();
                    break;

                case 5:
                    deleteEmail();
                    break;

                case 6:
                    createFolder();
                    break;

                case 7:
                    moveEmail();
                    break;

                case 8:
                    receiveEmail();
                    break;

                case 9:
                    cout << "Exiting Email Client. Goodbye!\n";
                    break;

                default:
                    cout << "Invalid choice. Try again.\n";
            }

        } while (choice != 9);
    }
};

// ---------------- MAIN FUNCTION ----------------
int main() {
    string email;

    cout << "Enter your email address: ";
    getline(cin, email);

    EmailClient client(email);
    client.run();

    return 0;
}
