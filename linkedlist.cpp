#include<iostream>
using namespace std;
class Node{
public:
    int data;
    Node* next;

    Node(int val){
        data =val;
        next=NULL;
    }

};
class list{
    Node* Head;
    Node* Tail;
public:
    list(){
        Head=Tail=NULL;
    } 
    void push_front(int val){
        Node* newNode= new Node(val); //dynamic
        
        if(Head == NULL){
            Head = Tail = newNode;
            return;
        }else{
            newNode -> next = Head;
            Head = newNode;
        }
    }

    void push_back(int val){
        Node* newNode=new Node(val);
        if(Head == NULL){
            Head = Tail = newNode;
            return;
        }else{
            Tail -> next = newNode;
            Tail =newNode;
        }
    }

    void insert(int val, int pos){
        if(pos<0){
            return;
        }
        if(pos==0){
            push_front(val);
            return;
        }
        Node* temp=Head;
        for(int i=0; i<pos-1; i++){
            if(temp==NULL){
                return;
            }
            temp = temp -> next;
        }
        Node* newNode = new Node(val);
        newNode -> next= temp -> next;
        temp->next = newNode;

    }

    void search(int val){
      //  if(Head -> data ==val){
       //     cout << "found" <<endl;
       //     return;
       // }
        Node* temp=Head;
        while(temp != NULL){
            if(temp -> data == val){
                cout << "found" << endl;
                return;
            }
            temp= temp-> next;
        }
        cout << "not found" << endl;
        return;
    }

    void pop_front(){
        if(Head == NULL){
            cout << "list is empty" << endl;
            return;
        }
        Node* temp=Head;
        Head= Head->next;
        temp->next=NULL;
        delete temp;
    }

    void pop_back(){
        if(Head == NULL){
            return;
        }
        Node* n=Tail;
        Node* temp=Head;
        while(temp->next != Tail){
            temp=temp->next;
        }
        Tail= temp;
        Tail -> next=NULL;
        delete n;
    }

    void print(){
        Node* temp=Head;
        while(temp!=NULL){
            cout << temp -> data << "->";
            temp=temp -> next;
        }
        cout << "NULL" << endl;
    }
};
int main(){
    list ll;
    ll.push_front(3);
    ll.push_front(2);
    ll.push_front(1);
    
    ll.print();

    ll.insert(4,0);

    ll.print();

    ll.search(4);
    return 0;
}