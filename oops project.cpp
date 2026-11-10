#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// ---------------------------------------------------------------
// Attachment: a file attached to an email
// ---------------------------------------------------------------
class Attachment {
private:
    string fileName;
    string mimeType;
    size_t sizeBytes;

public:
    Attachment(const string& name, const string& type, size_t size)
        : fileName(name), mimeType(type), sizeBytes(size) {}

    const string& getFileName() const { return fileName; }
    const string& getMimeType() const { return mimeType; }
    size_t getSizeBytes() const { return sizeBytes; }

    void display() const {
        cout << "    [Attachment] " << fileName << " (" << mimeType << ", "
             << sizeBytes / 1024.0 << " KB)\n";
    }
};

// ---------------------------------------------------------------
// Email: a single message with optional attachments
// ---------------------------------------------------------------
class Email {
private:
    static int nextId;
    static const size_t MAX_ATTACHMENT_BYTES = 25 * 1024 * 1024;  // 25 MB total

    int id;
    string sender;
    vector<string> recipients;
    string subject;
    string body;
    time_t timestamp;
    bool read;
    bool starred;
    vector<Attachment> attachments;

public:
    Email(const string& from, const vector<string>& to,
          const string& subj, const string& text)
        : id(nextId++), sender(from), recipients(to), subject(subj),
          body(text), timestamp(time(nullptr)), read(false), starred(false) {}

    // Getters
    int getId() const { return id; }
    const string& getSender() const { return sender; }
    const vector<string>& getRecipients() const { return recipients; }
    const string& getSubject() const { return subject; }
    const string& getBody() const { return body; }
    bool isRead() const { return read; }
    bool isStarred() const { return starred; }
    const vector<Attachment>& getAttachments() const { return attachments; }

    // Actions
    void markRead() { read = true; }
    void markUnread() { read = false; }
    void toggleStar() { starred = !starred; }

    size_t totalAttachmentSize() const {
        size_t total = 0;
        for (const auto& a : attachments) total += a.getSizeBytes();
        return total;
    }

    void addAttachment(const Attachment& a) {
        if (totalAttachmentSize() + a.getSizeBytes() > MAX_ATTACHMENT_BYTES)
            throw runtime_error("Attachment limit (25 MB) exceeded");
        attachments.push_back(a);
    }

    bool matches(const string& keyword) const {
        auto contains = [&](const string& text) {
            return text.find(keyword) != string::npos;
        };
        return contains(sender) || contains(subject) || contains(body);
    }

    void display(bool full = false) const {
        string t = ctime(&timestamp);
        t.pop_back();  // remove trailing newline
        cout << (read ? "  " : "* ") << (starred ? "[S] " : "    ")
             << "#" << id << " | From: " << sender
             << " | " << subject << " | " << t;
        if (!attachments.empty()) cout << " | " << attachments.size() << " attachment(s)";
        cout << "\n";

        if (full) {
            cout << "    To: ";
            for (size_t i = 0; i < recipients.size(); i++)
                cout << recipients[i] << (i + 1 < recipients.size() ? ", " : "");
            cout << "\n    Body: " << body << "\n";
            for (const auto& a : attachments) a.display();
        }
    }
};
int Email::nextId = 1;

// ---------------------------------------------------------------
// Folder: a named collection of emails
// ---------------------------------------------------------------
class Folder {
private:
    string name;
    vector<shared_ptr<Email>> emails;

public:
    explicit Folder(const string& n) : name(n) {}

    const string& getName() const { return name; }
    size_t count() const { return emails.size(); }

    size_t unreadCount() const {
        return count_if(emails.begin(), emails.end(),
                        [](const shared_ptr<Email>& e) { return !e->isRead(); });
    }

    void addEmail(shared_ptr<Email> e) { emails.push_back(e); }

    // Removes and returns the email (so it can be moved elsewhere)
    shared_ptr<Email> removeEmail(int id) {
        auto it = find_if(emails.begin(), emails.end(),
                          [id](const shared_ptr<Email>& e) { return e->getId() == id; });
        if (it == emails.end()) return nullptr;
        shared_ptr<Email> e = *it;
        emails.erase(it);
        return e;
    }

    shared_ptr<Email> findEmail(int id) const {
        for (const auto& e : emails)
            if (e->getId() == id) return e;
        return nullptr;
    }

    vector<shared_ptr<Email>> search(const string& keyword) const {
        vector<shared_ptr<Email>> results;
        for (const auto& e : emails)
            if (e->matches(keyword)) results.push_back(e);
        return results;
    }

    void display() const {
        cout << "\n=== " << name << " (" << count() << " emails, "
             << unreadCount() << " unread) ===\n";
        if (emails.empty()) cout << "  (empty)\n";
        for (const auto& e : emails) e->display();
    }
};

// ---------------------------------------------------------------
// EmailClient: manages the account and its folders
// ---------------------------------------------------------------
class EmailClient {
private:
    string userAddress;
    map<string, Folder> folders;

    Folder& getFolder(const string& name) {
        auto it = folders.find(name);
        if (it == folders.end()) throw runtime_error("Folder not found: " + name);
        return it->second;
    }

public:
    explicit EmailClient(const string& address) : userAddress(address) {
        for (const string n : {"Inbox", "Sent", "Drafts", "Trash"})
            folders.emplace(n, Folder(n));
    }

    void createFolder(const string& name) {
        if (folders.count(name)) throw runtime_error("Folder already exists");
        folders.emplace(name, Folder(name));
    }

    // Compose a draft (saved in Drafts until sent)
    shared_ptr<Email> compose(const vector<string>& to, const string& subject,
                              const string& body) {
        auto e = make_shared<Email>(userAddress, to, subject, body);
        getFolder("Drafts").addEmail(e);
        return e;
    }

    // Send a draft: moves it from Drafts to Sent
    void send(int emailId) {
        auto e = getFolder("Drafts").removeEmail(emailId);
        if (!e) throw runtime_error("Draft not found");
        e->markRead();
        getFolder("Sent").addEmail(e);
        cout << "Email #" << emailId << " sent.\n";
    }

    // Simulates receiving an email from someone else
    void receive(shared_ptr<Email> e) { getFolder("Inbox").addEmail(e); }

    void moveEmail(int emailId, const string& from, const string& to) {
        Folder& src = getFolder(from);
        Folder& dst = getFolder(to);
        auto e = src.removeEmail(emailId);
        if (!e) throw runtime_error("Email not found in " + from);
        dst.addEmail(e);
    }

    // Delete: first time goes to Trash, from Trash it's gone for good
    void deleteEmail(int emailId, const string& from) {
        if (from == "Trash") {
            if (!getFolder("Trash").removeEmail(emailId))
                throw runtime_error("Email not found in Trash");
        } else {
            moveEmail(emailId, from, "Trash");
        }
    }

    void readEmail(int emailId, const string& folder) {
        auto e = getFolder(folder).findEmail(emailId);
        if (!e) throw runtime_error("Email not found");
        e->markRead();
        e->display(true);
    }

    void search(const string& keyword) {
        cout << "\nSearch results for \"" << keyword << "\":\n";
        bool found = false;
        for (auto& [name, folder] : folders) {
            for (const auto& e : folder.search(keyword)) {
                cout << "  [" << name << "] ";
                e->display();
                found = true;
            }
        }
        if (!found) cout << "  No matches.\n";
    }

    void showFolder(const string& name) { getFolder(name).display(); }
};

// ---------------------------------------------------------------
// Demo
// ---------------------------------------------------------------
int main() {
    EmailClient client("me@example.com");

    try {
        // Receive some emails
        auto mail1 = make_shared<Email>("prof@college.edu", vector<string>{"me@example.com"},
                                        "Assignment deadline", "Submit the OOP assignment by Friday.");
        mail1->addAttachment(Attachment("assignment.pdf", "application/pdf", 204800));

        auto mail2 = make_shared<Email>("team@hackathon.in", vector<string>{"me@example.com"},
                                        "Round 2 details", "Venue and schedule attached.");
        mail2->addAttachment(Attachment("schedule.png", "image/png", 512000));
        mail2->addAttachment(Attachment("venue.pdf", "application/pdf", 102400));

        client.receive(mail1);
        client.receive(mail2);
        client.showFolder("Inbox");

        // Read an email
        cout << "\n--- Reading email #" << mail1->getId() << " ---\n";
        client.readEmail(mail1->getId(), "Inbox");

        // Compose, attach, and send
        auto draft = client.compose({"prof@college.edu"}, "Re: Assignment deadline",
                                    "Thank you, sir. Will submit on time.");
        draft->addAttachment(Attachment("draft_notes.txt", "text/plain", 2048));
        client.showFolder("Drafts");
        client.send(draft->getId());

        // Custom folder + move
        client.createFolder("Hackathon");
        client.moveEmail(mail2->getId(), "Inbox", "Hackathon");

        // Delete (to Trash)
        client.deleteEmail(mail1->getId(), "Inbox");

        // Search across all folders
        client.search("assignment");

        client.showFolder("Inbox");
        client.showFolder("Sent");
        client.showFolder("Hackathon");
        client.showFolder("Trash");

        // Error handling demo: attachment over the limit
        cout << "\n--- Testing attachment limit ---\n";
        mail2->addAttachment(Attachment("huge.zip", "application/zip", 30 * 1024 * 1024));
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << "\n";
    }

    return 0;
}